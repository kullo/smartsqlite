/*
 * Codec class for SQLite3 encryption codec.
 * (C) 2010 Olivier de Gaalon
 * (C) 2016 Daniel Seither (Kullo GmbH)
 *
 * Distributed under the terms of the Botan license
 */

#include <cassert>

#include "codec.h"

Codec::Codec(void *db)
    : m_db(db)
{
    try
    {
        m_encipherFilter = get_cipher(BLOCK_CIPHER_STR, Botan::ENCRYPTION);
        assert(m_encipherFilter);
        m_decipherFilter = get_cipher(BLOCK_CIPHER_STR, Botan::DECRYPTION);
        assert(m_decipherFilter);
        m_mac = Botan::MessageAuthenticationCode::create(MAC_STR);
        m_encipherPipe.append(m_encipherFilter);
        m_decipherPipe.append(m_decipherFilter);
        assert(m_mac);
    }
    catch(Botan::Exception e)
    {
        m_botanErrorMsg.reset(new std::string(e.what()));
    }
}

Codec::Codec(const Codec *other, void *db)
    : Codec(db)
{
    assert(other);
    m_hasReadKey = other->m_hasReadKey;
    m_hasWriteKey = other->m_hasWriteKey;
    m_encodedReadKey = other->m_encodedReadKey;
    m_readKey = other->m_readKey;
    m_ivReadKey = other->m_ivReadKey;
    m_encodedWriteKey = other->m_encodedWriteKey;
    m_writeKey = other->m_writeKey;
    m_ivWriteKey = other->m_ivWriteKey;
}

void Codec::setWriteKey(const char *key, size_t keyLength)
{
    assert(key);
    assert(keyLength > 0);

    auto encodedKey = std::string(key, keyLength);
    Botan::secure_vector<Botan::byte> decodedKey;

    try
    {
        decodedKey = Botan::base64_decode(encodedKey);
    }
    catch (Botan::Exception &e)
    {
        m_botanErrorMsg.reset(new std::string(e.what()));
        return;
    }

    auto expectedKeySize = KEY_SIZE + IV_DERIVATION_KEY_SIZE;
    if (decodedKey.size() != expectedKeySize)
    {
        m_botanErrorMsg.reset(
                    new std::string(
                        std::string("Bad key size. Got ")
                        + std::to_string(keyLength)
                        + " bytes (after base64 decoding), expected "
                        + std::to_string(expectedKeySize)
                        + " bytes."));
        return;
    }

    try
    {
        // store keys in temp vars so that either all or none of the member
        // variables are changed
        auto writeKey = Botan::SymmetricKey(decodedKey.data(), KEY_SIZE);
        auto ivWriteKey = Botan::SymmetricKey(decodedKey.data() + KEY_SIZE, IV_DERIVATION_KEY_SIZE);

        m_encodedWriteKey = encodedKey;
        m_writeKey = writeKey;
        m_ivWriteKey = ivWriteKey;
        m_hasWriteKey = true;
    }
    catch(Botan::Exception e)
    {
        m_botanErrorMsg.reset(new std::string(e.what()));
    }
}

void Codec::getWriteKey(const char **key, size_t *keyLength)
{
    *key = reinterpret_cast<const char *>(m_encodedWriteKey.c_str());
    *keyLength = m_encodedWriteKey.size();
}

void Codec::dropWriteKey()
{
    m_hasWriteKey = false;
}

void Codec::setReadIsWrite()
{
    m_encodedReadKey = m_encodedWriteKey;
    m_readKey = m_writeKey;
    m_ivReadKey = m_ivWriteKey;
    m_hasReadKey = m_hasWriteKey;
}

void Codec::setWriteIsRead()
{
    m_encodedWriteKey = m_encodedReadKey;
    m_writeKey = m_readKey;
    m_ivWriteKey = m_ivReadKey;
    m_hasWriteKey = m_hasReadKey;
}

unsigned char* Codec::encrypt(unsigned int page, unsigned char *data, bool useWriteKey)
{
    assert(data);
    memcpy(m_page, data, m_pageSize);

    try
    {
        m_encipherFilter->set_key(useWriteKey ? m_writeKey : m_readKey);
        m_encipherFilter->set_iv(getIVForPage(page, useWriteKey));
        m_encipherPipe.process_msg(m_page, m_pageSize);
        m_encipherPipe.read(
                    m_page,
                    m_encipherPipe.remaining(Botan::Pipe::LAST_MESSAGE),
                    Botan::Pipe::LAST_MESSAGE);
    }
    catch(Botan::Exception e)
    {
        m_botanErrorMsg.reset(new std::string(e.what()));
    }

    return m_page; //return location of newly ciphered data
}

void Codec::decrypt(unsigned int page, unsigned char *data)
{
    assert(data);
    try
    {
        m_decipherFilter->set_key(m_readKey);
        m_decipherFilter->set_iv(getIVForPage(page, false));
        m_decipherPipe.process_msg(data, m_pageSize);
        m_decipherPipe.read(
                    data,
                    m_decipherPipe.remaining(Botan::Pipe::LAST_MESSAGE),
                    Botan::Pipe::LAST_MESSAGE);
    }
    catch(Botan::Exception e)
    {
        m_botanErrorMsg.reset(new std::string(e.what()));
    }
}

Botan::InitializationVector Codec::getIVForPage(uint32_t page, bool useWriteKey)
{
    unsigned char intiv[4];
    Botan::store_le(page, intiv);
    m_mac->clear();
    m_mac->set_key(useWriteKey ? m_ivWriteKey : m_ivReadKey);
    return m_mac->process(intiv, 4);
}

const char* Codec::getError()
{
    if (!m_botanErrorMsg) return nullptr;
    return m_botanErrorMsg->c_str();
}

void Codec::resetError()
{
    m_botanErrorMsg.reset();
}

#include "codec_c_interface.h"

void* InitializeNewCodec(void *db)
{
    return new Codec(db);
}
void* InitializeFromOtherCodec(const void *otherCodec, void *db)
{
    return new Codec(static_cast<const Codec*>(otherCodec), db);
}
void SetWriteKey(void *codec, const char *key, size_t keyLength)
{
    assert(codec);
    static_cast<Codec*>(codec)->setWriteKey(key, keyLength);
}
void GetWriteKey(void *codec, char **key, size_t *keyLength)
{
    assert(codec);
    static_cast<Codec*>(codec)->getWriteKey(const_cast<const char **>(key), keyLength);
}
void DropWriteKey(void *codec)
{
    assert(codec);
    static_cast<Codec*>(codec)->dropWriteKey();
}
void SetWriteIsRead(void *codec)
{
    assert(codec);
    static_cast<Codec*>(codec)->setWriteIsRead();
}
void SetReadIsWrite(void *codec)
{
    assert(codec);
    static_cast<Codec*>(codec)->setReadIsWrite();
}
unsigned char* Encrypt(void *codec, unsigned int page, unsigned char *data, bool useWriteKey)
{
    assert(codec);
    return static_cast<Codec*>(codec)->encrypt(page, data, useWriteKey);
}
void Decrypt(void *codec, unsigned int page, unsigned char *data)
{
    assert(codec);
    static_cast<Codec*>(codec)->decrypt(page, data);
}
void SetPageSize(void *codec, size_t pageSize)
{
    assert(codec);
    static_cast<Codec*>(codec)->setPageSize(pageSize);
}
bool HasReadKey(void *codec)
{
    assert(codec);
    return static_cast<Codec*>(codec)->hasReadKey();
}
bool HasWriteKey(void *codec)
{
    assert(codec);
    return static_cast<Codec*>(codec)->hasWriteKey();
}
void* GetDB(void *codec)
{
    assert(codec);
    return static_cast<Codec*>(codec)->getDB();
}
const char* GetError(void *codec)
{
    assert(codec);
    return static_cast<Codec*>(codec)->getError();
}
void ResetError(void *codec)
{
    assert(codec);
    static_cast<Codec*>(codec)->resetError();
}
void DeleteCodec(void *codec)
{
    assert(codec);
    delete static_cast<Codec*>(codec);
}
