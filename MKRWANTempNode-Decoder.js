function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  decoded.humidity=bytes[0];   
  decoded.temperature=bytes[1]-128;
  if(bytes.length==3) decoded.battery=bytes[2]/10;
  return decoded;
}
