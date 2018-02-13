#include <nan.h>
#include "Bindings/WalletBinding.h"

NAN_MODULE_INIT(InitModule) { 
    WalletBinding::Init(target); 
}

NODE_MODULE(cryptonoteHelper, InitModule);