if (process.env.DEBUG) {
    module.exports = require('./build/Debug/bbscoinHelper.node')
} else {
    module.exports = require('./build/Release/bbscoinHelper.node')
}