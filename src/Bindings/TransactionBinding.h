#pragma once

#include <nan.h>
#include "crypto/crypto.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "Common/StringTools.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"

using namespace CryptoNote;
using namespace Common;

class TransactionBinding {
  public:
    static NAN_MODULE_INIT(Init);
    static NAN_METHOD(DecomposeAmount);
    static NAN_METHOD(BuildTransaction);
    static NAN_METHOD(CheckRingSignature);
    static NAN_METHOD(GenerateKeyImage);
};