/*
 * Codec class for SQLite3 encryption codec.
 * (C) 2010 Olivier de Gaalon
 * (C) 2016 Daniel Seither (Kullo GmbH)
 *
 * Distributed under the terms of the Botan license
 */

#pragma once

#include <string>
#include <memory>
#include <botan/botan_all.h>

/*These constants can be used to tweak the codec behavior as follows
 *Note that once you've encrypted a database with these settings,
 *recompiling with any different settings will give you a library that
 *cannot read that database, even given the same passphrase.*/

//BLOCK_CIPHER_STR: Cipher and mode used for encrypting the database
//make sure to add "/NoPadding" for modes that use padding schemes
const std::string BLOCK_CIPHER_STR = "AES-256/XTS";

//MAC_STR: CMAC used to derive the IV that is used for db page
//encryption
const std::string MAC_STR = "CMAC(AES-256)";

//KEY_SIZE: Size of the encryption key. Note that XTS splits the key
//between two ciphers, so if you're using XTS, double the intended key
//size. (ie, "AES-128/XTS" should have a 256 bit KEY_SIZE)
const size_t KEY_SIZE = 512/8; //512 bit, 64 byte key. (256 bit XTS key)

//IV_DERIVATION_KEY_SIZE: Size of the key used with the CMAC (MAC_STR)
//above.
const size_t IV_DERIVATION_KEY_SIZE = 256/8; //256 bit, 32 byte key

//This is defined in sqlite3.c and very unlikely to change
#define SQLITE_MAX_PAGE_SIZE 65536

class Codec
{
public:
    Codec(void *db);
    Codec(const Codec *other, void *db);

    void getWriteKey(const char **key, size_t *keyLength);
    void setWriteKey(const char *key, size_t keyLength);
    void dropWriteKey();
    void setWriteIsRead();
    void setReadIsWrite();

    unsigned char* encrypt(unsigned int page, unsigned char *data, bool useWriteKey);
    void decrypt(unsigned int page, unsigned char *data);

    void setPageSize(size_t pageSize) { m_pageSize = pageSize; }

    bool hasReadKey() { return m_hasReadKey; }
    bool hasWriteKey() { return m_hasWriteKey; }
    void* getDB() { return m_db; }
    const char* getError();
    void resetError();

private:
    bool m_hasReadKey = false;
    bool m_hasWriteKey = false;

    std::string m_encodedReadKey;
    std::string m_encodedWriteKey;

    Botan::SymmetricKey
        m_readKey,
        m_writeKey,
        m_ivReadKey,
        m_ivWriteKey;

    std::unique_ptr<Botan::Cipher_Mode> m_encryptor;
    std::unique_ptr<Botan::Cipher_Mode> m_decryptor;
    std::unique_ptr<Botan::MessageAuthenticationCode> m_mac;

    size_t m_pageSize = 0;
    unsigned char m_page[SQLITE_MAX_PAGE_SIZE] = {0};
    void *m_db = nullptr;
    std::unique_ptr<std::string> m_botanErrorMsg;

    Botan::InitializationVector getIVForPage(uint32_t page, bool useWriteKey);
};
