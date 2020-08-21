const ciholasSerial = require('./ciholas_serial_number').ciholasSerial;

const CDP__PACKET_HEADER__SIZE = 20;
const CDP__PACKET_HEADER__MARK = 0x3230434c;
const CDP__DATA_ITEM_HEADER__SIZE = 4;

function parseCdp(msg) {

  let index = 0;

  // CDP packet header:
  const headerMark = msg.readUInt32LE(index);
  index += 4;
  const headerSequence = msg.readUInt32LE(index);
  index += 4;
  const headerString = msg.toString("utf8", index, index + 8);
  index += 8;
  const senderSerialNumber = ciholasSerial(msg.readUInt32LE(index)).string;
  index += 4;

  let cdp = undefined;

  if (headerMark === CDP__PACKET_HEADER__MARK && headerString === 'CDP0002\0' && msg.length >= CDP__PACKET_HEADER__SIZE) {

    cdp = {
      header: {
        mark: headerMark,
        sequence: headerSequence,
        string: headerString,
        serialNumber: senderSerialNumber
      },
      dataItems: [
        /**
         * type:
         * size:
         * data:
         */
      ]
    }

    while (index < msg.length - CDP__DATA_ITEM_HEADER__SIZE) {
      const dataItemType = msg.readUInt16LE(index);
      index += 2;
      const dataItemSize = msg.readUInt16LE(index);
      index += 2;
      const dataItemData = msg.slice(index, index + dataItemSize);
      index += dataItemSize;

      cdp.dataItems.push({type: dataItemType, size: dataItemSize, data: dataItemData});
    }
  }
  return cdp;
}
exports.parseCdp = parseCdp;
