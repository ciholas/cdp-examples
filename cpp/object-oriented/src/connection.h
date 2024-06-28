// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__CONNECTION__H_
#define _UTILS__CDP__CONNECTION__H_

#include "receiver.h"
#include "transmitter.h"

class CdpPacket;

// Provides the interface for all CDP communications.
class CdpConnection {
public:
    // Constructs this.
    explicit CdpConnection(char * multicast_address, uint16_t port, char * iface_string);
    // Destroys this.
    ~CdpConnection(void);

    // Modifier Functions
    // Sends packet on the connection.
    void Publish(CdpPacket& packet);
    // Removes all of the connection's subscriptions.
    void ClearSubscriptions(void);
    // Sets up callback to be run when a CDP packet containing a data item matching type is received.
    uint32_t Subscribe(uint16_t type, cdp_callback_t callback);
    // Removes the subscription with the given id.
    void Unsubscribe(uint32_t id);

private:
  
    // Handles all packet transmissions on this CDP connection.
    CdpTransmitter* mTransmitter;
    // Handles all receptions on this CDP connection.
    CdpReceiver*    mReceiver;
    // The thread that handles UDP reception on the CDP connection.
    std::thread     mReceiverThread;
    // A mutex to control access to the connection's transmitter.
    std::mutex      mTransmitterMutex;
};

#endif // _UTILS__CDP__CONNECTION__H_
