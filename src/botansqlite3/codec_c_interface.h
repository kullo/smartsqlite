/*
 * Encryption codec class C interface
 * (C) 2010 Olivier de Gaalon
 * (C) 2016 Daniel Seither (Kullo GmbH)
 *
 * Distributed under the terms of the Botan license
 */
#pragma once

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void* InitializeNewCodec(void *db);

void* InitializeFromOtherCodec(const void *otherCodec, void *db);

void GetWriteKey(void *codec, char **key, int *keyLength);

void SetWriteKey(void *codec, const char *key, int keyLength);

void DropWriteKey(void *codec);

void SetWriteIsRead(void *codec);

void SetReadIsWrite(void *codec);

unsigned char* Encrypt(void *codec, unsigned int page, unsigned char *data, bool useWriteKey);

void Decrypt(void *codec, unsigned int page, unsigned char *data);

void SetPageSize(void *codec, size_t pageSize);

bool HasReadKey(void *codec);

bool HasWriteKey(void *codec);

void* GetDB(void *codec);

const char* GetError(void *codec);

void ResetError(void *codec);

void DeleteCodec(void *codec);

#ifdef __cplusplus
}
#endif
