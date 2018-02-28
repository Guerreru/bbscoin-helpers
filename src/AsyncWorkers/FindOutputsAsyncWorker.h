#pragma once

#include <nan.h>
#include <unordered_map>
#include <unordered_set>
#include "crypto/crypto.h"
#include "CryptoNote.h"
#include "Common/StringTools.h"


using namespace Common;
using namespace Crypto;

struct OutputRecord {
    uint64_t amount;
    uint32_t globalIndex;
    Crypto::PublicKey key;
};

class FindOutputsAsyncWorker : public Nan::AsyncWorker {

  public:
    FindOutputsAsyncWorker(Crypto::PublicKey txPublicKey,
                             std::vector<OutputRecord> outputs,
                             Crypto::SecretKey viewSecretKey,
                             std::unordered_set<Crypto::PublicKey> spendPublicKeys,
                             Nan::Callback *callback);
    void Execute();
    void HandleOKCallback();
    void HandleErrorCallback();

  private:
    Crypto::PublicKey txPublicKey;
    std::vector<OutputRecord> outputs;
    Crypto::SecretKey viewSecretKey;
    std::unordered_set<Crypto::PublicKey> spendPublicKeys;
    std::unordered_map<Crypto::PublicKey, std::vector<OutputRecord>> results;
};