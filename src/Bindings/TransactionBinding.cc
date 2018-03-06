#include "TransactionBinding.h"
#include "AsyncWorkers/BuildTransactionAsyncWorker.h"

NAN_MODULE_INIT(TransactionBinding::Init) {
    Nan::SetMethod(target, "decomposeAmount", DecomposeAmount);
    Nan::SetMethod(target, "buildTransaction", BuildTransaction);
}

NAN_METHOD(TransactionBinding::DecomposeAmount) {
    if (!info[0]->IsNumber()) {
        return Nan::ThrowError(Nan::New("expected arg 0: amount").ToLocalChecked());
    }
    if (!info[1]->IsNumber()) {
        return Nan::ThrowError(Nan::New("expected arg 1: dustThreshold").ToLocalChecked());
    }

    uint64_t amount = info[0]->IntegerValue();
    uint64_t dustThreshold = info[1]->IntegerValue();
    std::vector<uint64_t> decomposedAmounts;
    CryptoNote::decomposeAmount(amount, dustThreshold, decomposedAmounts);

    size_t count = 0;
    v8::Local<v8::Array> result = Nan::New<v8::Array>();
    for (const uint64_t amonut : decomposedAmounts) {
        Nan::Set(result, count, Nan::New<v8::Number>(amonut));
        count++;
    }
    info.GetReturnValue().Set(result);
}

NAN_METHOD(TransactionBinding::BuildTransaction) {
    if (!info[0]->IsObject()) {
        return Nan::ThrowError(Nan::New("expected arg 0: transaction object").ToLocalChecked());
    }

    v8::Local<v8::Object> transactionJs = info[0]->ToObject();

    // {
    //     unlockTime : '',
    //     extra: '',
    //     sender : {
    //         address : '',
    //         viewSecret : '',
    //         spendSecret : '',
    //     }
    //     sources : [
    //         {
    //             amount : 0,
    //             realOutput : {
    //                 transactionKey : '',
    //                 transactionIndex : 0, // Real output index in the outpus array below
    //                 outputInTransaction : 0, // Output index in the original transaction
    //             },
    //             outputs : [
    //                 {
    //                     key : '',
    //                     globalIndex : 0
    //                 }
    //             ]
    //         }
    //     ],
    //     destinations : [
    //         {
    //             address : '',
    //             amount : 0
    //         }
    //     ]
    // }

    // unlockTime
    uint64_t unlockTime = Nan::Get(transactionJs, Nan::New("unlockTime").ToLocalChecked()).ToLocalChecked()->NumberValue();

    // extra
    v8::Local<v8::Value> extraValue = Nan::Get(transactionJs, Nan::New("extra").ToLocalChecked()).ToLocalChecked();
    std::string extra = std::string(*Nan::Utf8String(extraValue->ToString()));

    // sender
    CryptoNote::AccountKeys senderKeys;
    v8::Local<v8::Object> sender = Nan::Get(transactionJs, Nan::New("sender").ToLocalChecked()).ToLocalChecked()->ToObject();

    // sender.address
    v8::Local<v8::Value> addressValue = Nan::Get(sender, Nan::New("address").ToLocalChecked()).ToLocalChecked();
    std::string address = std::string(*Nan::Utf8String(addressValue->ToString()));
    uint64_t prefix = parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX;
    if (!parseAccountAddressString(prefix, senderKeys.address, address)) {
        return Nan::ThrowError(Nan::New("invalid sender address").ToLocalChecked());
    }

    // sender.viewSecret
    v8::Local<v8::Value> viewSecretValue = Nan::Get(sender, Nan::New("viewSecret").ToLocalChecked()).ToLocalChecked();
    std::vector<uint8_t> viewSecret = fromHex(std::string(*Nan::Utf8String(viewSecretValue->ToString())));
    senderKeys.viewSecretKey = *reinterpret_cast<Crypto::SecretKey *>(viewSecret.data());

    // sender.spendSecret
    v8::Local<v8::Value> spendSecretValue = Nan::Get(sender, Nan::New("spendSecret").ToLocalChecked()).ToLocalChecked();
    std::vector<uint8_t> spendSecret = fromHex(std::string(*Nan::Utf8String(spendSecretValue->ToString())));
    senderKeys.spendSecretKey = *reinterpret_cast<Crypto::SecretKey *>(spendSecret.data());

    // sources
    std::vector<InputInfo> sources;
    v8::Local<v8::Object> sourcesJs = Nan::Get(transactionJs, Nan::New("sources").ToLocalChecked()).ToLocalChecked()->ToObject();
    size_t sourcesJsLength = Nan::Get(sourcesJs, Nan::New("length").ToLocalChecked()).ToLocalChecked()->Uint32Value();
    for (size_t i = 0; i < sourcesJsLength; i++) {
        v8::Local<v8::Object> sourceJs = Nan::Get(sourcesJs, i).ToLocalChecked()->ToObject();

        InputKeyInfo keyInfo;

        // sources[].amount
        keyInfo.amount = Nan::Get(sourceJs, Nan::New("amount").ToLocalChecked()).ToLocalChecked()->NumberValue();

        // sources[].realOutput
        OutputKeyInfo realOutput;
        keyInfo.realOutput = std::move(realOutput);
        v8::Local<v8::Object> realOutputValue = Nan::Get(sourceJs, Nan::New("realOutput").ToLocalChecked()).ToLocalChecked()->ToObject();

        // sources[].realOutput.transactionKey
        v8::Local<v8::Value> transactionKeyValue = Nan::Get(realOutputValue, Nan::New("transactionKey").ToLocalChecked()).ToLocalChecked();
        std::vector<uint8_t> transactionKey = fromHex(std::string(*Nan::Utf8String(transactionKeyValue->ToString())));
        keyInfo.realOutput.transactionPublicKey = *reinterpret_cast<Crypto::PublicKey *>(transactionKey.data());

        // sources[].realOutput.transactionIndex
        // This is the index of the real output in the outputs list
        keyInfo.realOutput.transactionIndex = Nan::Get(realOutputValue, Nan::New("transactionIndex").ToLocalChecked()).ToLocalChecked()->NumberValue();

        // sources[].realOutput.outputInTransaction
        keyInfo.realOutput.outputInTransaction = Nan::Get(realOutputValue, Nan::New("outputInTransaction").ToLocalChecked()).ToLocalChecked()->NumberValue();

        // sources[].outputs
        v8::Local<v8::Object> outputsJs = Nan::Get(sourceJs, Nan::New("outputs").ToLocalChecked()).ToLocalChecked()->ToObject();
        size_t outputsJsLength = Nan::Get(outputsJs, Nan::New("length").ToLocalChecked()).ToLocalChecked()->Uint32Value();
        for (size_t j = 0; j < outputsJsLength; j++) {
            v8::Local<v8::Object> outputJs = Nan::Get(outputsJs, j).ToLocalChecked()->ToObject();

            GlobalOutput globalOutput;

            // sources[].outputs[].outputIndex
            globalOutput.outputIndex = Nan::Get(outputJs, Nan::New("globalIndex").ToLocalChecked()).ToLocalChecked()->NumberValue();

            // sources[].outputs[].targetKey
            v8::Local<v8::Value> keyValue = Nan::Get(outputJs, Nan::New("key").ToLocalChecked()).ToLocalChecked();
            std::vector<uint8_t> targetKey = fromHex(std::string(*Nan::Utf8String(keyValue->ToString())));
            globalOutput.targetKey = *reinterpret_cast<Crypto::PublicKey *>(targetKey.data());

            keyInfo.outputs.push_back(std::move(globalOutput));
        }
        
        InputInfo inputInfo;
        inputInfo.keyInfo = std::move(keyInfo);
        sources.push_back(std::move(inputInfo));
    }

    // destinations
    std::vector<OutputInfo> destinations;
    v8::Local<v8::Object> destinationsJs = Nan::Get(transactionJs, Nan::New("destinations").ToLocalChecked()).ToLocalChecked()->ToObject();
    size_t destinationsJsLength = Nan::Get(destinationsJs, Nan::New("length").ToLocalChecked()).ToLocalChecked()->Uint32Value();
    for (size_t i = 0; i < destinationsJsLength; i++) {
        v8::Local<v8::Object> destinationJs = Nan::Get(destinationsJs, i).ToLocalChecked()->ToObject();
        OutputInfo outputInfo;

        // destinations[].amount
        outputInfo.amount = Nan::Get(destinationJs, Nan::New("amount").ToLocalChecked()).ToLocalChecked()->NumberValue();

        // destinations[].address
        v8::Local<v8::Value> addressValue = Nan::Get(destinationJs, Nan::New("address").ToLocalChecked()).ToLocalChecked();
        std::string address = std::string(*Nan::Utf8String(addressValue->ToString()));
        uint64_t prefix = parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX;
        if (!parseAccountAddressString(prefix, outputInfo.address, address)) {
            return Nan::ThrowError(Nan::New("invalid address in output " + std::to_string(i)).ToLocalChecked());
        }

        destinations.push_back(std::move(outputInfo));
    }

    Nan::AsyncQueueWorker(new BuildTransactionAsyncWorker(
        unlockTime,
        senderKeys,
        extra,
        sources,
        destinations,
        new Nan::Callback(info[1].As<v8::Function>())));

    info.GetReturnValue().Set(Nan::Undefined());
}