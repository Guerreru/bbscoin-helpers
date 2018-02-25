const assert = require('assert');

const { 
    generateNewKeyPair,
    generateAddressFromKeyPair,
    getKeyPairFromAddress,
} = require('../');

const view = generateNewKeyPair();
const spend = generateNewKeyPair();
const address = generateAddressFromKeyPair(spend.public, view.public);
const parsedPublicPair = getKeyPairFromAddress(address);

assert(parsedPublicPair.view === view.public);
assert(parsedPublicPair.spend === spend.public);

console.log('passed');

assert(getKeyPairFromAddress('unvalid_address') === null);

console.log('passed');