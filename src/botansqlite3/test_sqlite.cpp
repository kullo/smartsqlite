/*
 * Quick and dirty test for SQLite3 encryption codec.
 * (C) 2010 Olivier de Gaalon
 * (C) 2016 Daniel Seither (Kullo GmbH)
 *
 * Distributed under the terms of the Botan license
 */

#define SQLITE_HAS_CODEC 1

#include <stdio.h>

//TODO Remove after updating SQLite (should be fixed)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#include "smartsqlite/sqlite3.h"
#pragma clang diagnostic pop

namespace SQL
{
    static const char * CREATE_TABLE_TEST =
        "create table 'test' (id INTEGER PRIMARY KEY, name TEXT, creationtime TEXT);";
    static const char * CREATE_TABLE_TEST2 =
        "create table 'test2' (id INTEGER PRIMARY KEY, name TEXT, creationtime TEXT);";
    static const char * INSERT_INTO_TEST =
        "INSERT INTO test (name, creationtime) VALUES ('widget', '1st time');\
         INSERT INTO test (name, creationtime) VALUES ('widget', '2nd time');\
         INSERT INTO test (name, creationtime) VALUES ('widget', '3rd time');\
         INSERT INTO test (name, creationtime) VALUES ('widget', '4th time');\
         INSERT INTO test (name, creationtime) VALUES ('widget', '5th time');";
    static const char * INSERT_INTO_TEST2 =
        "INSERT INTO test2 (name, creationtime) VALUES ('widget2', '1st time2');\
         INSERT INTO test2 (name, creationtime) VALUES ('widget2', '2nd time2');\
         INSERT INTO test2 (name, creationtime) VALUES ('widget2', '3rd time2');\
         INSERT INTO test2 (name, creationtime) VALUES ('widget2', '4th time2');\
         INSERT INTO test2 (name, creationtime) VALUES ('widget2', '5th time2');";
    static const char * SELECT_FROM_TEST =
        "SELECT * FROM test;";
    static const char * SELECT_FROM_TEST2 =
        "SELECT * FROM test2;";
};

static int callback(void *extra, int argc, char **argv, char **azColName){
    (void)extra;

    int i;
    fprintf(stderr, "\t");
    for(i=0; i<argc; i++){
        fprintf(stderr, "%s = %s | ", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    fprintf(stderr, "\n");
    return 0;
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    sqlite3 * db;
    const char * key = "anotherkey";
    const char * dbname = "./testdb";
    int keylen = 7;
    char * error=0;

    fprintf(stderr, "Creating Database \"%s\"\n", dbname);
    int rc = sqlite3_open(dbname, &db);
    if (rc != SQLITE_OK) { fprintf(stderr, "Can't open/create database: %s\n", sqlite3_errmsg(db)); return 1; }

    fprintf(stderr, "Keying Database with key \"%s\"\n", key);
    rc = sqlite3_key(db, key, keylen);
    if (rc != SQLITE_OK) { fprintf(stderr, "Can't key database: %s\n", sqlite3_errmsg(db)); return 1; }

    fprintf(stderr, "Creating table \"test\"\n");
    rc = sqlite3_exec(db, SQL::CREATE_TABLE_TEST, 0, 0, &error);
    if (rc != SQLITE_OK) { fprintf(stderr, "SQL error: %s\n", error); return 1; }

    fprintf(stderr, "Creating table \"test2\"\n");
    rc = sqlite3_exec(db, SQL::CREATE_TABLE_TEST2, 0, 0, &error);
    if (rc != SQLITE_OK) { fprintf(stderr, "SQL error: %s\n", error); return 1; }

    fprintf(stderr, "Inserting into table \"test\"\n");
    rc = sqlite3_exec(db, SQL::INSERT_INTO_TEST, 0, 0, &error);
    if (rc != SQLITE_OK) { fprintf(stderr, "SQL error: %s\n", error); return 1; }

    fprintf(stderr, "Inserting into table \"test2\"\n");
    rc = sqlite3_exec(db, SQL::INSERT_INTO_TEST2, 0, 0, &error);
    if (rc != SQLITE_OK) { fprintf(stderr, "SQL error: %s\n", error); return 1; }

    fprintf(stderr, "Closing Database \"%s\"\n", dbname);
    sqlite3_close(db);

    fprintf(stderr, "Opening Database \"%s\"\n", dbname);
    rc = sqlite3_open(dbname, &db);
    if (rc != SQLITE_OK) { fprintf(stderr, "Can't open/create database: %s\n", sqlite3_errmsg(db)); return 1; }

    fprintf(stderr, "Keying Database with key \"%s\"\n", key);
    rc = sqlite3_key(db, key, keylen);
    if (rc != SQLITE_OK) { fprintf(stderr, "Can't key database: %s\n", sqlite3_errmsg(db)); return 1; }

    fprintf(stderr, "Selecting all from test\n");
    rc = sqlite3_exec(db, SQL::SELECT_FROM_TEST, callback, 0, &error);
    if (rc != SQLITE_OK) { fprintf(stderr, "SQL error: %s\n", error); return 1; }

    fprintf(stderr, "Selecting all from test2\n");
    rc = sqlite3_exec(db, SQL::SELECT_FROM_TEST2, callback, 0, &error);
    if (rc != SQLITE_OK) { fprintf(stderr, "SQL error: %s\n", error); return 1; }

    fprintf(stderr, "Closing Database \"%s\"\n", dbname);
    sqlite3_close(db);

    fprintf(stderr, "All Seems Good \n");
    return 0;
}
