// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include "packet.h"

#include <cstring>
#include "data_item.h"

void CdpPacket::_SetSequence(uint32_t sequence) {
    mCdpPacket->header.sequence = sequence;
    mPacketInfo.sequence = sequence;
}

CdpPacket::CdpPacket(uint32_t serial_number) : AbstractPacket() {
    mDataItems.resize(0);

    mPacketBuffer.resize(sizeof(cdp_packet_t));
    mCdpPacket = (cdp_packet_t*)mPacketBuffer.data();
    mCdpPacket->header.mark = CDP_HEADER__MARK;
    memcpy(mCdpPacket->header.string, CDP_HEADER__STRING, 8);

    SetSerialNumber(serial_number);
    _SetSequence(0);
}

CdpPacket::CdpPacket(const CdpPacket& other) : AbstractPacket() {
    mDataItems.resize(0);

    _CopyCdpPacket(other);
}

CdpPacket::~CdpPacket(void) {
    ClearDataItems();
}

CdpPacket& CdpPacket::operator=(const CdpPacket& other) {
    _CopyCdpPacket(other);

    return *this;
}

void CdpPacket::SetBufferSequenceNumber(const uint8_t *buffer, uint32_t sequence_number) {
    cdp_header_t* _header = (cdp_header_t*) buffer;
    _header->sequence = htole32(sequence_number);
}

void CdpPacket::_CopyCdpPacket(const CdpPacket& other) {
    ClearDataItems();

    CopyFromPacket(&other);
    mCdpPacket = (cdp_packet_t*) mPacketBuffer.data();

    this->ConvertPacketBuffer();
}

void CdpPacket::AddDataItem(uint8_t* data_item) {
    mDataItemsMutex.lock();
    CdpDataItem _new_item;
    mDataItems.push_back(_new_item);
    mDataItems.at(mDataItems.size()-1).SetData(data_item);
    mDataItemsMutex.unlock();
}

void CdpPacket::AddDataItem(uint16_t type, uint16_t size, void* data) {
    cdp_data_item_t _item;
    _item.type = htole16(type);
    _item.size = htole16(size);

    uint32_t _curr_idx = this->mPacketBuffer.size();
    this->mPacketBuffer.resize(_curr_idx + size + sizeof(cdp_data_item_t));
    memcpy(mPacketBuffer.data() + _curr_idx, (uint8_t*)&_item, sizeof(cdp_data_item_t));
    memcpy(mPacketBuffer.data() + _curr_idx + sizeof(cdp_data_item_t), (uint8_t*)data, size);

    AddDataItem(mPacketBuffer.data() + _curr_idx);
}

void CdpPacket::ClearDataItems(void) {
    mDataItemsMutex.lock();
    mDataItems.clear();
    mPacketBuffer.resize(sizeof(cdp_packet_t));
    mDataItemsMutex.unlock();
}

void CdpPacket::SetSerialNumber(uint32_t serial_number) {
    mCdpPacket->header.serial_number = serial_number;
    mPacketInfo.serial_number = serial_number;
}

uint8_t* CdpPacket::CreateBuffer() {
    uint8_t* _buffer = new uint8_t[GetPacketSize()];

    mDataItemsMutex.lock();
    memcpy(_buffer, mPacketBuffer.data(), mPacketBuffer.size());
    mDataItemsMutex.unlock();

    return _buffer;
}

void CdpPacket::SetSrcIp(uint32_t ip) {
    mPacketInfo.source_ip_address = ip;
}

void CdpPacket::SetSrcPort(uint16_t port) {
    mPacketInfo.source_ip_port = port;
}

cdp_packet_info_t CdpPacket::GetInfo() const {
    return mPacketInfo;
}

uint8_t* CdpPacket::CreateBufferWithSequenceNumber(uint32_t sequence_number) {
    _SetSequence(sequence_number);

    return CreateBuffer();
}

bool CdpPacket::HasValidHeader() {
    mCdpPacket = (cdp_packet_t*)mPacketBuffer.data();

    if ((mCdpPacket->header.mark == CDP_HEADER__MARK)
            && (memcmp(mCdpPacket->header.string, CDP_HEADER__STRING, 8) == 0)) {
        return true;
    }

    return false;
}

void CdpPacket::ConvertPacketBuffer() {
    uint16_t _index = 0;
    uint16_t _data_items_size = mPacketBuffer.size() - sizeof(cdp_header_t);

    mPacketInfo.serial_number = mCdpPacket->header.serial_number;
    mPacketInfo.sequence = mCdpPacket->header.sequence;

    while (_index < _data_items_size) {

        AddDataItem(mCdpPacket->data_items + _index);

        _index += mDataItems.at(mDataItems.size()-1).GetSize();
    }
}

const std::vector<CdpDataItem>* CdpPacket::GetDataItemsReference(void) const {
    return &mDataItems;
}

std::vector<CdpDataItem> CdpPacket::GetDataItems(void) const {
    return mDataItems;
}

uint16_t CdpPacket::GetPacketSize(void) const {
    uint16_t _size;

    mDataItemsMutex.lock();
    _size = mPacketBuffer.size();
    mDataItemsMutex.unlock();

    return _size;
}
