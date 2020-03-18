// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__MAGNETOMETER__V2__H_
#define _UTILS__CDP__DATATYPES__MAGNETOMETER__V2__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the Magnetometer V2 Data Item.
#define CDP_DATATYPE_MAGNETOMETER_V2 (0x013B)

// The Magnetometer V2 (type 0x013B) data item has a total size of 26 bytes.
// Reports the magnetometer data from an onboard MPU-9250.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes).
    uint64_t network_time;   // The Network Time when the sensor recorded the data (8 bytes).
    int32_t x;               // The X magnetometer value (4 bytes).
    int32_t y;               // The Y magnetometer value (4 bytes).
    int32_t z;               // The Z magnetometer value (4 bytes).
    uint16_t scale;          // The full-scale representation in microtesla (2 bytes).
} cdp_data_magnetometer_v2_t;

// If we are in C++, add a class definition around the helper function(s) to prevent compiler warnings.
#ifdef __cplusplus

// Provides functionality involving magnetometer data from the MPU9250.
class Mpu9250Magnetometer {
public:
#endif

// Converts the raw magnetometer value from the MPU9250 into milliTeslas.
static double ConvertRawValueToMilliteslas(int16_t raw_value) {
    return (((double)raw_value) * 4.8) / 32767.0;
}

#ifdef __cplusplus
};
}
#endif

#endif // _UTILS__CDP__DATATYPES__MAGNETOMETER__V2__H_
