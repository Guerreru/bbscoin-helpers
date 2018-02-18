#include "WalletBinding.h"
#include "AsyncWorkers/GenerateWalletAsyncWorker.h"
#include "AsyncWorkers/FindOutputsAsyncWorker.h"

NAN_MODULE_INIT(WalletBinding::Init) {
    Nan::SetMethod(target, "createWallet", CreateWallet);
    Nan::SetMethod(target, "generateNewKeyPair", GenerateNewKeyPair);
    Nan::SetMethod(target, "generateAddressFromKeyPair", GenerateAddressFromKeyPair);
    Nan::SetMethod(target, "findOutputs", FindOutputs);
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

NAN_METHOD(WalletBinding::FindOutputs) {
    if(!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: transaction public key").ToLocalChecked());
    }
    if(!info[1]->IsArray()) {
        return Nan::ThrowError(Nan::New("expected arg 1: Array<OutputRecord> outputs").ToLocalChecked());
    }
    if(!info[2]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 2: string viewSecretKey").ToLocalChecked());
    }
    if(!info[3]->IsArray()) {
        return Nan::ThrowError(Nan::New("expected arg 3: Array<string> spendPublicKeys").ToLocalChecked());
    }
    if(!info[4]->IsFunction()) {
        return Nan::ThrowError(Nan::New("expected arg 4: function callback").ToLocalChecked());
    }

    // tx public key
    Crypto::PublicKey txPublicKey;
    std::vector<uint8_t> txKey = fromHex(std::string(*Nan::Utf8String(info[0]->ToString())));
    if ((txKey.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
        return Nan::ThrowError(Nan::New("invalid arg 0: transaction public key has invalid length").ToLocalChecked());
    }
    txPublicKey = *reinterpret_cast<Crypto::PublicKey*>(txKey.data());

    // outputs
    std::vector<OutputRecord> outputs;
    v8::Local<v8::Object> outputs_js = info[1]->ToObject();
    const v8::Local<v8::String> lengthString = Nan::New("length").ToLocalChecked();
    const v8::Local<v8::String> amountString = Nan::New("amount").ToLocalChecked();
    const v8::Local<v8::String> keyString = Nan::New("key").ToLocalChecked();
    size_t outputLength = Nan::Get(outputs_js, lengthString).ToLocalChecked()->Uint32Value();
    for (size_t i = 0; i < outputLength; i++) {
        v8::Local<v8::Object> item = Nan::Get(outputs_js, i).ToLocalChecked()->ToObject();
        if (!Nan::HasOwnProperty(item, amountString).FromJust()) {
            return Nan::ThrowError(Nan::New("invalid arg 1: output " + std::to_string(i) + " missing amount").ToLocalChecked());
        }

        if (!Nan::HasOwnProperty(item, keyString).FromJust()) {
            return Nan::ThrowError(Nan::New("invalid arg 1: output " + std::to_string(i) + " missing key").ToLocalChecked());
        }

        OutputRecord outputRecord;

        // output.amount
        v8::Local<v8::Value> numberValue = Nan::Get(item, amountString).ToLocalChecked();
        if (!numberValue->IsNumber()) {
            return Nan::ThrowError(Nan::New("invalid arg 1: amount of output " + std::to_string(i) + " is not a number").ToLocalChecked());
        }
        outputRecord.amount = numberValue->NumberValue();

        // output.key
        v8::Local<v8::Value> keyValue = Nan::Get(item, keyString).ToLocalChecked();
        if (!keyValue->IsString()) {
            return Nan::ThrowError(Nan::New("invalid arg 1: key of output " + std::to_string(i) + " is not a string").ToLocalChecked());
        }
        std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(keyValue->ToString())));
        if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
            return Nan::ThrowError(Nan::New("invalid arg 1: key of output " + std::to_string(i) + " has invalid length").ToLocalChecked());
        }
        outputRecord.key = *reinterpret_cast<Crypto::PublicKey*>(key.data());

        outputs.push_back(outputRecord);
    }

    // view secret key
    Crypto::SecretKey viewSecretKey;
    std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(info[2]->ToString())));
    if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::SecretKey)) {
        return Nan::ThrowError(Nan::New("invalid arg 2: view secret key has invalid length").ToLocalChecked());
    }
    viewSecretKey = *reinterpret_cast<Crypto::SecretKey*>(key.data());

    // spend public keys
    std::unordered_set<Crypto::PublicKey> spendPublicKeys;
    v8::Local<v8::Object> spendPublicKeys_js = info[3]->ToObject();
    size_t keyLength = Nan::Get(spendPublicKeys_js, lengthString).ToLocalChecked()->Uint32Value();
    for (size_t i = 0; i < keyLength; i++) {
        // key
        v8::Local<v8::Value> keyValue = Nan::Get(spendPublicKeys_js, i).ToLocalChecked();
        if (!keyValue->IsString()) {
            return Nan::ThrowError(Nan::New("invalid arg 3: spend public key " + std::to_string(i) + " is not a string").ToLocalChecked());
        }
        std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(keyValue->ToString())));
        if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
            return Nan::ThrowError(Nan::New("invalid arg 3: spend public key " + std::to_string(i) + " has invalid length").ToLocalChecked());
        }
        spendPublicKeys.insert(*reinterpret_cast<Crypto::PublicKey*>(key.data()));
    }

	Nan::AsyncQueueWorker(new FindOutputsAsyncWorker(
        txPublicKey,
        outputs,
        viewSecretKey,
        spendPublicKeys,
        new Nan::Callback(info[4].As<v8::Function>())
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
