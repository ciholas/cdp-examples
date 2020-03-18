// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include "connection.h"
#include "data_item.h"
#include "receiver.h"
#include "transmitter.h"

CdpConnection::CdpConnection(char * multicast_address, uint16_t port) {
    mTransmitter = new CdpTransmitter(multicast_address, port);
    mReceiver = new CdpReceiver(multicast_address, port);
    mReceiverThread = std::thread(std::bind(&CdpReceiver::Run, mReceiver));
    mReceiverThread.detach();
}

CdpConnection::~CdpConnection(void) {
    if (mReceiverThread.joinable()) {
        mReceiverThread.join();
    }

    if (mReceiver != NULL) {
        mReceiver->ClearSubscriptions();
        mReceiver->Shutdown();
        delete mReceiver;
    }

    if (mTransmitter != NULL) {
        delete mTransmitter;
    }
}

uint32_t CdpConnection::Subscribe(uint16_t type, cdp_callback_t callback) {
    if (mReceiver != NULL) {
        return mReceiver->Subscribe(type, callback);
    }

    return 0;
}

void CdpConnection::Unsubscribe(uint32_t id) {
    if (mReceiver != NULL) {
        mReceiver->Unsubscribe(id);
    }
}

void CdpConnection::Publish(CdpPacket& packet) {
    if (mTransmitter != NULL) {
        mTransmitter->Publish(packet);
    }
}

void CdpConnection::ClearSubscriptions(void) {
    if (mReceiver != NULL) {
        mReceiver->ClearSubscriptions();
    }
}
