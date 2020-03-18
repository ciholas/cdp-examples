// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DELIVER__USER__DATA__V1__H_
#define _UTILS__CDP__DELIVER__USER__DATA__V1__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for a Deliver User Data V1 Data Item.
#define CDP_DATATYPE_DELIVER_USER_DATA_V1 (0x010C)

// The Deliver User Data V1 (type 0x010C) data item has a varying total size.
// This data item functions as a format for sending user data to the server to be sent wirelessly to devices on the network.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the device to receive the user data (4 bytes).
    uint8_t payload[];       // The user data to be sent to the specified device (X bytes).
} cdp_data_deliver_user_data_v1_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DELIVER__USER__DATA__V1__H_
