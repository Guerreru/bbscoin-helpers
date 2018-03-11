const assert = require('assert');
const { address, spendKey, viewKey } = require('./fixtures/test.keys.json');
const { decomposeAmount, checkRingSignature, generateKeyImage } = require('../');

describe('transaction helpers', () => {
    it('should decompose amount', () => {
        const amounts = decomposeAmount(62387455827, 500000);
        assert.deepEqual(amounts, [
            455827,
            7000000,
            80000000,
            300000000,
            2000000000,
            60000000000
        ]);
    });

    it('should check ring signatures', () => {
        assert(checkRingSignature(
            'ab10639c0e1cc1861c062f81126be7af04e40d27bc5d8229b781488d21946a2c',
            '0a0d183640f37d6928be9015197694826b07d302a0c3e3784c154c99515fcc40',
            ['811c81aab650afcf2ef8d9586410ec86848e29db3aa175e3d93a9231ead065f7'],
            ['8f7a4752e2c048e15527853b53ffbb2672d87eb1fec7fa41fb02199b6b96010f2e5eb716c7f28ed09fd238ab37bf248b25320d36d6f54dc0fc7e6351d6117907']) === true);

        assert(checkRingSignature(
            'ab10639c0e1cc1861c062f81126be7af04e40d27bc5d8229b781488d21946a2c',
            '9abd09b77c9d38518a4264d07dff5d5035946ad117e3372530cd5959e3e97c9e',
            ['56d3233d18af5ee7ccc3c6b4acb114eeaa48417c2785fa444cd4bce81da1e603'],
            ['9d6c194e8301ac8a29f02270e99c52c545985d5812ee1d163127abfd77350f0f2aa4b329d0253a33ae47f4b23623091b0570314bce69bea1360f951d65391e08']) === true);

        assert(checkRingSignature(
            'ab10639c0e1cc1861c062f81126be7af04e40d27bc5d8229b781488d21946a2c',
            '9abd09b77c9d38518a4264d07dff5d5035946ad117e3372530cd5959e3e97c9e',
            ['56d3233d18af5ee7ccc3c6b4acb114eeaa48417c2785fa444cd4bce81da1e603'],
            ['9d6c194e8301ac8a29f02270e99c52c545985d5812ee1d163127abfd77350f0f2aa4b329d0253a33ae47f4b23623091b0570314bce69bea1360f951d65391e08']) === true);
    });

    it('should generate key image', () => {
        assert(generateKeyImage({
            address,
            viewSecret: viewKey.secret,
            spendSecret: spendKey.secret
        }, '486ae678af345ff7cbd7cf7800ae1d11a7d39f7ee80a6e096afe2313cf93a1da', 6) === '56ef7da1fce4f142d44defcd392709ec94ad2bd3d68786372269f8a94b31936f');
    });
});