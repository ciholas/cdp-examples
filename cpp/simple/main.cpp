// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0

#include <arpa/inet.h>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

# define PACKED __attribute__((__packed__))
// Maximum size of any UDP packet received
#define RECEIVE_BUFFER_SIZE 65536

// The expected value of the CDP Header MARK field
#define CDP_MARK_WORD 0x3230434C
// The expected value of the CDP Header STRING field
#define CDP_STRING_WORD "CDP0002"
// The CDP Packet Header has a total size of 20 bytes
// CDP Header container type
typedef struct PACKED CDP_HEADER_T {
    uint32_t mark;           // Mark: The magic word '0x3230434C' in little-endian (842023756 in decimal) (4 bytes)
    uint32_t sequence;       // Sequence: The sequence number of the CDP packet (4 bytes)
    uint8_t  string[8];      // String: The ASCII string 'CDP0002\0' (8 bytes)
    uint32_t serial_number;  // Serial Number: Unique identifier of the reporting device (4 bytes)
    uint8_t  payload[];      // One or more data items included under this header
} cdp_header_t;

// The Data Item Header has a total size of 4 bytes
// CDP Data Header container type
typedef struct PACKED CDP_DATA_ITEM_T {
    uint16_t type;       // Type: The type of the CDP Data Item (2 bytes)
    uint16_t size;       // Size: The size of the Data Item, not including the Data Item Header (2 bytes)
    uint8_t  payload[];  // The remaining bytes in the Data Item are the payload
} cdp_data_item_t;

// The Position V3 (type 0x0135) data item has a total size of 30 bytes
// Position V3 Data Item container type
typedef struct PACKED CDP_POSITION_V3_T {
    uint32_t serial_number;  // Serial Number: The serial number of the reporting device (4 bytes)
    uint64_t network_time;   // Network time: The timestamp when the sensor recorded the data (8 bytes)
    int32_t x_in_mm;         // X-Coordinate: The x-coordinate from the origin (4 bytes)
    int32_t y_in_mm;         // Y-Coordinate: The y-coordinate from the origin (4 bytes)
    int32_t z_in_mm;         // Z-Coordinate: The z-coordinate from the origin (4 bytes)
    uint16_t quality;        // Quality: The quality of the computed position (2 bytes)
    uint8_t anchor_count;    // Anchor Count: The number of anchors involved in the calculation of this position (1 byte)
    uint8_t flags;           // Flags: 1 bit = inactive mode. 1 bit = was not calculated. 6 bits = unused. (1 byte)
    uint16_t smoothing;      // Smoothing: The effective smoothing factor (2 bytes)
} cdp_position_v3_t;

int main(int argc,char **argv) {
    // Ensure correct usage of the program
    if(argc != 4){
        std::cout <<
            "Usage:" << argv[0] << "GROUP PORT IFACE\r\n"
            "    GROUP - IP to listen on (ie: 239.255.76.67)\r\n"
            "     PORT - Port to listen on (ie: 7667)\r\n"
            "    IFACE - IP of network interface to bind socket to (ie: 127.0.0.1)\r\n" << std::endl;
    }
    char * group_string = argv[1];
    uint16_t port = atoi(argv[2]);
    char * iface_string = argv[3];

    std::cout << "Start CDP C++ Example Program" << std::endl;

    // The socket on which to receive CDP packets
    int cdp_socket;
    /// The socket interface on which we will be receiving CDP packets
    struct sockaddr_in __ifc_address;
    // Create a UDP socket for listening to CDP packets
    if((cdp_socket = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        std::cout << "Experienced error while creating socket." << std::endl;
        return 1;
    }
    // Allow other apps to use the same port/ifc
    static int so_reuse = 1;
    if(setsockopt(cdp_socket, SOL_SOCKET, SO_REUSEADDR, &so_reuse, sizeof(so_reuse)) < 0) {
        std::cout << "Experienced error while allowing other apps to use the same port/ifc." << std::endl;
        return 1;
    }
    // Setup receive ip address
    memset(&__ifc_address, 0, sizeof(__ifc_address));
    __ifc_address.sin_family=AF_INET;
    __ifc_address.sin_addr.s_addr = inet_addr(group_string);
    // Set the port
    __ifc_address.sin_port=htons(port);
    // Bind the socket
    if(bind(cdp_socket, (struct sockaddr *)&__ifc_address, sizeof(__ifc_address)) < 0) {
        std::cout << "Experienced error while binding the socket." << std::endl;
        return 1;
    }
    // Setup the multicast for the GROUP
    struct ip_mreq multicast_request;
    multicast_request.imr_multiaddr.s_addr=inet_addr(group_string);
    multicast_request.imr_interface.s_addr=inet_addr(iface_string);
    if(setsockopt(cdp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_request, sizeof(multicast_request)) < 0) {
        std::cout << "Experienced error while setting up multicast." << std::endl;
        return 1;
    }

    // The setup of the socket is now complete, so data can now be pulled from it
    // Declare variables for inside the loop to avoid stack thrashing
    uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
    cdp_header_t * cdp_header = (cdp_header_t *) receive_buffer;
    socklen_t addrlen = sizeof(__ifc_address);
    int bytes_received;
    int ii=0;

    // Loop on reception of packet
    while (1) {
        // Block on receiving a UDP packet
        if ((bytes_received = recvfrom(cdp_socket, receive_buffer, sizeof(receive_buffer), 0, (struct sockaddr *)&__ifc_address, &addrlen)) < 0) {
            std::cout << "Error while receiving a UDP packet" << std::endl;
            return 1;
        }
        // Ensure the CDP Header is valid (that is, that the mark matches the 'magic word' and that the string matches the specified ASCII string)
        if (cdp_header->mark == CDP_MARK_WORD && strcmp((char *)(cdp_header->string), CDP_STRING_WORD)==0) {
            // Loop while data items are available
            ii = 0;
            while (ii < (bytes_received - sizeof(cdp_header_t))) {
                // Gather the individual data items from the CDP packet payload
                cdp_data_item_t * data_item = (cdp_data_item_t *) &cdp_header->payload[ii];

                // At this point, if the line below was not commented out, all data item received's types (in decimal) and sizes would print
                // std::cout << data_item->type << "    " << data_item->size << std::endl;
                
                // For this example, we only care about type 0x0135, which is Position V3, all other types will be ignored
                if (data_item->type == 0x0135) {
                    // Check if sizes match to ensure valid data
                    if (data_item->size == sizeof(cdp_position_v3_t)) {
                        // Pull the data from the data item payload and unpack it according to the Position V3 struct defined earlier
                        cdp_position_v3_t * data = (cdp_position_v3_t *) data_item->payload;
                        // Print the data pulled from the data item (flags and smoothing are ignored in this example for conciseness)
                        // Note the + in front of data->anchor_count. This is because uint8_t (the type for anchor_count) is a typedef for unsigned char
                        //     and the ostream class in C++ has a special overload for unsigned char, causing it to attempt to print a non-printable character
                        std::cout << "Position V3 - Serial Number: " << data->serial_number << ", Network Time: " << data->network_time << ", Coordinates: (" << data->x_in_mm
                                  << ", " << data->y_in_mm << ", " << data->z_in_mm << "), Quality: " << data->quality << ", Anchor Count: " << +data->anchor_count << std::endl;
                    }
                }
                // Move along to the next data item
                ii += sizeof(cdp_data_item_t) + data_item->size;
            }
        }
        // If the CDP Header is not valid, an unknown packet was received.
        else {
            std::cout << "Received a non-CDP packet" << std::endl;
        }
    }
    return 0;
}
