const { spendKey, viewKey } = require('./fixtures/test.keys.json');
const util = require('util');
const fs = require('fs');
const path = require('path');
const request = require('request-promise');
const {
    findOutputs
} = require('../');

const fsWriteFile = util.promisify(fs.writeFile);

const blockHeights = [22733, 25605, 25606, 25616];

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
    blockHeights.sort((a, b) => b - a).forEach((height) => {
        readTxData(height).result.transactions.forEach((transaction) => {
            findOutputs(transaction.publicKey, transaction.outputs, viewKey.secret, [spendKey.public], function(error, results) {
                if (error) {
                    console.error(error);
                }
        
                if (results[spendKey.public]) {
                    const amount = results[spendKey.public].reduce((sum, current) => sum + current.amount, 0);
                    console.log(`Hash: ${transaction.hash}\tAmount: ${amount / 100000000}\tFee: ${transaction.fee / 100000000}`);
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