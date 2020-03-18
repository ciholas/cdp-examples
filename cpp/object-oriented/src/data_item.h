// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__DATA_ITEM__H_
#define _UTILS__CDP__DATA_ITEM__H_

#include <cstdint>

#include "CompilerConfig.h"

// Provides filtering capabilities for CDP.
#define MAX_CDP_DATA_ITEM_SIZE  1536

// Defines the CDP Data Item structure
// Contains the header (4 bytes) and the data payload (X bytes).
// Note that each field is transmitted in little endian.
typedef struct PACKED CDP_DATA_ITEM_T {
    uint16_t type;   // The type of the data (2 bytes).
    uint16_t size;   // The size of the data item in bytes (2 bytes).
    uint8_t data[];  // A variable length buffer containing all of the data contained in the data item (X bytes).
} cdp_data_item_t;


// Allows for easy creation and manipulation of CDP data items.
class CdpDataItem {
public:

    /////////////////////
    // Class Functions //
    /////////////////////
    /// Constructs this as an empty data item.
    CdpDataItem(void);

    // Constructs this.
    // The parameters are the data type of the CDP Data Item, the size in bytes of the Data Item's data,
    //     and a pointer to the buffer that contains the data of the data item.
    CdpDataItem(uint16_t type, uint16_t size, void *data);

    // Constructs this.
    // The parameter is a buffer containing a CDP Data Item in little endian format.
    CdpDataItem(const uint8_t* data_item);

    // Copies other into this.
    // The parameter is the other CDP Data Item to be copied.
    CdpDataItem(const CdpDataItem& other);

    // Destroys this.
    ~CdpDataItem(void);

    // Assigns other to this.
    CdpDataItem& operator =(const CdpDataItem& other);


    /////////////////////
    // Const Functions //
    /////////////////////
    // Returns a pointer to the data item's data.
    void* GetData(void) const;

    // Returns the size of the data item in bytes.
    uint16_t GetSize(void) const;

    // Returns the type of the data item.
    uint16_t GetType(void) const;

    // Returns the size of the data item's data.
    uint16_t GetDataSize(void) const;


    ////////////////////////
    // Modifier Functions //
    ////////////////////////
    // Write the CDP Data Item to the buffer.
    // The parameter is the buffer to write the CDP Data Item to.
    // Return the number of bytes written to the buffer.
    uint16_t WriteToBuffer(uint8_t* buffer);

    // Sets the data portion of this object equal to that of data_item.
    void SetData(uint8_t *data_item);

private:

    // The data of the data item.
    cdp_data_item_t* mData = nullptr;

    // Flag indicating whether to delete data.
    bool mDeleteData = false;

    // Allocates and copies data to the data item's data.
    // The parameter is a pointer to the data to be copied.
    void _BuildData(void *data);

    // Copy from other into this object.
    void _CopyItem(const CdpDataItem& other);
};

#endif // _UTILS__CDP__DATA_ITEM__H_
