var NativeExtension = require('bindings')('airtrash');

module.exports = (input, callback) => {
  console.log("PUSH", input.address, input.path)
  NativeExtension.push(input.address, input.path)
  callback(null, input)
}
