#include <nan.h>
#include "FilterOutputsAsyncWorker.h"

FilterOutputsAsyncWorker::FilterOutputsAsyncWorker(std::vector<OutputRecord> outputs,
                                                   Crypto::SecretKey viewSecretKey,
                                                   std::vector<Crypto::PublicKey> spendPublicKeys,
                                                   Nan::Callback *callback) : Nan::AsyncWorker(callback) {

    this->outputs = outputs;
    this->viewSecretKey = viewSecretKey;
    this->spendPublicKeys = spendPublicKeys;
}

void FilterOutputsAsyncWorker::Execute() {
}

void FilterOutputsAsyncWorker::HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        Nan::Null()
    };
    callback->Call(2, argv);
}

void FilterOutputsAsyncWorker::HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::New(ErrorMessage()).ToLocalChecked(),
        Nan::Null()
    };
    callback->Call(2, argv);
}