// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__USER_DEFINED__V2__H_
#define _UTILS__CDP__DATATYPES__USER_DEFINED__V2__H_

#ifdef __cplusplus
extern "C" {
#endif


/// The type for the User Defined V2 Data Item.
#define CDP_DATATYPE_USER_DEFINED_V2 (0x0148)
#define MAX_CDP_USER_DATA_SIZE (255)

// The User Defined V2 (type 0x0148) data item has a varying total size.
// Reports any user-defined data bytes.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;                // The serial number of the device sending the user-defined data (4 bytes).
    char payload[MAX_CDP_USER_DATA_SIZE];  // The format of the contents are defined by the user (X bytes).
} cdp_data_user_defined_v2_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__USER_DEFINED__V2__H_
