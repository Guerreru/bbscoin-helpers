#include "WalletBinding.h"
#include "AsyncWorkers/FindOutputsAsyncWorker.h"
#include "AsyncWorkers/GenerateWalletAsyncWorker.h"
#include "AsyncWorkers/ParseWalletAsyncWorker.h"

NAN_MODULE_INIT(WalletBinding::Init) {
    Nan::SetMethod(target, "createWallet", CreateWallet);
    Nan::SetMethod(target, "parseWallet", ParseWallet);
    Nan::SetMethod(target, "secretKeyToPublicKey", SecretKeyToPublicKey);
    Nan::SetMethod(target, "generateNewKeyPair", GenerateNewKeyPair);
    Nan::SetMethod(target, "generateAddressFromKeyPair", GenerateAddressFromKeyPair);
    Nan::SetMethod(target, "getKeyPairFromAddress", GetKeyPairFromAddress);
    Nan::SetMethod(target, "findOutputs", FindOutputs);
}

NAN_METHOD(WalletBinding::CreateWallet) {
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: string path").ToLocalChecked());
    }
    if (!info[1]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 1: string container password").ToLocalChecked());
    }
    if (!info[2]->IsFunction()) {
        return Nan::ThrowError(Nan::New("expected arg 2: function callback").ToLocalChecked());
    }

    Nan::AsyncQueueWorker(new GenerateWalletAsyncWorker(
        std::string(*Nan::Utf8String(info[0]->ToString())),
        std::string(*Nan::Utf8String(info[1]->ToString())),
        new Nan::Callback(info[2].As<v8::Function>())));

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(WalletBinding::ParseWallet) {
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: string path").ToLocalChecked());
    }
    if (!info[1]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 1: string container password").ToLocalChecked());
    }
    if (!info[2]->IsFunction()) {
        return Nan::ThrowError(Nan::New("expected arg 2: function callback").ToLocalChecked());
    }

    Nan::AsyncQueueWorker(new ParseWalletAsyncWorker(
        std::string(*Nan::Utf8String(info[0]->ToString())),
        std::string(*Nan::Utf8String(info[1]->ToString())),
        new Nan::Callback(info[2].As<v8::Function>())));

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(WalletBinding::SecretKeyToPublicKey) {
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: secret key").ToLocalChecked());
    }

    try {
        Crypto::SecretKey secretKey;
        std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(info[0]->ToString())));
        secretKey = *reinterpret_cast<Crypto::SecretKey *>(key.data());

        Crypto::PublicKey publicKey;
        if (!secret_key_to_public_key(secretKey, publicKey)) {
            info.GetReturnValue().Set(Nan::Null());
            return;
        }

        info.GetReturnValue().Set(Nan::New(toHex(reinterpret_cast<const char *>(&publicKey), sizeof(Crypto::PublicKey))).ToLocalChecked());
    } catch (...) {
        info.GetReturnValue().Set(Nan::Null());
        return;
    }
}

NAN_METHOD(WalletBinding::FindOutputs) {
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: transaction public key").ToLocalChecked());
    }
    if (!info[1]->IsArray()) {
        return Nan::ThrowError(Nan::New("expected arg 1: Array<OutputRecord> outputs").ToLocalChecked());
    }
    if (!info[2]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 2: string viewSecretKey").ToLocalChecked());
    }
    if (!info[3]->IsArray()) {
        return Nan::ThrowError(Nan::New("expected arg 3: Array<string> spendPublicKeys").ToLocalChecked());
    }
    if (!info[4]->IsFunction()) {
        return Nan::ThrowError(Nan::New("expected arg 4: function callback").ToLocalChecked());
    }

    // tx public key
    Crypto::PublicKey txPublicKey;
    std::vector<uint8_t> txKey = fromHex(std::string(*Nan::Utf8String(info[0]->ToString())));
    if ((txKey.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
        return Nan::ThrowError(Nan::New("invalid arg 0: transaction public key has invalid length").ToLocalChecked());
    }
    txPublicKey = *reinterpret_cast<Crypto::PublicKey *>(txKey.data());

    // outputs
    std::vector<OutputRecord> outputs;
    v8::Local<v8::Object> outputsJs = info[1]->ToObject();
    const v8::Local<v8::String> lengthString = Nan::New("length").ToLocalChecked();
    const v8::Local<v8::String> amountString = Nan::New("amount").ToLocalChecked();
    const v8::Local<v8::String> globalIndexString = Nan::New("globalIndex").ToLocalChecked();
    const v8::Local<v8::String> keyString = Nan::New("key").ToLocalChecked();
    size_t outputLength = Nan::Get(outputsJs, lengthString).ToLocalChecked()->Uint32Value();
    for (size_t i = 0; i < outputLength; i++) {
        v8::Local<v8::Object> item = Nan::Get(outputsJs, i).ToLocalChecked()->ToObject();
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
            return Nan::ThrowError(Nan::New("invalid arg: amount of output " + std::to_string(i) + " is not a number").ToLocalChecked());
        }
        outputRecord.amount = numberValue->NumberValue();

        // output.globalIndex
        v8::Local<v8::Value> globalIndexValue = Nan::Get(item, globalIndexString).ToLocalChecked();
        if (!globalIndexValue->IsNumber()) {
            return Nan::ThrowError(Nan::New("invalid arg: globalIndex of output " + std::to_string(i) + " is not a number").ToLocalChecked());
        }
        outputRecord.globalIndex = globalIndexValue->NumberValue();

        // output.key
        v8::Local<v8::Value> keyValue = Nan::Get(item, keyString).ToLocalChecked();
        if (!keyValue->IsString()) {
            return Nan::ThrowError(Nan::New("invalid arg: key of output " + std::to_string(i) + " is not a string").ToLocalChecked());
        }
        std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(keyValue->ToString())));
        if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
            return Nan::ThrowError(Nan::New("invalid arg 1: key of output " + std::to_string(i) + " has invalid length").ToLocalChecked());
        }
        outputRecord.key = *reinterpret_cast<Crypto::PublicKey *>(key.data());

        outputs.push_back(std::move(outputRecord));
    }

    // view secret key
    Crypto::SecretKey viewSecretKey;
    std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(info[2]->ToString())));
    if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::SecretKey)) {
        return Nan::ThrowError(Nan::New("invalid arg 2: view secret key has invalid length").ToLocalChecked());
    }
    viewSecretKey = *reinterpret_cast<Crypto::SecretKey *>(key.data());

    // spend public keys
    std::unordered_set<Crypto::PublicKey> spendPublicKeys;
    v8::Local<v8::Object> spendPublicKeysJs = info[3]->ToObject();
    size_t keyLength = Nan::Get(spendPublicKeysJs, lengthString).ToLocalChecked()->Uint32Value();
    for (size_t i = 0; i < keyLength; i++) {
        // key
        v8::Local<v8::Value> keyValue = Nan::Get(spendPublicKeysJs, i).ToLocalChecked();
        if (!keyValue->IsString()) {
            return Nan::ThrowError(Nan::New("invalid arg 3: spend public key " + std::to_string(i) + " is not a string").ToLocalChecked());
        }
        std::vector<uint8_t> key = fromHex(std::string(*Nan::Utf8String(keyValue->ToString())));
        if ((key.size() * sizeof(uint8_t)) != sizeof(Crypto::PublicKey)) {
            return Nan::ThrowError(Nan::New("invalid arg 3: spend public key " + std::to_string(i) + " has invalid length").ToLocalChecked());
        }
        spendPublicKeys.insert(*reinterpret_cast<Crypto::PublicKey *>(key.data()));
    }

    Nan::AsyncQueueWorker(new FindOutputsAsyncWorker(
        txPublicKey,
        outputs,
        viewSecretKey,
        spendPublicKeys,
        new Nan::Callback(info[4].As<v8::Function>())));

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(WalletBinding::GenerateNewKeyPair) {
    KeyPair key;
    Crypto::generate_keys(key.publicKey, key.secretKey);
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("public").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&key.publicKey), sizeof(Crypto::PublicKey))).ToLocalChecked());
    Nan::Set(result, Nan::New("secret").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&key.secretKey), sizeof(Crypto::SecretKey))).ToLocalChecked());

    info.GetReturnValue().Set(result);
}

NAN_METHOD(WalletBinding::GenerateAddressFromKeyPair) {
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: string spend public key").ToLocalChecked());
    }
    if (!info[1]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 1: string view public key").ToLocalChecked());
    }

    try {
        std::string spendKey = std::string(*Nan::Utf8String(info[0]->ToString()));
        std::string viewKey = std::string(*Nan::Utf8String(info[1]->ToString()));
        Crypto::PublicKey spendPublicKey;
        Crypto::PublicKey viewPublicKey;
        fromHex(spendKey, &spendPublicKey, spendKey.size());
        fromHex(viewKey, &viewPublicKey, viewKey.size());

        std::string address = getAccountAddressAsStr(parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, {spendPublicKey, viewPublicKey});

        info.GetReturnValue().Set(Nan::New(address).ToLocalChecked());
    } catch (...) {
        info.GetReturnValue().Set(Nan::Null());
        return;
    }
}

NAN_METHOD(WalletBinding::GetKeyPairFromAddress) {
    if (!info[0]->IsString()) {
        return Nan::ThrowError(Nan::New("expected arg 0: wallet address").ToLocalChecked());
    }

    try {
        std::string address = std::string(*Nan::Utf8String(info[0]->ToString()));
        CryptoNote::AccountPublicAddress keys;
        uint64_t prefix = parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX;
        if (!parseAccountAddressString(prefix, keys, address)) {
            info.GetReturnValue().Set(Nan::Null());
            return;
        }

        v8::Local<v8::Object> result = Nan::New<v8::Object>();
        Nan::Set(result, Nan::New("view").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&keys.viewPublicKey), sizeof(Crypto::PublicKey))).ToLocalChecked());
        Nan::Set(result, Nan::New("spend").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&keys.spendPublicKey), sizeof(Crypto::PublicKey))).ToLocalChecked());
        info.GetReturnValue().Set(result);
    } catch (...) {
        info.GetReturnValue().Set(Nan::Null());
        return;
    }
}
