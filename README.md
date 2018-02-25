# bbscoin-helpers

Native Node.js module for BBSCoin. This should work for all cryptonote based cryptocurrencies.

## Usages

Create a GUI compatible wallet with a single spending key.
```javascript
createWallet(filePath, walletPassword, callback);
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

## Compilation

First, please clone bbscoin project to cryptonote-helpers' root or create a symbol link to ./bbscoin.
Please follow the instructions to build it first: https://github.com/bbscoin/bbscoin 
As we will link static libraries from the build release folder.


### Requirement

- node (v9.5.0)
- boost

### macOS

```bash
brew install boost
npm install
node-gyp rebuild
```