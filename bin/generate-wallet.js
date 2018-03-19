#!/usr/bin/env node
const { generateWallet } = require('../');
const path = require('path');
const args = process.argv.slice(2);
const argv = require('minimist')(args);

if (!args.length || !argv['out'] || !argv['address'] || !argv['view-secret'] || !argv['spend-secret']) {
    console.log('Usage: ./bin/export-key.js --out out [--pass password] --address address --view-secret secret --spend-secret secret');
    process.exit(1);
}

generateWallet(argv.out, argv.pass ? String(argv.pass) : '', argv['address'], argv['view-secret'], argv['spend-secret'], (error, parseResult) => {
    if (error) {
        return console.error(error);
    }

    console.log(`Wallet file saved at ${path.resolve(__dirname, argv.out)}`);
});