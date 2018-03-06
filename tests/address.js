const assert = require('assert');
const { 
    generateNewKeyPair,
    generateAddressFromKeyPair,
    getKeyPairFromAddress,
    secretKeyToPublicKey,
} = require('../');

describe('address', () => {
    let view;
    let spend;
    let address;

    before(() => {
        view = generateNewKeyPair();
        spend = generateNewKeyPair();
        address = generateAddressFromKeyPair(spend.public, view.public);
    });
    
    it('should parse key from address', () => {
        const parsedPublicPair = getKeyPairFromAddress(address);

        assert(parsedPublicPair.view === view.public);
        assert(parsedPublicPair.spend === spend.public);
    });

    it('should return null if public keys are not correct', () => {
        assert(generateAddressFromKeyPair('this', 'that') === null);
    });

    it('should return null if address is invalid', () => {
        assert(getKeyPairFromAddress('invalid_address') === null);
    });

    it('should generate public from secret', () => {
        assert(secretKeyToPublicKey(view.secret) === view.public);
    });

    it('should return null if secret is invalid', () => {
        assert(secretKeyToPublicKey('what') === null);
    });
});