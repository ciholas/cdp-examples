// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__GYROSCOPE__V2__H_
#define _UTILS__CDP__DATATYPES__GYROSCOPE__V2__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the Gyroscope V2 Data Item.
#define CDP_DATATYPE_GYROSCOPE_V2 (0x013A)

// The Gyroscope V2 (type 0x013A) data item has a total size of 26 bytes.
// Reports the gyroscope data from an onboard MPU-9250.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes).
    uint64_t network_time;   // The Network Time when the sensor recorded the data (8 bytes).
    int32_t x;               // The X gyroscope value (4 bytes).
    int32_t y;               // The Y gyroscope value (4 bytes).
    int32_t z;               // The Z gyroscope value (4 bytes).
    uint16_t scale;          // The full-scale representation in Degrees Per Second (2 bytes).
} cdp_data_gyroscope_v2_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__GYROSCOPE__V2__H_
