const assert = require('assert');
const path = require('path');
const fs = require('fs');
const { createWallet, parseWallet } = require('../');

describe('wallet', () => {
    let filePath

    before(() => {
        filePath = path.resolve(__dirname, path.join('test.wallet'));
        if (fs.existsSync(filePath)) {
            fs.unlinkSync(filePath);
        }
    });

    it('should generate new wallet', (done) => {
        createWallet(filePath, '123456', (error, createResult) => {
            if (error) {
                return done(error);
            }
        
            parseWallet(filePath, '123456', (error, parseResult) => {
                if (error) {
                    return done(error);
                }
        
                assert(createResult.address === parseResult.address);
                assert(createResult.timestamp === parseResult.timestamp);
                assert(createResult.viewKey.public === parseResult.viewKey.public);
                assert(createResult.viewKey.secret === parseResult.viewKey.secret);
                assert(createResult.spendKey.public === parseResult.spendKey.public);
                assert(createResult.spendKey.secret === parseResult.spendKey.secret);
                done();
            });
        });
    });
});