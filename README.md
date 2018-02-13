# cryptonote-helpers

Native Node.js module for CryptoNote based cryptocurrencies.


## Usages
Create a wallet with a single spending key. (In bytecoin's GreenWallet format)
```
createWallet(file_path, container_password, callback);
```

## Compilation
First, please clone bbscoin project to the project root and build it.
Because we will require static libraries from it.
For instructions regarding to how to build bbscoin, please refer to: https://github.com/bbscoin/bbscoin 

### macOS
```
brew install boost
node-gyp rebuild
```