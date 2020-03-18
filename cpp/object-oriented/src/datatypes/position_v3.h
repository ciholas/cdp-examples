// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__POSITION__V3__H_
#define _UTILS__CDP__DATATYPES__POSITION__V3__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "utils/coordinates_data.h"


// The type for a Position V3 Data Item.
#define CDP_DATATYPE_POSITION_V3 (0x0135)

// The Position V3 (type 0x0135) data item has a total size of 30 bytes.
// It reports the 3-dimensional position of the reporting device.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;     // The serial number of the reporting device (4 bytes).
    uint64_t network_time;      // The Network Time when the sensor recorded the data (8 bytes).
    coordinates_t coordinates;  // The coordinates in mm from the origin (12 bytes).
    uint16_t quality;           // The quality of the computed position (2 bytes).
    uint8_t anchor_count;       // The number of anchors involved in the calculation of this position (1 byte).
    uint8_t flags;              // 1 bit = inactive mode. 1 bit = was not calculated. 6 bits = unused. (1 byte).
    uint16_t smoothing;         // The effective smoothing factor (2 bytes).
} cdp_data_position_v3_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__POSITION__V3__H_
