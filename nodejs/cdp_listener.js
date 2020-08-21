const dgram = require('dgram');
const parseCdp = require('./cdp').parseCdp;
const parsePositionV3 = require('./position_v3').parsePositionV3;

const CDP__POSITION_V3__TYPE = 0x0135;

const cdpClient = dgram.createSocket('udp4');

let bound = false;

function cdpListener(PORT, HOST, GROUP, callback) {
  if (!bound) {

    cdpClient.bind(PORT, () => {
      cdpClient.setBroadcast(true);
      cdpClient.setMulticastTTL(128);
      cdpClient.addMembership(GROUP, HOST)
    })
    console.log("Listening for CDP packets at " + GROUP + ":" + PORT + " on " + HOST);
    bound = true;
  }

  cdpClient.on('message', (msg) => {
    const cdp = parseCdp(msg);
    if (cdp) {
      cdp.dataItems.forEach((dataItem) => {
        if (dataItem.type == CDP__POSITION_V3__TYPE) {
          const positionV3 = parsePositionV3(dataItem.data, cdp.header);
          if (positionV3) {
            callback(positionV3);
          }
        }
      })
    }
  })
}
exports.cdpListener = cdpListener;
