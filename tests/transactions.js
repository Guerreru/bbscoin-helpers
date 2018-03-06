const assert = require('assert');
const { spendKey, viewKey } = require('./fixtures/test.keys.json');
const util = require('util');
const fs = require('fs');
const path = require('path');
const request = require('request-promise');
const { findOutputs } = require('../');

const fsWriteFile = util.promisify(fs.writeFile);

const blockHeights = [
    22733,
    25605,
    25606,
    26100,
    25616,
    26107,
    26216,
    26218,
    26915
];

async function prepareData() {
    const fixturesFolder = path.resolve(__dirname, `fixtures/tx-data`);

    return Promise.all(blockHeights.map((height) => {
        return request({
            method: 'POST',
            uri: 'http://localhost:21204/json_rpc',
            body: {
                'method': 'on_get_txs_by_height',
                'params': {
                    'height': height
                }
            },
            json: true
        }).then((data) => {
            return fsWriteFile(`${fixturesFolder}/${height}.json`, JSON.stringify(data, null, '  '));
        });
    }))
}

function readTxData(height) {
    return JSON.parse(fs.readFileSync(path.resolve(__dirname, `fixtures/tx-data/${height}.json`)));
}

function getTransactions() {
    const knownPublicKeys = {};

    return Promise.all(blockHeights.sort().map((height) => {
        return Promise.all(readTxData(height).result.transactions.map((transaction) => new Promise((resolve, reject) => {
            return findOutputs(transaction.publicKey, transaction.outputs, viewKey.secret, [spendKey.public], function (error, results) {
                if (error) {
                    return reject(error);
                }

                if (results[spendKey.public]) {
                    let outputSum = 0;

                    results[spendKey.public].forEach((output) => {
                        outputSum += output.amount;
                        knownPublicKeys[output.amount] = knownPublicKeys[output.amount] || {};
                        knownPublicKeys[output.amount][output.globalIndex] = output.key;
                    });

                    let inputSum = 0;
                    transaction.inputs.forEach((input) => {
                        input.globalIndexes.forEach((globalIndex) => {
                            const key = (knownPublicKeys[input.amount] || {})[globalIndex];
                            if (key) {
                                inputSum += input.amount;
                            }
                        });
                    });

                    return resolve({
                        outputSum,
                        inputSum
                    });
                }

                resolve(null);
            });
        }))).then((results) => {
            return results.reduce((result, item) => {
                return item === null ? result : result.concat(item);
            }, [])
        });
    })).then((results) => {
        return results.reduce((result, item) => {
            return item === null ? result : result.concat(item);
        }, [])
    });
}

describe('transactions', () => {
    it('should filter outputs', async () => {
        await prepareData();
        const transactions = await getTransactions();
        const sum = transactions.reduce((sum, item) => sum + item.outputSum - item.inputSum, 0);
        assert.deepEqual(transactions, [
            { outputSum: 100000000, inputSum: 0 },
            { outputSum: 90000000, inputSum: 100000000 },
            { outputSum: 1000000000, inputSum: 0 },
            { outputSum: 120000000, inputSum: 130000000 },
            { outputSum: 260000000, inputSum: 300000000 },
            { outputSum: 12345678, inputSum: 0 },
            { outputSum: 39900000, inputSum: 60000000 },
            { outputSum: 379800000, inputSum: 479900000 },
            { outputSum: 31234578, inputSum: 542345678 },
            { outputSum: 249934578, inputSum: 350034578 }
        ]);
        assert((sum / 100000000 - 3.20934578) <= 1e-9);
    });
});