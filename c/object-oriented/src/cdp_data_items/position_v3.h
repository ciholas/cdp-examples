// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _CDP_DATA_ITEM__POSITION_V3__H_
#define _CDP_DATA_ITEM__POSITION_V3__H_
#include <stdint.h>

/***********************
 * Public Definitions
 ***********************/
 // The type for a Position V3 Data Item.
#define CDP_DATA_ITEM__POSITION_V3 0x0135


/***********************
 * Public Types
 ***********************/
 // the Position V3 (type 0x0135) data item has a total size of 30 bytes.
 // It reports the 3-dimensional position of the reporting device.
 // Note that each fiel is transmitted in little endian.
typedef struct __attribute__((__packed__)) CDP_POSITION_V3_T {
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes).
    uint64_t network_time;   // The timestamp when the sensor recorded the data (8 bytes).
    uint32_t x_in_mm;        // The x-coordinate from the origin (4 bytes).
    uint32_t y_in_mm;        // The y-coordinate from the origin (4 bytes).
    uint32_t z_in_mm;        // The z-coordinate from the origin (4 bytes).
    uint16_t quality;        // The quality of the computed position (2 bytes).
    uint8_t anchor_count;    // The number of anchors involved in the calculation of this position (1 byte).
    uint8_t flags;           // 1 bit = inactive mode. 1 bit = was not calculated. 6 bits = unused. (1 byte).
    uint16_t smoothing;      // The effective smoothing factor (2 bytes).
} cdp_position_v3_t;

#endif  // _CDP_DATA_ITEM__POSITION_V3__H_
