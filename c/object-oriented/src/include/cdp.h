// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _CDP__H_
#define _CDP__H_
#include <stdint.h>


/***********************
 * Public Definitions
 ***********************/
// The expected value of the CDP Header MARK field.
#define CDP_MARK_WORD 0x3230434C
// The expected value of the CDP Header STRING field.
#define CDP_STRING_WORD "CDP0002"


/***********************
 * Public Types
 ***********************/
// CDP Header container type with a total size of 20 bytes.
// All values are in little endian unless specified otherwise
typedef struct __attribute__((__packed__)) CDP_HEADER_T {
    uint32_t mark;           // The magic word (0x3230434c in little endian) (4 bytes).
    uint32_t sequence;       // Sequence number of the CDP packet, increments on each transmission from a source (4 bytes).
    uint8_t  string[8];      // An ASCII string of "CDP0002" with a null terminator (4 bytes).
    uint32_t serial_number;  // The serial number of the reporting device (4 bytes). 0 if unknown.
    uint8_t  payload[];      // One or more data items (X bytes).
} cdp_header_t;

// CDP Data Header container type with a total size of 4 bytes.
// Note that all values are in little endian.
typedef struct __attribute__((__packed__)) CDP_DATA_ITEM_T {
    uint16_t type;      // The types of the CDP Data Item (2 bytes).
    uint16_t size;      // The size of the CDP Data Item not including the Data Item Header (2 bytes).
    uint8_t  payload[]; // The content of the Data Item (X bytes).
} cdp_data_item_t;

#endif  // _CDP__H_
