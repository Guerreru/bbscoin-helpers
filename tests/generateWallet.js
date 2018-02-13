const { createWallet } = require('../');
const path = require('path');

const filePath = path.resolve(__dirname, path.join('tmp', 'test.wallet'));
createWallet(filePath, '123456', (error, result) => {
    if (error) {
        console.error(error);
        return;
    }

    console.log(result);
});