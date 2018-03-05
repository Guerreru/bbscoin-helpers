#pragma once

#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include <nan.h>

using namespace CryptoNote;

class TransactionBinding {
  public:
    static NAN_MODULE_INIT(Init);
    static NAN_METHOD(DecomposeAmount);
    static NAN_METHOD(BuildTransaction);
};