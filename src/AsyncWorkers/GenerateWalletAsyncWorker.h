#pragma once

#include <nan.h>
#include <initializer_list>

#include "CryptoNoteConfig.h"
#include "Common/StringOutputStream.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "Serialization/BinaryOutputStreamSerializer.h"
#include "Wallet/WalletGreen.h"
#include "Wallet/WalletSerializationV2.h"

using namespace Common;
using namespace Crypto;
using namespace CryptoNote;

class GenerateWalletAsyncWorker : public Nan::AsyncWorker {

  public:
    GenerateWalletAsyncWorker(std::string, std::string, Nan::Callback *);
    void Execute();
    void HandleOKCallback();
    void HandleErrorCallback();

  private:
    std::string path;
    std::string password;
    std::string address;

    struct ContainerStoragePrefix {
        uint8_t version;
        Crypto::chacha8_iv nextIv;
        EncryptedWalletRecord encryptedViewKeys;
    };

    EncryptedWalletRecord encryptKeyPair(const PublicKey &publicKey, const SecretKey &secretKey,
                                         uint64_t creationTimestamp, const Crypto::chacha8_key &key,
                                         const Crypto::chacha8_iv &iv);

    void incIv(Crypto::chacha8_iv &iv);

    void encryptAndSaveContainerData(ContainerStorage &storage,
                                     const Crypto::chacha8_key &key,
                                     const void *containerData,
                                     size_t containerDataSize);
};