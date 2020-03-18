// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__TEMPERATURE__V2__H_
#define _UTILS__CDP__DATATYPES__TEMPERATURE__V2__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the Temperature V2 Data Item.
#define CDP_DATATYPE_TEMPERATURE_V2 (0x013E)

// The Temperature V2 (type 0x013E) data item has a total size of 16 bytes.
// Reports the temperature measured by an onboard LPS25H.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes).
    uint64_t network_time;   // The Network Time when the sensor recorded the data (8 bytes).
    int16_t temperature;     // The temperature value (2 bytes).
    uint16_t scale;          // The full-scale representation in degrees Celsius (2 bytes).
} cdp_data_temperature_v2_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__TEMPERATURE__H_
