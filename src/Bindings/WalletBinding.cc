#include "WalletBinding.h"
#include "AsyncWorkers/GenerateWalletAsyncWorker.h"

NAN_MODULE_INIT(WalletBinding::Init) {
    Nan::SetMethod(target, "createWallet", CreateWallet);
}

NAN_METHOD(WalletBinding::CreateWallet) {
    if(!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: string path").ToLocalChecked());
    }
    if(!info[1]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 1: string container password").ToLocalChecked());
    }
    if(!info[2]->IsFunction()) {
        return Nan::ThrowError(Nan::New("expected arg 2: function callback").ToLocalChecked());
    }

	Nan::AsyncQueueWorker(new GenerateWalletAsyncWorker(
        std::string(*Nan::Utf8String(info[0]->ToString())),
        std::string(*Nan::Utf8String(info[1]->ToString())),
        new Nan::Callback(info[2].As<v8::Function>())
	));

    info.GetReturnValue().Set(Nan::Undefined());
}