// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include <stdlib.h>
#include "cdp_data_items/position_v3.h"
#include "cdp_data_items/distance_v2.h"
#include "cdp_receiver.h"
#include "utils/print.h"


/***********************
 * Private Definitions
 ***********************/
// Index of the multicast IP (group) program argument
#define _ARGV_INDEX__GROUP  1
// Index of the UDP port program argument
#define _ARGV_INDEX__PORT   2
// Index of the network interface IP to bind socket to
#define _ARGV_INDEX__IFACE  3


/***********************
 * Private Functions
 ***********************/
static void _ExamplePositionHandler(cdp_header_t * header, void * payload, uint16_t size){
    (void) header; // unused

    if(size == sizeof(cdp_position_v3_t)){
        cdp_position_v3_t * data = (cdp_position_v3_t *) payload;
        Print_Line("Position V3 - Serial Number: %u, Network Time: %lu, Coordinates: (%d, %d, %d), Quality: %hu, Anchor Count: %d",
                   data->serial_number, data->network_time, data->x_in_mm, data->y_in_mm, data->z_in_mm, data->quality, data->anchor_count);
    }
}

static void _ExampleDistanceHandler(cdp_header_t * header, void * data, uint16_t size){
    (void) header; //unused

    if(size == sizeof(cdp_distance_v2_t)){
        cdp_distance_v2_t * distance = (cdp_distance_v2_t *) data;
        Print_Line("  Distance %d.%03d",
                   distance->distance / 1000,
                   distance->distance % 1000);
    }
}


/***********************
 * Public Functions
 ***********************/
// Start of program
int main(int argc,char **argv) {

    if(argc != 4){
        Print_ExitOnMessage(
            argc == 1 ? 0 : 1,
            "Usage: %s GROUP PORT IFACE\r\n"
            "   GROUP - IP to listen on (ie: 239.255.76.67)\r\n"
            "    PORT - Port to listen on (ie: 7667)\r\n"
            "    IFACE - IP of network interface to bind socket to (ie: 127.0.0.1)\r\n", argv[0]);
    }

    Print_Line("Started CDP C Example Program");

    /***********************
     * Example Receive
     ***********************/
    // Now register a special data item handler
    CdpReceiver_RegisterDataItemHandler(CDP_DATA_ITEM__POSITION_V3, _ExamplePositionHandler);
    // CdpReceiver_RegisterDataItemHandler(CDP_DATA_ITEM__DISTANCE_V2, _ExampleDistanceHandler);

    // Start receiving (this loops and doesn't return)
    CdpReceiver_Create(argv[_ARGV_INDEX__GROUP], atoi(argv[_ARGV_INDEX__PORT]), argv[_ARGV_INDEX__IFACE]);

    return 0;
}
