#pragma once

#include <nan.h>

class WalletBinding {
public:
  static NAN_MODULE_INIT(Init);
  static NAN_METHOD(CreateWallet);
};