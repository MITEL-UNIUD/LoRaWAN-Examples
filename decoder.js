function Decoder(bytes, port) {
  // Decode an uplink message from two bytes to an object.
  //NB DHT-11 temperature range is 0-50, thus no negatives
  var decoded = {};

  decoded.humidity = bytes[0];
  decoded.temperature=bytes[1];

  return decoded;
}
