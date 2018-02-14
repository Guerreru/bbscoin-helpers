#include "WalletBinding.h"
#include "AsyncWorkers/GenerateWalletAsyncWorker.h"

NAN_MODULE_INIT(WalletBinding::Init) {
    Nan::SetMethod(target, "createWallet", CreateWallet);
    Nan::SetMethod(target, "generateNewKeyPair", GenerateNewKeyPair);
    Nan::SetMethod(target, "generateAddressFromKeyPair", GenerateAddressFromKeyPair);
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

NAN_METHOD(WalletBinding::GenerateNewKeyPair) {
    KeyPair key;
    Crypto::generate_keys(key.publicKey, key.secretKey);
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("public").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char*>(&key.publicKey), sizeof(Crypto::PublicKey))).ToLocalChecked());
    Nan::Set(result, Nan::New("secret").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char*>(&key.secretKey), sizeof(Crypto::SecretKey))).ToLocalChecked());

    info.GetReturnValue().Set(result);
}

NAN_METHOD(WalletBinding::GenerateAddressFromKeyPair) {
    if(!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: string spend public key").ToLocalChecked());
    }
    if(!info[1]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 1: string view public key").ToLocalChecked());
    }

    std::string spendKey = std::string(*Nan::Utf8String(info[0]->ToString()));
    std::string viewKey = std::string(*Nan::Utf8String(info[1]->ToString()));
    Crypto::PublicKey spendPublicKey;
    Crypto::PublicKey viewPublicKey;
    fromHex(spendKey, &spendPublicKey, spendKey.size());
    fromHex(viewKey, &viewPublicKey, viewKey.size());

    std::string address = getAccountAddressAsStr(parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, {spendPublicKey, viewPublicKey});

    info.GetReturnValue().Set(Nan::New(address).ToLocalChecked());
}
