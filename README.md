# bbscoin-helpers

Native Node.js module for BBSCoin. This should work for all cryptonote based cryptocurrencies.

## Usages

Create a GUI compatible wallet file with a single spending key.
```javascript
createWallet(filePath, walletPassword, callback);
```

Create a GUI compatible wallet file with given keys.
```javascript
generateWallet(filePath, walletPassword, address, viewSecret, spendSecret, callback);
```

Parse a GUI wallet file.
```javascript
parseWallet(filePath, walletPassword, callback);
```

Create a new public/secret key pair.
```javascript
generateNewKeyPair();
```

Create an address from public keys.
```javascript
generateAddressFromKeyPair(spendPublicKey, viewPublicKey);
```

Get public view and spend key from a wallet address.
Returns null if the address is not valid.
```javascript
getKeyPairFromAddress(walletAddress);
```

Generate public key from secret key
```javascript
secretKeyToPublicKey(secretKey)
```

Filter transaction output by given view and spend keys.
```javascript
const transactionPublicKey = '64 characters long hex key';
const transactionOutputs = [
    {
        key: '64 characters long hex key'
        amount: 100000,
    }
]
const viewSecretKey = '64 characters long hex key';
const publicSpendKeys = [
    '64 characters long hex key'
]
findOutputs(transactionPublicKey, transactionOutputs, viewSecretKey, publicSpendKeys, callback)
```

Check whether the ring signature is correct
```javascript
checkRingSignature(txPrefixHash, keyImage, outputPublicKeysFromInput, signatures);
```

Generate key image
```javascript
generateKeyImage({ address, viewSecret, spendSecret }, transactionHash, indexInOutput);
```

Decompose amount
```javascript
decomposeAmount(amount, dustThreshold)
// eg:
decomposeAmount(62387455827, 500000) === [
    455827,
    7000000,
    80000000,
    300000000,
    2000000000,
    60000000000
]
```

## Compilation
### Requirement

- node (v9.5.0+)
- boost

### macOS
You will need the daemon source code in order to compile this module
```bash
brew install boost
ln ../bbscoin bbscoin
npm install
node-gyp rebuild
```

### Test
```bash
npm test
```
