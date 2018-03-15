#!/usr/bin/env node
const { parseWallet } = require('../');
const args = process.argv.slice(2);
const argv = require('minimist')(args);

if (!args.length) {
    console.log('Usage: ./bin/export-key.js --path path [--pass password]');
    process.exit(1);
}

parseWallet(String(argv.path), argv.pass ? String(argv.pass) : '', (error, parseResult) => {
    if (error) {
        return console.error(error);
    }

    console.log(parseResult);
});