# bbscoin-helpers

Native Node.js module for BBSCoin, however this should work for all cryptonote based cryptocurrencies.

## Usages

```javascript
createWallet(file_path, container_password, callback);
```
Create a wallet with a single spending key.

```javascript
generateNewKeyPair();
```
Create a new public/secret key pair

```javascript
generateAddressFromKeyPair(spendPublicKey, viewPublicKey);
```
Create an address from public keys


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