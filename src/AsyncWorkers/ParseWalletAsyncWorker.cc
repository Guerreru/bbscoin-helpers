#include "ParseWalletAsyncWorker.h"
#include <nan.h>

ParseWalletAsyncWorker::ParseWalletAsyncWorker(std::string path,
                                               std::string password,
                                               Nan::Callback *callback) : Nan::AsyncWorker(callback) {
    this->path = path;
    this->password = password;
}

void ParseWalletAsyncWorker::Execute() {
    ContainerStorage storage(path, Common::FileMappedVectorOpenMode::OPEN, sizeof(ContainerStoragePrefix));

    try {
        ContainerStoragePrefix *prefix = reinterpret_cast<ContainerStoragePrefix *>(storage.prefix());
        if (prefix->version < WalletSerializerV2::MIN_VERSION) {
            SetErrorMessage("Unsupported wallet file version. Please use GUI wallet to upgrade first.");
            return;
        }

        Crypto::cn_context cnContext;
        Crypto::chacha8_key key;
        generate_chacha8_key(cnContext, password, key);

        decryptKeyPair(prefix->encryptedViewKeys, viewKey.publicKey, viewKey.secretKey, timestamp, key);
        throwIfKeysMismatch(viewKey.secretKey, viewKey.publicKey, "Restored view public key doesn't correspond to secret key");

        WalletRecord wallet;
        decryptKeyPair(storage[0], spendKey.publicKey, spendKey.secretKey, timestamp, key);
        throwIfKeysMismatch(spendKey.secretKey, spendKey.publicKey, "Restored spend public key doesn't correspond to secret key");

        address = getAccountAddressAsStr(parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, {spendKey.publicKey, viewKey.publicKey});

        storage.close();

    } catch (const std::exception &e) {
        storage.close();
        std::string msg = "Failed to parse wallet: ";
        SetErrorMessage((msg + e.what()).c_str());
        return;
    }
}

void ParseWalletAsyncWorker::HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Object> spendKeyResult = Nan::New<v8::Object>();
    Nan::Set(spendKeyResult, Nan::New("public").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&spendKey.publicKey), sizeof(Crypto::PublicKey))).ToLocalChecked());
    Nan::Set(spendKeyResult, Nan::New("secret").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&spendKey.secretKey), sizeof(Crypto::SecretKey))).ToLocalChecked());
    v8::Local<v8::Object> viewKeyResult = Nan::New<v8::Object>();
    Nan::Set(viewKeyResult, Nan::New("public").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&viewKey.publicKey), sizeof(Crypto::PublicKey))).ToLocalChecked());
    Nan::Set(viewKeyResult, Nan::New("secret").ToLocalChecked(), Nan::New(toHex(reinterpret_cast<const char *>(&viewKey.secretKey), sizeof(Crypto::SecretKey))).ToLocalChecked());
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("timestamp").ToLocalChecked(), Nan::New<v8::Number>(timestamp));
    Nan::Set(result, Nan::New("address").ToLocalChecked(), Nan::New(address).ToLocalChecked());
    Nan::Set(result, Nan::New("spendKey").ToLocalChecked(), spendKeyResult);
    Nan::Set(result, Nan::New("viewKey").ToLocalChecked(), viewKeyResult);

    v8::Local<v8::Value> argv[] = {Nan::Null(), result};
    callback->Call(2, argv);
}

void ParseWalletAsyncWorker::HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {Nan::New(ErrorMessage()).ToLocalChecked(), Nan::Null()};
    callback->Call(2, argv);
}

void ParseWalletAsyncWorker::decryptKeyPair(const EncryptedWalletRecord &cipher,
                                            PublicKey &publicKey,
                                            SecretKey &secretKey,
                                            uint64_t &creationTimestamp,
                                            const Crypto::chacha8_key &key) {

    std::array<char, sizeof(cipher.data)> buffer;
    chacha8(cipher.data, sizeof(cipher.data), key, cipher.iv, buffer.data());

    MemoryInputStream stream(buffer.data(), buffer.size());
    BinaryInputStreamSerializer serializer(stream);

    serializer(publicKey, "publicKey");
    serializer(secretKey, "secretKey");
    serializer.binary(&creationTimestamp, sizeof(uint64_t), "creationTimestamp");
}