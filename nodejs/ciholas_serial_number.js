function ciholasSerial(intSerialNumber) {

  intSerialNumber = intSerialNumber.toString(16);
  while(intSerialNumber.length < 8){
    intSerialNumber = "0" + intSerialNumber;
  }
  // Convert serial number to Ciholas format: vendorID:productID:deviceID (AA:BB:CCDD)
  const formattedSerialString = (intSerialNumber.slice(0, 2) + ":" +
    intSerialNumber.slice(2, 4) + ":" + intSerialNumber.slice(4, 8)).toUpperCase();

  const serial = {
    string: formattedSerialString,
    number: intSerialNumber
  }
  return serial;
}
exports.ciholasSerial = ciholasSerial;
