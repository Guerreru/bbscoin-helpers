const assert = require('assert');
const { decomposeAmount } = require('../');

function test() {
    const amounts = decomposeAmount(62387455827, 500000);
    assert.deepEqual(amounts, [
        455827,
        7000000,
        80000000,
        300000000,
        2000000000,
        60000000000
    ]);
    console.log('pass');
}

test();