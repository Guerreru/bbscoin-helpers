const { spendKey, viewKey } = require('./fixtures/test.keys.json');
const util = require('util');
const fs = require('fs');
const path = require('path');
const request = require('request-promise');
const {
    findOutputs
} = require('../');

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
                'method':'on_get_txs_by_height',
                'params':{
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

function printTxs() {
    const knownPublicKeys = {};
    
    blockHeights.sort().forEach((height) => {
        readTxData(height).result.transactions.forEach((transaction) => {
            findOutputs(transaction.publicKey, transaction.outputs, viewKey.secret, [spendKey.public], function(error, results) {
                if (error) {
                    console.error(error);
                }

                if (results[spendKey.public]) {
                    console.log(`hash: ${transaction.hash} height: ${height}`);
                    console.log(`fee: ${transaction.fee / 100000000}`);

                    let outputSum = 0;
                    
                    results[spendKey.public].forEach((output) => {
                        outputSum += output.amount;
                        knownPublicKeys[output.amount] = knownPublicKeys[output.amount] || {};
                        knownPublicKeys[output.amount][output.globalIndex] = output.key;
                        console.log(`${output.key} received: ${output.amount / 100000000}`);
                    });

                    let inputSum = 0;
                    transaction.inputs.forEach((input) => {
                        input.globalIndexes.forEach((globalIndex) => {
                            const key = (knownPublicKeys[input.amount] || {})[globalIndex];
                            if (key) {
                                inputSum += input.amount;
                                console.log(`${key} sent: ${input.amount / 100000000}`);
                            }
                        });
                    });

                    console.log(`Final: ${outputSum / 100000000} - ${inputSum / 100000000} = ${(outputSum - inputSum) / 100000000}`);
                    console.log(`===`);
                }
            });
        });
    });
}

async function test() {
    await prepareData();
    printTxs();
}

test();