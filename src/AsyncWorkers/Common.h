#pragma once

#include "CryptoNote.h"
#include "Wallet/WalletGreen.h"

struct ContainerStoragePrefix {
    uint8_t version;
    Crypto::chacha8_iv nextIv;
    CryptoNote::EncryptedWalletRecord encryptedViewKeys;
};