// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _CDP_RECEIVER__H_
#define _CDP_RECEIVER__H_
#include "cdp.h"
#include <stdint.h>


/***********************
 * Public Types
 ***********************/
// Data Item Handler
typedef void (*cdp_data_item_handler_t)(cdp_header_t *, void *, uint16_t);


/***********************
 * Public Functions
 ***********************/
// Sets up the socket and receives data.
void CdpReceiver_Create(char * group_string, uint16_t port, char * iface_string);

// Add a specific handler for a data item.
void CdpReceiver_RegisterDataItemHandler(uint16_t type, cdp_data_item_handler_t handler);
#endif
