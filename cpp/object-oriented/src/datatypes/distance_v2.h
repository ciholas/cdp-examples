// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__DISTANCE__V2__H_
#define _UTILS__CDP__DATATYPES__DISTANCE__V2__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the distance data item.
#define CDP_DATATYPE_DISTANCE_V2 (0x0127)

// The Distance V2 (type 0x0127) data item has a total size of 24 bytes.
// Reports the distance data between two devices.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number_1;  // The serial number of the first device (4 bytes).
    uint32_t serial_number_2;  // The serial number of the second device (4 bytes).
    uint8_t interface_id_1;    // The interface identifier of the first device (1 byte).
    uint8_t interface_id_2;    // The interface identifier of the second device (1 byte).
    uint64_t rx_timestamp;     // Time at which the last packet was received (8 bytes).
    uint32_t distance;         // The distance, in millimeters, between the two devices (4 bytes).
    uint16_t quality;          // The quality of the computed device (2 bytes).
} cdp_data_distance_v2_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__DISTANCE__V2__H_
