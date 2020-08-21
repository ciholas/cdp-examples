const ciholasSerial = require('./ciholas_serial_number').ciholasSerial;

function parsePositionV3(unparsedData, cdpHeader) {

  let index = 0;

  // Serial number of the device emitting position data
  const positionSerialString = ciholasSerial(unparsedData.readUInt32LE(index)).string;
  index += 4;

  const networkTime = unparsedData.readBigUInt64LE(index);
  index += 8;

  const positionX = unparsedData.readInt32LE(index);
  const positionY = unparsedData.readInt32LE(index + 4);
  const positionZ = unparsedData.readInt32LE(index + 8);
  index += 12;

  // Quality
  index += 2;

  // Anchor count
  index += 1;

  // Flags
  index += 1;

  // Smoothing
  index += 2;

  const parsedPosData = {
    networkInstance: cdpHeader.serialNumber,
    serialNumber: positionSerialString,
    positionX: positionX,
    positionY: positionY,
    positionZ: positionZ,
    networkTime: networkTime
  }

  return parsedPosData;
}
exports.parsePositionV3 = parsePositionV3;
