// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__ANCHOR_HEALTH__V5__H_
#define _UTILS__CDP__DATATYPES__ANCHOR_HEALTH__V5__H_

#ifdef __cplusplus
extern "C" {
#endif


// The type for the Anchor Health V5 Data Item.
#define CDP_DATATYPE_ANCHOR_HEALTH_V5 (0x014A)

enum anchor_health_error_code_t {
    ANCHOR_HEALTH__NO_ERROR,
    ANCHOR_HEALTH__ADD_BLACKLIST_PAIRS,
    ANCHOR_HEALTH__BAD_SURVEY,
    ANCHOR_HEALTH__UNKNOWN,
};

// This structure specifies the full identifier for a device.
typdef struct PACKED {
    uint32_t serial_number;        // The serial number of the device (4 bytes).
    uint8_t interface_identifier;  // The interface identifier of the device (1 byte).
} full_device_id_t;

// The Anchor Health V5 (type 0x014A) data item has a varying total size.
// Reports the health of anchors in the network.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;                 // The serial number of the anchor (4 bytes).
    uint8_t interface_identifier;           // The interface identifier of the anchor (1 byte).
    uint32_t ticks_reported;                // The total quantity of anchor Ticks that were reported by the anchor since the last Anchor Health Information (4 bytes).
    uint32_t tim_rxs_reported;              // The total quantity of Times Rxs that were reported by the anchor since the last Anchor Health Information (4 bytes).
    uint32_t beacons_reported;              // Total quantity of tag beacons that were reported by the anchor since the last Anchor Health Information packet (4 bytes).
    uint32_t beacons_discarded;             // Total quantity of tag beacons that were discarded from the anchor since the last Anchor Health Information (4 bytes).
    uint16_t average_quality;               // The average of the quality number from 0 to 10,000, with 0 being poor quality,
                                            //     10,000 being high quality for the anchor since the last Anchor Health Information (2 bytes).
    uint8_t report_period;                  // Period of the packet in seconds (1 byte).
    uint8_t interanchor_comms_error_code;   // Specifies type of comms error between anchors (1 byte). 0 = No error. 1 = Blacklisting. 2 = Bad survey. 3 = Unknown.
    full_device_id_t bad_paired_anchors[];  // An array of all the anchors that this anchor has a bad connection to which are causing the error code (X bytes).
} cdp_data_anchor_health_v5_t;

#ifdef __cplusplus
}
#endif

#endif  // _UTILS__CDP__DATATYPES__ANCHOR_HEALTH__V5__H_
