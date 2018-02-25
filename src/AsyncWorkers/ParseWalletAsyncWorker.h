#pragma once

#include <nan.h>

#include "Common.h"
#include "Common/MemoryInputStream.h"
#include "Common/StringTools.h"
#include "CryptoNote.h"
#include "CryptoNoteConfig.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "Wallet/WalletUtils.h"
#include "Wallet/WalletSerializationV2.h"
#include "Wallet/WalletSerializationV2.h"

using namespace Common;
using namespace Crypto;
using namespace CryptoNote;

class ParseWalletAsyncWorker : public Nan::AsyncWorker {

  public:
    ParseWalletAsyncWorker(std::string path, std::string password, Nan::Callback *callback);
    void Execute();
    void HandleOKCallback();
    void HandleErrorCallback();

  private:
    std::string path;
    std::string password;

    uint64_t timestamp;
    std::string address;
    KeyPair spendKey;
    KeyPair viewKey;

    void decryptKeyPair(const EncryptedWalletRecord &cipher,
                        PublicKey &publicKey,
                        SecretKey &secretKey,
                        uint64_t &creationTimestamp,
                        const Crypto::chacha8_key &key);
};