// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__QUATERNION__V2__H_
#define _UTILS__CDP__DATATYPES__QUATERNION__V2__H_

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>


// The type for a Quaternion V2 Data Item.
#define CDP_DATATYPE_QUATERNION_V2 (0x013D)

// The Quaternion V2 (type 0x013D) data item has a total size of 29 bytes.
// Reports the quaternion data from an onboard MPU-9250.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes).
    uint64_t network_time;   // The Network Time when the sensor recorded the data (8 bytes).
    int32_t x;               // The X quaternion value (4 bytes).
    int32_t y;               // The Y quaternion value (4 bytes).
    int32_t z;               // The Z quaternion value (4 bytes).
    int32_t w;               // The W quaternion value (4 bytes).
    uint8_t fields;          // Indicates if the data is normalized (1 byte). 2 = Sensor fused.
                             //     1 = Normalized. 0 = Not normalized.
} cdp_data_mpu9250_quaternion_v2_t;

// If we are in C++, add a class definition around the helper function(s) to prevent compiler warnings.
#ifdef __cplusplus

// Provides functionality involving quaternion data from the MPU9250.
class Mpu9250Quaternion {
public:
#endif

// Converts the raw quaternion value from the MPU9250 into a quaternion value.
static double ConvertRawValueToQuaternion(int32_t value) {
    double _rv = value;
    return _rv / pow(2, 30);
}

#ifdef __cplusplus
};
}
#endif

#endif // _UTILS__CDP__DATATYPES__QUATERNION__V2__H_
