const assert = require('assert');
const path = require('path');
const fs = require('fs');

const { createWallet, parseWallet } = require('../');

const filePath = path.resolve(__dirname, path.join('tmp', 'test.wallet'));
if (fs.existsSync(filePath)) {
    fs.unlinkSync(filePath);
}
createWallet(filePath, '123456', (error, createResult) => {
    if (error) {
        console.error(error);
        return;
    }

    parseWallet(filePath, '123456', (error, parseResult) => {
        if (error) {
            console.error(error);
            return;
        }

        assert(createResult.address === parseResult.address);
        assert(createResult.timestamp === parseResult.timestamp);
        assert(createResult.viewKey.public === parseResult.viewKey.public);
        assert(createResult.viewKey.secret === parseResult.viewKey.secret);
        assert(createResult.spendKey.public === parseResult.spendKey.public);
        assert(createResult.spendKey.secret === parseResult.spendKey.secret);
        console.log('passed');
    });
});