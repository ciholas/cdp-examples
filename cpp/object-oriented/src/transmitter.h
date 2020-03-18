// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__TRANSMITTER__H_
#define _UTILS__CDP__TRANSMITTER__H_

#include <atomic>
#include <arpa/inet.h>

class CdpPacket;

// Handles transmitting CDP packets through a UDP socket.
class CdpTransmitter {
public:

    /////////////////////
    // Class Functions //
    /////////////////////
    // Constructs this.
    // Parameters are the group string and the port.
    explicit CdpTransmitter(char * group_string, uint16_t port);

    // Destroys this.
    ~CdpTransmitter(void);


    ////////////////////////
    // Modifier Functions //
    ////////////////////////
    // Transmit packet on the UDP socket.
    // The parameter is the CDP packet to transmit.
    void Publish(CdpPacket& packet);

private:

    // The sequence number of the next CDP packet to be sent.
    std::atomic_uint_fast32_t __sequence_number;

    // Send to socket.
    int __cdp_socket;

    // Send to address struct
    struct sockaddr_in __ifc_address;
};

#endif // _UTILS__CDP__TRANSMITTER__H_
