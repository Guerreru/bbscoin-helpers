#include <nan.h>
#include "Bindings/WalletBinding.h"
#include "Bindings/TransactionBinding.h"

NAN_MODULE_INIT(InitModule) { 
    WalletBinding::Init(target); 
    TransactionBinding::Init(target); 
}

NODE_MODULE(cryptonoteHelper, InitModule);