const assert = require('assert');
const path = require('path');
const fs = require('fs');

const { 
    createWallet,
    generateNewKeyPair,
    generateAddressFromKeyPair
} = require('../');


const filePath = path.resolve(__dirname, path.join('tmp', 'test.wallet'));
if (fs.existsSync(filePath)) {
    fs.unlinkSync(filePath);
}
createWallet(filePath, '123456', (error, result) => {
    if (error) {
        console.error(error);
        return;
    }

    console.log("Wallet key pairs");
    console.log(result);

    console.log("New key pair");
    console.log(generateNewKeyPair());

    assert(generateAddressFromKeyPair(result.spendKey.public, result.viewKey.public) === result.address);
});