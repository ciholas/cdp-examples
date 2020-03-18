// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include "data_item.h"

#include <cstdlib>
#include <cstring>

CdpDataItem::CdpDataItem(void) {
    mData = nullptr;
}

CdpDataItem::CdpDataItem(const uint8_t* data_item) {
    mData = (cdp_data_item_t*)data_item;
    mDeleteData = false;
}

void CdpDataItem::_CopyItem(const CdpDataItem& other) {
    if (other.mData != nullptr && other.GetSize() > 0) {
        mDeleteData = true;
        mData = (cdp_data_item_t*)malloc(other.GetSize());
        memcpy(mData, other.mData, other.GetSize());
    }
}

CdpDataItem::CdpDataItem(const CdpDataItem& other) {
    mData = nullptr;
    _CopyItem(other);
}

void CdpDataItem::SetData(uint8_t* data_item) {
    if (mDeleteData && mData != nullptr) {
        free(mData);
    }
    mData = (cdp_data_item_t*)data_item;
    mDeleteData = false;
}

CdpDataItem::~CdpDataItem(void) {
    if (mDeleteData && mData != nullptr) {
        free(mData);
    }
}

CdpDataItem& CdpDataItem::operator =(const CdpDataItem &other) {
    mData = nullptr;
    _CopyItem(other);
    return *this;
}

uint16_t CdpDataItem::WriteToBuffer(uint8_t* buffer) {
    memcpy(buffer, (uint8_t*)mData, GetSize());

    return GetSize();
}

void* CdpDataItem::GetData(void) const {
    return (void*)(mData->data);
}

uint16_t CdpDataItem::GetDataSize(void) const {
    if (mData == nullptr) return 0;
    return mData->size;
}

uint16_t CdpDataItem::GetSize(void) const {
    if (mData == nullptr) return 0;
    return mData->size + sizeof(cdp_data_item_t);
}

uint16_t CdpDataItem::GetType(void) const {
    return mData->type;
}
