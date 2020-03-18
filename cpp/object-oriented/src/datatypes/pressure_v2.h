// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__LPS25H_PRESSURE__V2__H_
#define _UTILS__CDP__DATATYPES__LPS25H_PRESSURE__V2__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the Pressure V2 Data Item.
#define CDP_DATATYPE_LPS25H_PRESSURE (0x013C)

// The Pressure V2 (type 0x013C) data item has a total size of 20 bytes.
// Reports the pressure measured by an onboard LPS25H.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes).
    uint64_t network_time;   // The Network Time when the sensor recorded the data (8 bytes).
    int32_t pressure;        // The pressure value (4 bytes).
    uint32_t scale;          // The full-scale representation in millibar (4 bytes).
} cdp_data_lps25h_pressure_v2_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__LPS25H_PRESSURE__V2__H_
