const assert = require('assert');
const { address, spendKey, viewKey } = require('./fixtures/test.keys.json');
const { decomposeAmount, checkRingSignature, generateKeyImage, buildTransaction } = require('../');

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

    it('should build transaction', (done) => {
        const tx = buildTransaction({
            "unlockTime": 0,
            "extra": "",
            "sender": {
                address,
                viewSecret: viewKey.secret,
                spendSecret: spendKey.secret
            },
            "sources": [
                {
                    "amount": 100000000,
                    "realOutput": {
                        "transactionIndex": 3,
                        "transactionKey": "59170d0e17e3df7a36ff838a069a017583acbb993deca09d7594a330b123bded",
                        "outputInTransaction": 4
                    },
                    "outputs": [
                        {
                            "globalIndex": 10408,
                            "key": "5c978096d515696948382e85206b81acd2088925da4c1f7d829f5229d44ee631"
                        },
                        {
                            "globalIndex": 65066,
                            "key": "830e2625491ce8296d244cca14cc1d961727448b3b621beebd07d2855cfb295d"
                        },
                        {
                            "globalIndex": 69945,
                            "key": "81f995e8b51dfbaebd4739e8a17d45e34a99b454dc2273a11b4e6dd86e5cf822"
                        },
                        {
                            "globalIndex": 110586,
                            "key": "a81674065c21945ddd80c1f54b8f43633b0325fe279c2110ce5c7f57a928d6fa"
                        }
                    ]
                }
            ],
            "destinations": [
                {
                    "address": "fyTpS7UgsfTJaGWxKoBcYNAie7ZXkia8UKarA8mAEnusGpM1JQPAzvFd6S8fpDR7WgEN9wTVL6vdBe3PjPmqafpm2PogvpQna",
                    "amount": 10000000
                },
                {
                    "type": "change",
                    "address": "fySYVoeryY4WZmM6vpduKBccALWaLukXfQ6KwsFgg8pR8HhjHw3AeCiDrXfqEPSYnkgbK74CRBat9YETn8qwj7aN23TMRqiTh",
                    "amount": 80000000
                }
            ]
        }, function (error, result) {
            if (error) {
                console.error(error);
                done(error);
            }

            assert(result.hash);
            assert(result.data);
            done();
        });

    });
});