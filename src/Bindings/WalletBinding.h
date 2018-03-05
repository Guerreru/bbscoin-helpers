#pragma once

#include <nan.h>
#include "CryptoNoteCore/CryptoNoteTools.h"

class WalletBinding {
public:
  static NAN_MODULE_INIT(Init);
  static NAN_METHOD(CreateWallet);
  static NAN_METHOD(ParseWallet);
  static NAN_METHOD(GenerateNewKeyPair);
  static NAN_METHOD(GenerateAddressFromKeyPair);
  static NAN_METHOD(GetKeyPairFromAddress);
  static NAN_METHOD(FindOutputs);
};