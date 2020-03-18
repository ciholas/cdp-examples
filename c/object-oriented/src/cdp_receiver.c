// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "cdp.h"
#include "cdp_receiver.h"
#include "utils/print.h"


/***********************
 * Private Definitions
 ***********************/
// Maximum sized UDP packet received.
#define _RECEIVE_BUFFER_SIZE 65536
// Maximum number of data item handlers.
#define _MAXIMUM_DATA_ITEM_HANDLERS 100


/***********************
 * Private Variables
 ***********************/
static struct {
    uint16_t type;
    cdp_data_item_handler_t handler;
}_registered_handlers[_MAXIMUM_DATA_ITEM_HANDLERS] = {};

/***********************
 * Public Functions
 ***********************/
void CdpReceiver_Create(char * group_string, uint16_t port) {

    // Create a socket.
    int cdp_socket;
    if((cdp_socket = socket(AF_INET,SOCK_DGRAM,0)) < 0)
        Print_ExitOnError("socket()");
    // Allow other apps to use the same port/ifc.
    static int so_reuse = 1;
    if(setsockopt(cdp_socket, SOL_SOCKET, SO_REUSEADDR, &so_reuse, sizeof(so_reuse)) < 0)
        Print_ExitOnError("setsockopt(SO_REUSEADDR)");
    // Setup receive ip address.
    struct sockaddr_in ifc_address;
    memset(&ifc_address, 0, sizeof(ifc_address));
    ifc_address.sin_family=AF_INET;
    ifc_address.sin_addr.s_addr = inet_addr(group_string);
    // Set the port.
    ifc_address.sin_port=htons(port);
    // Bind.
    if(bind(cdp_socket, (struct sockaddr *)&ifc_address, sizeof(ifc_address)) < 0)
        Print_ExitOnError("bind(2) - cdp report");
    // Setup the multicast for the GROUP.
    struct ip_mreq multicast_request;
    multicast_request.imr_multiaddr.s_addr=inet_addr(group_string);
    if(setsockopt(cdp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_request, sizeof(multicast_request)) < 0)
        Print_ExitOnError("setsockopt - multicast");

    // Declare variables for inside the loop to avoid stack thrashing.
    uint8_t receive_buffer[_RECEIVE_BUFFER_SIZE];
    cdp_header_t * cdp_header = (cdp_header_t *) receive_buffer;
    socklen_t addrlen = sizeof(ifc_address);
    int bytes_received;
    int invalid_cdp_packet_count=0;
    int ii=0;
    int jj;

    // Loop on reception of packet.
    while(1){

        // Block on receiving a UDP packet.
        if((bytes_received = recvfrom(cdp_socket, receive_buffer, sizeof(receive_buffer), 0, (struct sockaddr *)&ifc_address, &addrlen)) < 0)
            Print_ExitOnError("recvfrom(2) P3");

        // Check the CDP Header.
        if(cdp_header->mark == CDP_MARK_WORD && strcmp(cdp_header->string, CDP_STRING_WORD)==0){

            // Loop on data items.
            ii = 0;
            while(ii < (bytes_received - sizeof(cdp_header_t))){
                cdp_data_item_t * data_item = (cdp_data_item_t *) &cdp_header->payload[ii];

                // Try to use any handlers for the data item.
                for(jj=0; jj<_MAXIMUM_DATA_ITEM_HANDLERS && _registered_handlers[jj].type != 0; jj++){
                    if(_registered_handlers[jj].type == data_item->type){
                        _registered_handlers[jj].handler(cdp_header, data_item->payload, data_item->size);
                    }
                }

                // Move to next data item.
                ii += sizeof(cdp_data_item_t) + data_item->size;
            }
        }else{
            Print_Line("Received non-CDP packet (%d)", ++invalid_cdp_packet_count);
        }
    }
}

void CdpReceiver_RegisterDataItemHandler(uint16_t type, cdp_data_item_handler_t handler){
    for(int ii=0; ii<_MAXIMUM_DATA_ITEM_HANDLERS; ii++){
        if(_registered_handlers[ii].type == 0){
            _registered_handlers[ii].type    = type;
            _registered_handlers[ii].handler = handler;
            break;
        }
    }
}
