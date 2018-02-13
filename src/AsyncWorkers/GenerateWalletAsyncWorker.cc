#include "GenerateWalletAsyncWorker.h"
#include <nan.h>

GenerateWalletAsyncWorker::GenerateWalletAsyncWorker(std::string path,
                                                     std::string password, Nan::Callback *callback)
    : Nan::AsyncWorker(callback) {

    this->path = path;
    this->password = password;
}

void GenerateWalletAsyncWorker::Execute() {
    address = "";

    try {
        ContainerStorage storage(path, Common::FileMappedVectorOpenMode::CREATE, sizeof(ContainerStoragePrefix));
        ContainerStoragePrefix *prefix = reinterpret_cast<ContainerStoragePrefix *>(storage.prefix());
        prefix->version = static_cast<uint8_t>(WalletSerializerV2::SERIALIZATION_VERSION);
        prefix->nextIv = Crypto::rand<Crypto::chacha8_iv>();

        // Generate storage key
        Crypto::cn_context cnContext;
        chacha8_key storageKey;
        Crypto::generate_chacha8_key(cnContext, password, storageKey);

        // Generate spend/view key pair
        KeyPair spendKey;
        KeyPair viewKey;
        Crypto::generate_keys(spendKey.publicKey, spendKey.secretKey);
        Crypto::generate_keys(viewKey.publicKey, viewKey.secretKey);
        uint64_t creationTimestamp = time(nullptr);

        // View keys
        prefix->encryptedViewKeys = encryptKeyPair(viewKey.publicKey, viewKey.secretKey, creationTimestamp, storageKey, prefix->nextIv);

        // Spend keys
        incIv(prefix->nextIv);
        storage.push_back(encryptKeyPair(spendKey.publicKey, spendKey.secretKey, creationTimestamp, storageKey, prefix->nextIv));

        // Other stuff
        std::string containerData;
        Common::StringOutputStream containerStream(containerData);
        CryptoNote::BinaryOutputStreamSerializer s(containerStream);
        uint8_t saveLevel = static_cast<uint8_t>(WalletSaveLevel::SAVE_KEYS_ONLY);
        s(saveLevel, "saveLevel");

        encryptAndSaveContainerData(storage, storageKey, containerData.data(), containerData.size());
        storage.flush();

        // Generate wallet address
        address = getAccountAddressAsStr(parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, {spendKey.publicKey, viewKey.publicKey});

    } catch (const std::exception &e) {
        std::string msg = "Failed to save wallet: ";
        this->SetErrorMessage((msg + e.what()).c_str());
        return;
    }
}

void GenerateWalletAsyncWorker::HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        Nan::New(this->address).ToLocalChecked()};
    callback->Call(2, argv);
}

void GenerateWalletAsyncWorker::HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::New(this->ErrorMessage()).ToLocalChecked(),
        Nan::Null()};
    callback->Call(2, argv);
}

EncryptedWalletRecord GenerateWalletAsyncWorker::encryptKeyPair(const PublicKey &publicKey, const SecretKey &secretKey,
                                                                uint64_t creationTimestamp, const Crypto::chacha8_key &key,
                                                                const Crypto::chacha8_iv &iv) {

    EncryptedWalletRecord result;

    std::string serializedKeys;
    StringOutputStream outputStream(serializedKeys);
    BinaryOutputStreamSerializer serializer(outputStream);

    serializer(const_cast<PublicKey &>(publicKey), "publicKey");
    serializer(const_cast<SecretKey &>(secretKey), "secretKey");
    serializer.binary(&creationTimestamp, sizeof(uint64_t), "creationTimestamp");

    assert(serializedKeys.size() == sizeof(result.data));

    result.iv = iv;
    chacha8(serializedKeys.data(), serializedKeys.size(), key, result.iv, reinterpret_cast<char *>(result.data));

    return result;
}

void GenerateWalletAsyncWorker::incIv(Crypto::chacha8_iv &iv) {
    static_assert(sizeof(uint64_t) == sizeof(Crypto::chacha8_iv),
                  "Bad Crypto::chacha8_iv size");
    uint64_t *i = reinterpret_cast<uint64_t *>(&iv);
    if (*i < std::numeric_limits<uint64_t>::max()) {
        ++(*i);
    } else {
        *i = 0;
    }
}

void GenerateWalletAsyncWorker::encryptAndSaveContainerData(ContainerStorage &storage,
                                                            const Crypto::chacha8_key &key,
                                                            const void *containerData,
                                                            size_t containerDataSize) {

    ContainerStoragePrefix *prefix = reinterpret_cast<ContainerStoragePrefix *>(storage.prefix());
    Crypto::chacha8_iv suffixIv = prefix->nextIv;
    incIv(prefix->nextIv);

    BinaryArray encryptedContainer;
    encryptedContainer.resize(containerDataSize);
    chacha8(containerData, containerDataSize, key, suffixIv, reinterpret_cast<char *>(encryptedContainer.data()));

    std::string suffix;
    Common::StringOutputStream suffixStream(suffix);
    BinaryOutputStreamSerializer suffixSerializer(suffixStream);
    suffixSerializer(suffixIv, "suffixIv");
    suffixSerializer(encryptedContainer, "encryptedContainer");

    storage.resizeSuffix(suffix.size());
    std::copy(suffix.begin(), suffix.end(), storage.suffix());
}
