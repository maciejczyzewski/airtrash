var NativeExtension = require('bindings')('airtrash');

module.exports = (input, callback) => {
  console.log("SCAN")
  callback(null, NativeExtension.scan().split("|").filter(Boolean))
}
