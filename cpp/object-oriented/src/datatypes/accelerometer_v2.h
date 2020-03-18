// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__MPU9250_ACCELEROMETER__V2__H_
#define _UTILS__CDP__DATATYPES__MPU9250_ACCELEROMETER__V2__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the Accelerometer V2 Data Item.
#define CDP_DATATYPE_ACCELEROMETER_V2 (0x0139)

// The Accelerometer V2 (type 0x0139) data item has a total size of 25 bytes.
// Reports the accelerometer data from an onboard MPU-9250.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes).
    uint64_t network_time;   // The Network Time when the sensor recorded the data (8 bytes).
    int32_t x;               // The X accelerometer value (4 bytes).
    int32_t y;               // The Y accelerometer value (4 bytes).
    int32_t z;               // The Z accelerometer value (4 bytes).
    uint8_t scale;           // The full-scale representation in Gs (1 byte).
} cdp_data_accelerometer_v2_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__ACCELEROMETER__V2__H_
