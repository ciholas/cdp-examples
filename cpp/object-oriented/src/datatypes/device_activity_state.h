// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__DEVICE_ACTIVITY_STATE__H_
#define _UTILS__CDP__DATATYPES__DEVICE_ACTIVITY_STATE__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CompilerConfig.h"
#include "utils/coordinates_data.h"


// The type for the Device Activity State Data Item.
#define CDP_DATATYPE_DEVICE_ACTIVITY_STATE (0x0137)

enum device_connectivity_state_t {
    DEVICE_CONNECTIVITY__ETHERNET,
    DEVICE_CONNECTIVITY__UWB
};

enum device_synchronization_state_t {
    DEVICE_SYNCHRONIZATION_STATE__TRANSMIT_SYNCHRONIZED,
    DEVICE_SYNCHRONIZATION_STATE__RECEIVE_SYNCHRONIZED
};

// The Device Activity State (type 0x0137) data item has a total size of 20 bytes.
// Reports the current state of a device on the network.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;         // The serial number of the infrastructure node (4 bytes).
    uint8_t interface_id;           // The infrastructure node's interface identifier (1 byte).
    coordinates_t coordinates;      // The coordinates from the origin, in mm, for the last known position of the device (12 bytes)(4 bytes each).
    uint8_t role_id;                // The identifier for the role this device is current functioning as (1 byte).
    uint8_t connectivity_state;     // Specifies Ethernet or UWB connectivity (1 byte).
    uint8_t synchronization_state;  // Specifies TX and RX sync status (1 byte).
} cdp_data_infrastructure_v4_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__DEVICE_ACTIVITY_STATE__H_
