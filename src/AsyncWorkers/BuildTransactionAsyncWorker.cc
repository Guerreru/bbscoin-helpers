#include "BuildTransactionAsyncWorker.h"
#include <nan.h>

BuildTransactionAsyncWorker::BuildTransactionAsyncWorker(uint64_t unlockTime,
                                                         CryptoNote::AccountKeys senderKeys,
                                                         std::string extra,
                                                         std::vector<InputInfo> sources,
                                                         std::vector<OutputInfo> destinations,
                                                         Nan::Callback *callback) : Nan::AsyncWorker(callback) {
    this->unlockTime = unlockTime;
    this->senderKeys = senderKeys;
    this->extra = extra;
    this->sources = sources;
    this->destinations = destinations;
    this->callback = callback;
}

void BuildTransactionAsyncWorker::Execute() {
    std::unique_ptr<ITransaction> tx = createTransaction();

    for (const auto &destination : destinations) {
        tx->addOutput(destination.amount, destination.address);
    }

    tx->setUnlockTime(unlockTime);
    tx->appendExtra(Common::asBinaryArray(extra));

    for (auto &source : sources) {
        tx->addInput(senderKeys, source.keyInfo, source.ephKeys);
    }

    size_t i = 0;
    for (auto &source : sources) {
        tx->signInputKey(i++, source.keyInfo, source.ephKeys);
    }
}

void BuildTransactionAsyncWorker::HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        Nan::Null()};
    callback->Call(2, argv);
}

void BuildTransactionAsyncWorker::HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::New(ErrorMessage()).ToLocalChecked(),
        Nan::Null()};
    callback->Call(2, argv);
}