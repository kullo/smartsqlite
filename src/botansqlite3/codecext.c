/*
 * Encryption codec implementation
 * (C) 2010 Olivier de Gaalon
 * (C) 2016 Daniel Seither (Kullo GmbH)
 *
 * Distributed under the terms of the Botan license
 */

#ifndef SQLITE_OMIT_DISKIO

// This is always defined when building, so it isn't relevant for the build.
// The Qt Creator code model, however, needs these workarounds to understand the code.
#ifndef SQLITE_CORE
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #define SQLITE_HAS_CODEC
    #include "../sqlite3.c"
#endif

// uncomment to enable tracing
//#define BOTANSQLITE_TRACE_ENABLED

#ifdef BOTANSQLITE_TRACE_ENABLED
    #define BOTANSQLITE_TRACE(msg) fprintf(stderr, "TRACE: %s\n", msg)
#else
    #define BOTANSQLITE_TRACE(msg) (void)msg
#endif

// Forward-declare these method that SQLite uses but doesn't declare
int sqlite3CodecAttach(sqlite3*, int, const void*, int);
void sqlite3CodecGetKey(sqlite3*, int, void**, int*);

#include "codec_c_interface.h"

static Bool HandleError(void *pCodec)
{
    if (!pCodec) return 0;

    const char *error = GetError(pCodec);
    if (error)
    {
        sqlite3ErrorWithMsg((sqlite3*)(GetDB(pCodec)), SQLITE_ERROR, "Botan Error: %s", error);
        ResetError(pCodec);
        return 1;
    }
    return 0;
}

// "see" is "SQLite Encryption Extension" (the semi-official, for-pay, encryption codec)
void sqlite3_activate_see(const char *info)
{
    BOTANSQLITE_TRACE("sqlite3_activate_see");
    (void)info;
}

// Free the encryption codec, called from pager.c (address passed in sqlite3PagerSetCodec)
static void PagerFreeCodec(void *pCodec)
{
    BOTANSQLITE_TRACE("sqlite3PagerFreeCodec");
    if (pCodec) DeleteCodec(pCodec);
}

// Report the page size to the codec, called from pager.c (address passed in sqlite3PagerSetCodec)
static void CodecSizeChange(void *pCodec, int pageSize, int nReserve)
{
    BOTANSQLITE_TRACE("sqlite3CodecSizeChange");
    (void)nReserve;
    assert(pageSize >= 0);
    SetPageSize(pCodec, (size_t)pageSize);
}

// Encrypt/Decrypt functionality (address passed in sqlite3PagerSetCodec)
static void* Codec(void *pCodec, void *data, Pgno nPageNum, int nMode)
{
    BOTANSQLITE_TRACE("sqlite3Codec");

    // Don't change data if DB is not encrypted
    if (pCodec == NULL) return data;

    switch(nMode)
    {
    case 0: // Undo a "case 7" journal file encryption
    case 2: // Reload a page
    case 3: // Load a page
        if (HasReadKey(pCodec))
        {
            Decrypt(pCodec, nPageNum, (unsigned char*) data);
        }
        break;
    case 6: // Encrypt a page for the main database file
        if (HasWriteKey(pCodec))
        {
            data = Encrypt(pCodec, nPageNum, (unsigned char*) data, true);
        }
        break;
    case 7: // Encrypt a page for the journal file
    /*
    *Under normal circumstances, the readkey is the same as the writekey.  However,
    *when the database is being rekeyed, the readkey is not the same as the writekey.
    *(The writekey is the "destination key" for the rekey operation and the readkey
    *is the key the db is currently encrypted with)
    *Therefore, for case 7, when the rollback is being written, always encrypt using
    *the database's readkey, which is guaranteed to be the same key that was used to
    *read and write the original data.
    */
        if (HasReadKey(pCodec))
        {
            data = Encrypt(pCodec, nPageNum, (unsigned char*) data, false);
        }
        break;
    default:
        assert(false);
    }
    
    HandleError(pCodec);

    return data;
}

int sqlite3CodecAttach(sqlite3 *db, int nDb, const void *zKey, int nKey)
{
    BOTANSQLITE_TRACE("sqlite3CodecAttach");
    void *pCodec = NULL;

    if (zKey == NULL || nKey <= 0)
    {
        // No key specified, could mean either use the main db's encryption or no encryption
        if (nDb != 0 && nKey < 0)
        {
            //Is an attached database, therefore use the key of main database, if main database is encrypted
            void *pMainCodec = sqlite3PagerGetCodec(sqlite3BtreePager(db->aDb[0].pBt));
            if (pMainCodec != NULL)
            {
                pCodec = InitializeFromOtherCodec(pMainCodec, db);
                sqlite3PagerSetCodec(sqlite3BtreePager(db->aDb[nDb].pBt),
                                    Codec,
                                    CodecSizeChange,
                                    PagerFreeCodec, pCodec);
            }
        }
    }
    else
    {
        // Key specified, setup encryption key for database
        pCodec = InitializeNewCodec(db);
        GenerateWriteKey(pCodec, (const char*) zKey, nKey);
        SetReadIsWrite(pCodec);
        sqlite3PagerSetCodec(sqlite3BtreePager(db->aDb[nDb].pBt),
                            Codec,
                            CodecSizeChange,
                            PagerFreeCodec, pCodec);
    }

    if (HandleError(pCodec))
        return SQLITE_ERROR;

    return SQLITE_OK;
}

void sqlite3CodecGetKey(sqlite3* db, int nDb, void **zKey, int *nKey)
{
    BOTANSQLITE_TRACE("sqlite3CodecGetKey");
    (void)db;
    (void)nDb;

    // The unencrypted password is not stored for security reasons
    // therefore always return NULL
    *zKey = NULL;
    *nKey = -1;
}

int sqlite3_key_v2(sqlite3 *db, const char *zDbName, const void *zKey, int nKey)
{
    BOTANSQLITE_TRACE("sqlite3_key_v2");
    //We don't use zDbName (though maybe we could...). Pass-through to the old sqlite_key
    (void)(zDbName);
    return sqlite3_key(db, zKey, nKey);
}

int sqlite3_rekey_v2(sqlite3 *db, const char * zDbName, const void *zKey, int nKey)
{
    BOTANSQLITE_TRACE("sqlite3_rekey_v2");
    //We don't use zDbName (though maybe we could...). Pass-through to the old sqlite_rekey
    (void)(zDbName);
    return sqlite3_rekey(db, zKey, nKey);
}

int sqlite3_key(sqlite3 *db, const void *zKey, int nKey)
{
    BOTANSQLITE_TRACE("sqlite3_key");
    // The key is only set for the main database, not the temp database
    return sqlite3CodecAttach(db, 0, zKey, nKey);
}

int sqlite3_rekey(sqlite3 *db, const void *zKey, int nKey)
{
    BOTANSQLITE_TRACE("sqlite3_rekey");
    // Changes the encryption key for an existing database.
    int rc = SQLITE_ERROR;
    Btree *pbt = db->aDb[0].pBt;
    Pager *pPager = sqlite3BtreePager(pbt);
    void *pCodec = sqlite3PagerGetCodec(pPager);

    if ((zKey == NULL || nKey == 0) && pCodec == NULL)
    {
        // Database not encrypted and key not specified. Do nothing
        return SQLITE_OK;
    }

    if (pCodec == NULL)
    {
        // Database not encrypted, but key specified. Encrypt database
        pCodec = InitializeNewCodec(db);
        GenerateWriteKey(pCodec, (const char*) zKey, nKey);
        
        if (HandleError(pCodec)) return SQLITE_ERROR;

        sqlite3PagerSetCodec(pPager, Codec, CodecSizeChange, PagerFreeCodec, pCodec);
    }
    else if (zKey == NULL || nKey == 0)
    {
        // Database encrypted, but key not specified. Decrypt database
        // Keep read key, drop write key
        DropWriteKey(pCodec);
    }
    else
    {
        // Database encrypted and key specified. Re-encrypt database with new key
        // Keep read key, change write key to new key
        GenerateWriteKey(pCodec, (const char*) zKey, nKey);
        if (HandleError(pCodec)) return SQLITE_ERROR;
    }

    // Start transaction
    rc = sqlite3BtreeBeginTrans(pbt, 1);
    if (rc == SQLITE_OK)
    {
        // Rewrite all pages using the new encryption key (if specified)
        int nPageCount = -1;
        sqlite3PagerPagecount(pPager, &nPageCount);
        Pgno nPage = (Pgno) nPageCount;

        Pgno nSkip = PAGER_MJ_PGNO(pPager);
        DbPage *pPage;

        Pgno n;
        for (n = 1; rc == SQLITE_OK && n <= nPage; n++)
        {
            if (n == nSkip) continue;

            rc = sqlite3PagerGet(pPager, n, &pPage, 0);

            if (!rc)
            {
                rc = sqlite3PagerWrite(pPage);
                sqlite3PagerUnref(pPage);
            }
            else
            {
                sqlite3ErrorWithMsg(db, SQLITE_ERROR, "%s", "Error while rekeying database page. Transaction Canceled.");
            }
        }
    }
    else
        sqlite3ErrorWithMsg(db, SQLITE_ERROR, "%s", "Error beginning rekey transaction. Make sure that the current encryption key is correct.");

    if (rc == SQLITE_OK)
    {
        // All good, commit
        rc = sqlite3BtreeCommit(pbt);

        if (rc == SQLITE_OK)
        {
            //Database rekeyed and committed successfully, update read key
            if (HasWriteKey(pCodec))
                SetReadIsWrite(pCodec);
            else //No write key == no longer encrypted
                sqlite3PagerSetCodec(pPager, NULL, NULL, NULL, NULL); 
        }
        else
        {
            //FIXME: can't trigger this, not sure if rollback is needed, reference implementation didn't rollback
            sqlite3ErrorWithMsg(db, SQLITE_ERROR, "%s", "Could not commit rekey transaction.");
        }
    }
    else
    {
        // Rollback, rekey failed
        sqlite3BtreeRollback(pbt, SQLITE_ERROR, 0);

        // go back to read key
        if (HasReadKey(pCodec))
        {
            SetWriteIsRead(pCodec);
        }
        else //Database wasn't encrypted to start with
        {
            sqlite3PagerSetCodec(pPager, NULL, NULL, NULL, NULL); 
        }
    }

    return rc;
}

#endif // SQLITE_OMIT_DISKIO
