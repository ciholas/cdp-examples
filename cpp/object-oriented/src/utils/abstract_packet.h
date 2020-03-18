// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef __UTILS__ABSTRACT_PACKET__H__
#define __UTILS__ABSTRACT_PACKET__H__

#include <cstddef>
#include <cstdint>
#include <vector>

#define DEFAULT_PACKET_BUFF_SIZE   (1536)
#define DEFAULT_PACKET_BUFF_VALUE  (0)

class AbstractPacket {
    public:
        AbstractPacket();
        uint8_t* GetPacketBuffer();
        void SetPacketBufferSize(size_t size);

    protected:
        std::vector<uint8_t> mPacketBuffer = std::vector<uint8_t>(DEFAULT_PACKET_BUFF_SIZE, DEFAULT_PACKET_BUFF_VALUE);
        void CopyFromPacket(const AbstractPacket* other);
};

#endif // __UTILS__ABSTRACT_PACKET__H__
