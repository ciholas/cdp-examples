// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include "receiver.h"

#include <string.h>
#include <unistd.h>
#include "data_item.h"
#include "packet.h"

/// Maximum sized UDP packet received
#define _RECEIVE_BUFFER_SIZE 65536


CdpReceiver::CdpReceiver(char * group_string, uint16_t port) {
    __shutting_down = false;
    __id_counter = 0;

    // Create a socket.
    if ((__cdp_socket = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        printf("Error creating receive socket!\n");
    }

    // Allow other apps to use the same port/ifc.
    static int so_reuse = 1;
    if (setsockopt(__cdp_socket, SOL_SOCKET, SO_REUSEADDR, &so_reuse, sizeof(so_reuse)) < 0) {
        printf("Error with setsockopt(SO_REUSEADDR)!\n");
    }

    // Setup receive ip address.
    memset(&__ifc_address, 0, sizeof(__ifc_address));
    __ifc_address.sin_family=AF_INET;
    __ifc_address.sin_addr.s_addr = inet_addr(group_string);

    // Set the port.
    __ifc_address.sin_port=htons(port);

    // Bind.
    if (bind(__cdp_socket, (struct sockaddr *)&__ifc_address, sizeof(__ifc_address)) < 0) {
        printf("Error binding to receive socket!\n");
    }

    // Setup the multicast for the GROUP.
    struct ip_mreq multicast_request;
    multicast_request.imr_multiaddr.s_addr=inet_addr(group_string);
    multicast_request.imr_interface.s_addr=htonl(INADDR_ANY);
    if (setsockopt(__cdp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_request, sizeof(multicast_request)) < 0) {
        printf("Error with setsockopt for multicast!\n");
    }
}

CdpReceiver::~CdpReceiver() {
}

void CdpReceiver::Shutdown(void) {
    __shutting_down = true;

    __packet_queue_mutex.lock();
    __packet_queue.clear();
    __packet_queue_mutex.unlock();
    __packet_queue_var.notify_all();

    __udp_socket_mutex.lock();
    close(__cdp_socket);
    __udp_socket_mutex.unlock();

    if (__subscriptions_thread.joinable()) {
        __subscriptions_thread.join();
    }
}

uint32_t CdpReceiver::Subscribe(uint16_t type, cdp_callback_t callback) {
    __subscriptions_mutex.lock();
    std::deque<std::pair<uint16_t, cdp_callback_t>>* _callbacks = __subscriptions[type];

    if (_callbacks == NULL){
        _callbacks = new std::deque<std::pair<uint16_t, cdp_callback_t>>();
        __subscriptions[type] = _callbacks;
    }

    uint32_t _id = type;
    __id_counter += 1;
    _callbacks->push_back(std::make_pair(__id_counter, callback));
    _id = (_id << 16) | __id_counter;

    __subscriptions_mutex.unlock();
    return _id;
}

void CdpReceiver::Unsubscribe(uint32_t id) {
    uint16_t type = (id >> 16) & 0xffff;

    std::deque<std::pair<uint16_t, cdp_callback_t>>* _callbacks = __subscriptions[type];

    if (_callbacks != NULL) {
        uint16_t callbackId = id & 0xffff;

        for (uint16_t index = 0; index < _callbacks->size(); index++) {
            if (_callbacks->at(index).first == callbackId) {
                _callbacks->erase(_callbacks->begin()+index);
                return;
            }
        }
    }
}

void CdpReceiver::ClearSubscriptions() {
    __subscriptions_mutex.lock();

    for (auto it = __subscriptions.begin(); it != __subscriptions.end(); ) {
        it = __subscriptions.erase(it);
    }

    __subscriptions.clear();

    __subscriptions_mutex.unlock();
}

void CdpReceiver::_ProcessPackets() {
    std::unique_lock<std::mutex> _lock(__packet_queue_mutex);
    std::deque<CdpPacket> _pkts;

    while (!__shutting_down) {
        // Entering this function unlocks the mutex; when notified, the mutex becomes locked.
        __packet_queue_var.wait(_lock);

        do {
            // Swap the buffers between the vectors
            if (__packet_queue.size() > 0) {
                _pkts.swap(__packet_queue);
            }
            // Allow the socket thread access to the queue again.
            _lock.unlock();

            // Process all the cdp packets
            for (size_t _i = 0; _i < _pkts.size(); _i++) {
                _ProcessPacket(_pkts.at(_i));
            }

            _pkts.clear();
            _lock.lock();
        } while (__packet_queue.size() > 0);
    }
}

void CdpReceiver::Run(void) {
    __subscriptions_thread = std::thread(std::bind(&CdpReceiver::_ProcessPackets, this));
    __subscriptions_thread.detach();

    _ProcessPendingRxDatagrams();
}

void CdpReceiver::_ProcessPacket(const CdpPacket& packet) {
    const std::vector<CdpDataItem>* _data_items = packet.GetDataItemsReference();
    const cdp_packet_info_t _packet_info = packet.GetInfo();

    for (size_t _i = 0; _i < _data_items->size(); _i++) {
        _AlertSubscribers(_packet_info, _data_items->at(_i));
    }
}

void CdpReceiver::_ProcessPendingRxDatagrams(void) {
    uint8_t receive_buffer[_RECEIVE_BUFFER_SIZE];
    socklen_t addrlen = sizeof(__ifc_address);
    int bytes_received;

    // Loop on reception of packet.
    __udp_socket_mutex.lock();
    while (!__shutting_down) {

        // Block on receiving a UDP packet.
        if ((bytes_received = recvfrom(__cdp_socket, receive_buffer, sizeof(receive_buffer), 0, (struct sockaddr *)&__ifc_address, &addrlen)) < 0) {
            printf("Error receiving datagram!\n");
        }

        // Copy data into cdp packet structure.
        CdpPacket packet;
        packet.SetPacketBufferSize(bytes_received);
        memcpy(packet.GetPacketBuffer(), receive_buffer, bytes_received);

        if (packet.HasValidHeader()) {
            packet.ConvertPacketBuffer();

            __packet_queue_mutex.lock();
            __packet_queue.push_back(packet);
            __packet_queue_mutex.unlock();
            __packet_queue_var.notify_all();
        }
    }
    __udp_socket_mutex.unlock();
}

void CdpReceiver::_AlertSubscribers(const cdp_packet_info_t& packet_info, const CdpDataItem& data_item) {
    __subscriptions_mutex.lock();
    std::deque<std::pair<uint16_t, cdp_callback_t>>* _callbacks = __subscriptions[data_item.GetType()];

    // Nothing is subscribed to this type.
    if (_callbacks == NULL) {
        __subscriptions_mutex.unlock();
        return;
    }

    for (int _i = 0; _i < _callbacks->size(); _i++) {
        _callbacks->at(_i).second(packet_info, data_item);
    }
    __subscriptions_mutex.unlock();
}
