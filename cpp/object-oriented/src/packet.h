// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__PACKET__H_
#define _UTILS__CDP__PACKET__H_

#include "utils/abstract_packet.h"
#include "CompilerConfig.h"

#include <mutex>

#include "data_item.h"


// The 4 byte magic word for LCM.
// Note that this is in little endian.
#define CDP_HEADER__MARK        (0x3230434c)
// The string that identifies the LCM channel for CDP.
#define CDP_HEADER__STRING      ("CDP0002")
// The length of the header string.
#define CDP_HEADER__STRING_LEN  (8)

// The start of every CDP Packet has a header with a total size of 20 bytes.
// Note that, unless otherwise specified, each field is transmitted in little endian.
typedef struct PACKED CDP_HEADER_T {
    uint32_t mark;                        // The 4 byte magic word for LCM (0x3230434C in little endian) (4 bytes).
                                          //     All CDP Packets must start with this.
    uint32_t sequence;                    // The sequence number of the CDP packet (4 bytes).
                                          //     This is incremented on every transmission from a given CDP Connection
    char string[CDP_HEADER__STRING_LEN];  // An ASCII string of "CDP0002" with a null terminator (8 bytes).
                                          //     Note that this is not byte swapped.
    uint32_t serial_number;               // The serial number of the UWB device reporting the CDP packet (4 bytes).
                                          //     Note that if the reporting device is unknown, 0 is used.
} cdp_header_t;

// The format of a full CDP packet.
// Note that this is transmitted in little endian.
typedef struct PACKED CDP_PACKET_T {
    cdp_header_t header;   // The CDP Header (20 bytes).
    uint8_t data_items[];  // A variable length buffer containing all of the data items contained in the packet (X bytes).
} cdp_packet_t;

// Additional useful information about a CDP packet that is not contained its Data Items.
typedef struct CDP_PACKET_INFO_T {
    uint32_t serial_number;      // The serial number of the device reporting the packet.
    uint32_t sequence;           // The sequence number of the CDP packet.
    uint32_t source_ip_address;  // The source IP address of the CDP packet.
    uint16_t source_ip_port;     // The source IP port of the CDP packet.
} cdp_packet_info_t;

// Allows for easy creation and manipulation of CDP packets.
class CdpPacket : public AbstractPacket {
public:

    /////////////////////
    // Class Functions //
    /////////////////////
    // Constructs a CdpPacket with a serial number.
    // The parameter is the serial number of the UWB device reporting the packet.
    CdpPacket(uint32_t serial_number = 0);

    // Copies the given CdpPacket into this one.
    // The parameter is the CdpPacket to be copied.
    CdpPacket(const CdpPacket& other);

    // Destroys the given CdpPacket.
    ~CdpPacket(void);

    // Sets this CdpPacket to be equal to the given CdpPacket.
    // The parameter is the CdpPacket that this one should equal.
    // Returns a reference to this CdpPacket.
    CdpPacket& operator=(const CdpPacket& other);


    /////////////////////
    // Const Functions //
    /////////////////////
    // Returns a vector of the CdpDataItems in the CdpPacket.
    std::vector<CdpDataItem> GetDataItems(void) const;
     // Returns the size, in bytes, of the CdpPacket.
    uint16_t GetPacketSize(void) const;

    // Checks if this CdpPacket has a valid header.
    // Returns the validity of the header.
    // Note that this function modifies the mCdpPacket field.
    bool HasValidHeader();

    static void SetBufferSequenceNumber(const uint8_t* buffer, uint32_t sequence_number);


    ////////////////////////
    // Modifier Functions //
    ////////////////////////
    // Appends a CDP data item to the packet's data items.
    // The parameter is the CDP Data Item to add.
    void AddDataItem(uint8_t * data_item);

    // Constructs a CDP Data Item and appends it to the packet's data items.
    // The parameters are the CDP type of the data item to be added, the size, in bytes, of the data,
    //     and a pointer to the data that makes up the data item to be added.
    void AddDataItem(uint16_t type, uint16_t size, void *data);

    // Deletes the CdpPacket's data items.
    void ClearDataItems(void);

    // Sets the CdpPacket's serial number.
    // The parameter is the serial number of the CDP Packet.
    void SetSerialNumber(uint32_t serial_number);

    // Allocates and writes a buffer that contains the full CdpPacket.
    // The parameter is the sequence number of the CdpPacket.
    uint8_t* CreateBufferWithSequenceNumber(uint32_t sequence_number);

    // Converts the data in this object's packet buffer member variable into data stored in the CDP data items and CdpPacket header info member variables.
    void ConvertPacketBuffer();

    // Allocates and writes a buffer without specifying the sequence number.
    uint8_t *CreateBuffer();

    // Sets the source IP of this CdpPacket.
    void SetSrcIp(uint32_t ip);

    // Sets the source port of this CdpPacket.
    void SetSrcPort(uint16_t port);

    // Gets the info for this CdpPacket.
    cdp_packet_info_t GetInfo() const;

    // Returns a reference to the list of data items in this CdpPacket.
    const std::vector<CdpDataItem>* GetDataItemsReference() const;

private:

    // Pointer to the packet structure itself.
    cdp_packet_t* mCdpPacket = NULL;
    // The data items of the packet.
    std::vector<CdpDataItem> mDataItems = std::vector<CdpDataItem>(56);
    // A mutex to protect the modification of the packet's data items.
    mutable std::mutex mDataItemsMutex;

    // A struct containing the serial number, sequence number, and network info of this packet.
    cdp_packet_info_t mPacketInfo;

    // Copies other to this.
    // The parameter is the other CdpPacket to be copied.
    void _CopyCdpPacket(const CdpPacket& other);

    // Sets the sequence number for this CdpPacket.
    void _SetSequence(uint32_t sequence);
};

#endif // _UTILS__CDP__PACKET__H_
