#pragma once

#include <nan.h>

#include "Common/StringTools.h"
#include "Common/VectorOutputStream.h"
#include "CryptoNote.h"
#include "CryptoNoteCore/TransactionApi.h"
#include "ITransaction.h"
#include "Serialization/BinaryOutputStreamSerializer.h"

using namespace Common;
using namespace Crypto;
using namespace CryptoNote;
using namespace TransactionTypes;

struct InputInfo {
    InputKeyInfo keyInfo;
    KeyPair ephKeys;
};

struct OutputInfo {
    AccountPublicAddress address;
    uint64_t amount;
};

class BuildTransactionAsyncWorker : public Nan::AsyncWorker {

  public:
    BuildTransactionAsyncWorker(uint64_t unlockTime,
                                CryptoNote::AccountKeys senderKeys,
                                std::string extra,
                                std::vector<InputInfo> sources,
                                std::vector<OutputInfo> destinations,
                                Nan::Callback *callback);
    void Execute();
    void HandleOKCallback();
    void HandleErrorCallback();

  private:
    uint64_t unlockTime;
    CryptoNote::AccountKeys senderKeys;
    std::string extra;
    std::vector<InputInfo> sources;
    std::vector<OutputInfo> destinations;
    Nan::Callback *callback;
    BinaryArray binaryArray;
};