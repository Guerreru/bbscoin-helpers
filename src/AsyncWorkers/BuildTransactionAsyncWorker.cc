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
    tx = createTransaction();

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

    if (!tx->validateInputs()) {
        SetErrorMessage("Invalid inputs");
        return;
    }

    if (!tx->validateOutputs()) {
        SetErrorMessage("Invalid outputs");
        return;
    }

    if (!tx->validateSignatures()) {
        SetErrorMessage("Invalid signatures");
        return;
    }
}

void BuildTransactionAsyncWorker::HandleOKCallback() {
    Nan::HandleScope scope;

    Crypto::Hash hash = tx->getTransactionHash();

    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("hash").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&hash), sizeof(Crypto::Hash))).ToLocalChecked());
    Nan::Set(result, Nan::New("data").ToLocalChecked(), Nan::New(toHex(tx->getTransactionData())).ToLocalChecked());

    v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        result};
    callback->Call(2, argv);
}

void BuildTransactionAsyncWorker::HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::New(ErrorMessage()).ToLocalChecked(),
        Nan::Null()};
    callback->Call(2, argv);
}