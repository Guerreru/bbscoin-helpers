#pragma once

#include <nan.h>
#include "CryptoNote.h"

struct OutputRecord {
    uint64_t amount;
    Crypto::PublicKey key;
};

class FilterOutputsAsyncWorker : public Nan::AsyncWorker {

  public:
    FilterOutputsAsyncWorker(std::vector<OutputRecord> outputs,
                             Crypto::SecretKey viewSecretKey,
                             std::vector<Crypto::PublicKey> spendPublicKeys,
                             Nan::Callback *callback);
    void Execute();
    void HandleOKCallback();
    void HandleErrorCallback();

  private:
    std::vector<OutputRecord> outputs;
    Crypto::SecretKey viewSecretKey;
    std::vector<Crypto::PublicKey> spendPublicKeys;
    std::vector<OutputRecord> filteredOutputs;

};