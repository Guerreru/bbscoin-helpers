const assert = require('assert');
const path = require('path');
const fs = require('fs');
const { address, spendKey, viewKey } = require('./fixtures/test.keys.json');
const { createWallet, generateWallet, parseWallet } = require('../');

describe('wallet', () => {
    let filePath

    beforeEach(() => {
        filePath = path.resolve(__dirname, path.join('test.wallet'));
        if (fs.existsSync(filePath)) {
            fs.unlinkSync(filePath);
        }
    });

    it('should create new wallet', (done) => {
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

    it('should generate wallet file from keys', (done) => {
        generateWallet(filePath, '123456', address, viewKey.secret, spendKey.secret, (error, generateResult) => {
            if (error) {
                return done(error);
            }

            parseWallet(filePath, '123456', (error, parseResult) => {
                if (error) {
                    return done(error);
                }
        
                assert(generateResult.address === parseResult.address);
                assert(generateResult.timestamp === parseResult.timestamp);
                assert(generateResult.viewKey.public === parseResult.viewKey.public);
                assert(generateResult.viewKey.secret === parseResult.viewKey.secret);
                assert(generateResult.spendKey.public === parseResult.spendKey.public);
                assert(generateResult.spendKey.secret === parseResult.spendKey.secret);
                done();
            });
        })
    });
});