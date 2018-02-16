#include "WalletBinding.h"
#include "AsyncWorkers/GenerateWalletAsyncWorker.h"
#include "AsyncWorkers/FilterOutputsAsyncWorker.h"

NAN_MODULE_INIT(WalletBinding::Init) {
    Nan::SetMethod(target, "createWallet", CreateWallet);
    Nan::SetMethod(target, "generateNewKeyPair", GenerateNewKeyPair);
    Nan::SetMethod(target, "generateAddressFromKeyPair", GenerateAddressFromKeyPair);
    Nan::SetMethod(target, "filterOutputs", FilterOutputs);
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

NAN_METHOD(WalletBinding::FilterOutputs) {
    if(!info[0]->IsArray()) {
        return Nan::ThrowError(Nan::New("expected arg 0: Array<OutputRecord> outputs").ToLocalChecked());
    }
    if(!info[1]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 1: string viewSecretKey").ToLocalChecked());
    }
    if(!info[2]->IsArray()) {
        return Nan::ThrowError(Nan::New("expected arg 2: Array<string> spendPublicKeys").ToLocalChecked());
    }
    if(!info[3]->IsFunction()) {
        return Nan::ThrowError(Nan::New("expected arg 3: function callback").ToLocalChecked());
    }

    // outputs
    std::vector<OutputRecord> outputs;
    v8::Local<v8::Object> outputs_js = info[0]->ToObject();
    const v8::Local<v8::String> lengthString = Nan::New("length").ToLocalChecked();
    const v8::Local<v8::String> amountString = Nan::New("amount").ToLocalChecked();
    const v8::Local<v8::String> keyString = Nan::New("key").ToLocalChecked();
    uint32_t outputLength = Nan::Get(outputs_js, lengthString).ToLocalChecked()->Uint32Value();
    for (uint32_t i = 0; i < outputLength; i++) {
        v8::Local<v8::Object> item = Nan::Get(outputs_js, i).ToLocalChecked()->ToObject();
        if (!Nan::HasOwnProperty(item, amountString).FromJust()) {
            return Nan::ThrowError(Nan::New("invalid arg 0: output " + std::to_string(i) + " missing amount").ToLocalChecked());
        }

        if (!Nan::HasOwnProperty(item, keyString).FromJust()) {
            return Nan::ThrowError(Nan::New("invalid arg 0: output " + std::to_string(i) + " missing key").ToLocalChecked());
        }

        OutputRecord outputRecord;

        // output.amount
        v8::Local<v8::Value> numberValue = Nan::Get(item, amountString).ToLocalChecked();
        if (!numberValue->IsNumber()) {
            return Nan::ThrowError(Nan::New("invalid arg 0: amount of output " + std::to_string(i) + " is not a number").ToLocalChecked());
        }
        outputRecord.amount = numberValue->NumberValue();

        // output.key
        v8::Local<v8::Value> keyValue = Nan::Get(item, keyString).ToLocalChecked();
        if (!keyValue->IsString()) {
            return Nan::ThrowError(Nan::New("invalid arg 0: key of output " + std::to_string(i) + " is not a string").ToLocalChecked());
        }
        std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(keyValue->ToString())));
        if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
            return Nan::ThrowError(Nan::New("invalid arg 0: key of output " + std::to_string(i) + " has invalid length").ToLocalChecked());
        }
        outputRecord.key = *reinterpret_cast<Crypto::PublicKey*>(key.data());

        outputs.push_back(outputRecord);
    }

    // view secret key
    Crypto::SecretKey viewSecretKey;
    std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(info[1]->ToString())));
    if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::SecretKey)) {
        return Nan::ThrowError(Nan::New("invalid arg 1: view secret key has invalid length").ToLocalChecked());
    }
    viewSecretKey = *reinterpret_cast<Crypto::SecretKey*>(key.data());

    // spend public keys
    std::vector<Crypto::PublicKey> spendPublicKeys;
    v8::Local<v8::Object> spendPublicKeys_js = info[2]->ToObject();
    uint32_t keyLength = Nan::Get(spendPublicKeys_js, lengthString).ToLocalChecked()->Uint32Value();
    for (uint32_t i = 0; i < keyLength; i++) {
        // key
        v8::Local<v8::Value> keyValue = Nan::Get(spendPublicKeys_js, i).ToLocalChecked();
        if (!keyValue->IsString()) {
            return Nan::ThrowError(Nan::New("invalid arg 2: spend public key " + std::to_string(i) + " is not a string").ToLocalChecked());
        }
        std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(keyValue->ToString())));
        if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
            return Nan::ThrowError(Nan::New("invalid arg 2: spend public key " + std::to_string(i) + " has invalid length").ToLocalChecked());
        }
        spendPublicKeys.push_back(*reinterpret_cast<Crypto::PublicKey*>(key.data()));
    }

	Nan::AsyncQueueWorker(new FilterOutputsAsyncWorker(
        outputs,
        viewSecretKey,
        spendPublicKeys,
        new Nan::Callback(info[3].As<v8::Function>())
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
