// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include "CompilerConfig.h"
#include "connection.h"
#include "datatypes/position_v3.h"
#include "datatypes/deliver_user_data_v1.h"

/***********************
 * Private Definitions
 ***********************/
// Index of the multicast IP (group) program argument
#define _ARGV_INDEX__GROUP  1
// Index of the UDP port program argument
#define _ARGV_INDEX__PORT   2
// Index of the network interface IP to bind socket to
#define _ARGV_INDEX__IFACE  3
#define _PAYLOAD_DATA_SIZE  8

#define _VERSION_STRING     "1.2.128"

/***********************
 * Private Functions
 ***********************/
static void _ExamplePositionHandler(const cdp_packet_info_t& packet_info, const CdpDataItem& data_item) {
    UNUSED(packet_info);

    if (data_item.GetDataSize() >= sizeof(cdp_data_position_v3_t)) {
        cdp_data_position_v3_t * data = (cdp_data_position_v3_t *) data_item.GetData();
        printf("Position V3 - Serial Number: %u, Network Time: %lu, Coordinates: (%d, %d, %d), Quality: %hu, Anchor Count: %d\n",
               data->serial_number, data->network_time, data->coordinates.x_in_mm, data->coordinates.y_in_mm, data->coordinates.z_in_mm, data->quality, data->anchor_count);
    }
}


/***********************
 * Public Functions
 ***********************/
// Start of program.
int main(int argc,char **argv) {

    if (argc != 4) {
        printf("Usage: %s GROUP PORT IFACE\r\n"
               "   GROUP - IP to listen on (ie: 239.255.76.67)\r\n"
               "   PORT - Port to listen on (ie: 7667)\r\n"
               "    IFACE - IP of network interface to bind socket to (ie: 127.0.0.1)\r\n", argv[0]);
        exit(0);
    }

    printf("Started CDP C++ Example Program v%s\n", _VERSION_STRING);

    CdpConnection* cdp_connection = new CdpConnection(argv[_ARGV_INDEX__GROUP], atoi(argv[_ARGV_INDEX__PORT]), argv[_ARGV_INDEX__IFACE]);

    /***********************
     * Example Receive
     ***********************/
    cdp_connection->Subscribe(CDP_DATATYPE_POSITION_V3, _ExamplePositionHandler);


    /***********************
     * Example Transmit
     ***********************/
    while (true) {
        // Wait so we don't spam the UDP comms.
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Create buffer to store data in.
        uint8_t buffer[sizeof(cdp_data_deliver_user_data_v1_t) + _PAYLOAD_DATA_SIZE];

        // Create user data.
        cdp_data_deliver_user_data_v1_t * user_data_item = (cdp_data_deliver_user_data_v1_t *) buffer;
        user_data_item->serial_number = 0x06020002; // Destination device
        for (int ii=0; ii<_PAYLOAD_DATA_SIZE; ii++) {
            user_data_item->payload[ii] = ii;
        }

        // Create CDP packet, and add user data item to it.
        CdpPacket packet;
        packet.AddDataItem(CDP_DATATYPE_DELIVER_USER_DATA_V1, sizeof(cdp_data_deliver_user_data_v1_t) + _PAYLOAD_DATA_SIZE, user_data_item);

        // Send the data.
        cdp_connection->Publish(packet);
    }

    return 0;
}
