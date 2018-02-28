#include "FindOutputsAsyncWorker.h"
#include <nan.h>

FindOutputsAsyncWorker::FindOutputsAsyncWorker(Crypto::PublicKey txPublicKey,
                                               std::vector<OutputRecord> outputs,
                                               Crypto::SecretKey viewSecretKey,
                                               std::unordered_set<Crypto::PublicKey> spendPublicKeys,
                                               Nan::Callback *callback) : Nan::AsyncWorker(callback) {
    this->txPublicKey = txPublicKey;
    this->outputs = outputs;
    this->viewSecretKey = viewSecretKey;
    this->spendPublicKeys = spendPublicKeys;
}

void FindOutputsAsyncWorker::Execute() {
    KeyDerivation derivation;

    if (!generate_key_derivation(txPublicKey, viewSecretKey, derivation)) {
        SetErrorMessage("Can't generate key derivation");
        return;
    }

    size_t keyIndex = 0;
    for (const OutputRecord output : outputs) {
        Crypto::PublicKey spendKey;
        underive_public_key(derivation, keyIndex, output.key, spendKey);

        if (spendPublicKeys.find(spendKey) != spendPublicKeys.end()) {
            results[spendKey].push_back(output);
        }

        keyIndex++;
    }
}

void FindOutputsAsyncWorker::HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Object> groupedResults = Nan::New<v8::Object>();
    for (std::pair<Crypto::PublicKey, std::vector<OutputRecord>> result : results) {

        v8::Local<v8::Array> outputItems = Nan::New<v8::Array>();

        size_t count = 0;
        for (const OutputRecord output : result.second) {
            v8::Local<v8::Object> outputItem = Nan::New<v8::Object>();
            Nan::Set(outputItem, Nan::New("key").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&output.key), sizeof(Crypto::PublicKey))).ToLocalChecked());
            Nan::Set(outputItem, Nan::New("globalIndex").ToLocalChecked(), Nan::New<v8::Number>(output.globalIndex));
            Nan::Set(outputItem, Nan::New("amount").ToLocalChecked(), Nan::New<v8::Number>(output.amount));
            Nan::Set(outputItems, count, outputItem);
            count++;
        }

        Nan::Set(groupedResults, Nan::New(toHex(reinterpret_cast<const char *>(&result.first), sizeof(Crypto::PublicKey))).ToLocalChecked(), outputItems);
    }

    v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        groupedResults};
    callback->Call(2, argv);
}

void FindOutputsAsyncWorker::HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::New(ErrorMessage()).ToLocalChecked(),
        Nan::Null()};
    callback->Call(2, argv);
}