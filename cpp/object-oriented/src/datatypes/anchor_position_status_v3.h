// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__ANCHOR_POSITION_STATUS__V3__H_
#define _UTILS__CDP__DATATYPES__ANCHOR_POSITION_STATUS__V3__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the Anchor Position V3 Data Item
#define CDP_DATATYPE_ANCHOR_POSITION_STATUS_V3 (0x0136)

enum ANCHOR_POSITION_STATUS__QUALITY_BOUNDARIES {
    ANCHOR_POSITION_STATUS__MIN_QUALITY = 0,      // The worst possible quality.
    ANCHOR_POSITION_STATUS__MAX_QUALITY = 10000,  // The best possible quality.
};

enum ANCHOR_POSITION_STATUS__VALUES {
    ANCHOR_POSITION_STATUS__GOOD,                // Anchor data is good.
    ANCHOR_POSITION_STATUS__UNKNOWN_ANCHOR,      // The anchor is unknown on the network.
    ANCHOR_POSITION_STATUS__DIFF_OTHER_ANCHORS,  // Anchor data does not match the data from other anchors.
    ANCHOR_POSITION_STATUS__DIFF_PREV,           // Anchor data is inconsistent with the previous data about the location of the reporting device.
    ANCHOR_POSITION_STATUS__NO_NT_LOCK,          // Anchor data has No Network Time Lock.
};

typedef struct PACKED {
    uint32_t serial_number;               // The serial number of the anchor (4 bytes).
    uint8_t anchor_interface_identifier;  // The interface identifier of the anchor (1 byte).
    uint8_t status;                       // Status of the anchor data (1 byte).
    int16_t first_path;                   // The first path signal quality in millibels (2 bytes).
    int16_t total_path;                   // The total path signal quality in millibels (2 bytes).
    uint16_t quality;                     // A number from 0 to 10,000 representing quality with 0 being poor quality
                                          //     and 10,000 being high quality (2 bytes).
} position_anchor_status_t;

// The Anchor Position Status V3 (type 0x0136) data item has a varying total size.
// Reports the status of an anchor that attempted to provide location data about the reporting device.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;                          // The infrastructure node's serial number (4 bytes).
    uint64_T network_time;                           // The Network Time of the position (8 bytes).
    position_anchor_status_t anchor_status_array[];  // An array of Anchor Status Structures (X bytes).
} cdp_data_anchor_position_status_v2_t;

#ifdef __cplusplus
}
#endif

#endif  // _UTILS__CDP__DATATYPES__ANCHOR_POSITION_STATUS__V3__H_
