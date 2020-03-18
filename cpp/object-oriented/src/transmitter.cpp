// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include "transmitter.h"

#include <string.h>
#include <unistd.h>
#include "data_item.h"
#include "packet.h"

CdpTransmitter::CdpTransmitter(char * group_string, uint16_t port) {
    // Create a socket.
    if ((__cdp_socket = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        printf("Error attempting to create transmit socket!\n");
    }

    // Setup transmitter ip address.
    memset(&__ifc_address, 0, sizeof(__ifc_address));
    __ifc_address.sin_family=AF_INET;
    __ifc_address.sin_addr.s_addr = inet_addr(group_string);
    __ifc_address.sin_port = htons(port);
}

CdpTransmitter::~CdpTransmitter(void) {
    close(__cdp_socket);
}

void CdpTransmitter::Publish(CdpPacket& packet) {
    uint8_t* _buffer = packet.CreateBuffer();
    CdpPacket::SetBufferSequenceNumber(_buffer, __sequence_number.fetch_add(1));
    if (sendto(__cdp_socket, _buffer, packet.GetPacketSize(), 0, (struct sockaddr *) &__ifc_address, sizeof(__ifc_address)) < 0) {
        printf("Error attempting to send packet!\n");
    }
    delete[] _buffer;
}
