if (process.env.DEBUG) {
    module.exports = require('./build/Debug/cryptonoteHelper.node')
} else {
    module.exports = require('./build/Release/cryptonoteHelper.node')
}