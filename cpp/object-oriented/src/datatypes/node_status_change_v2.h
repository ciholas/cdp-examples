// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATATYPES__NODE_STATUS_CHANGE__V2__H_
#define _UTILS__CDP__DATATYPES__NODE_STATUS_CHANGE__V2__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CompilerConfig.h"


// The type for the Node Status Change V2 Data Item.
#define CDP_DATATYPE_NODE_STATUS_CHANGE_V2 (0x010D)

// The node is currently inactive on the network.
#define NODE_STATUS__INACTIVE (0x01)
// The node is currently active on the network.
#define NODE_STATUS__ACTIVE   (0x02)

// The Node Status Change V2 (type 0x010D) data item has a total size of 6 bytes.
// Reports when the status for a node has changed.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
    uint32_t serial_number;  // The serial number of the node (4 bytes);
    uint8_t interface_id;    // The interface identifier of the node (1 byte).
    uint8_t node_status;     // The status of the node (1 byte).
} cdp_data_node_status_change_v2_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__CDP__DATATYPES__NODE_STATUS_CHANGE__V2__H_
