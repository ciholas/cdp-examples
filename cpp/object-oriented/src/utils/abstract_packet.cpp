// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include "abstract_packet.h"

AbstractPacket::AbstractPacket() {
}

uint8_t* AbstractPacket::GetPacketBuffer() {
    return mPacketBuffer.data();
}

void AbstractPacket::SetPacketBufferSize(size_t size) {
    mPacketBuffer.resize(size);
}

void AbstractPacket::CopyFromPacket(const AbstractPacket *other){
    mPacketBuffer = other->mPacketBuffer;
}
