var NativeExtension = require('bindings')('airtrash');

module.exports = (input, callback) => {
  console.log("PULL", input.address, input.path)
  NativeExtension.pull(input.address, input.path)
  callback(null, input)
}
