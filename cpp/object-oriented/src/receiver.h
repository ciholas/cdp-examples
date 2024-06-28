// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__CDP__RECEIVER__H_
#define _UTILS__CDP__RECEIVER__H_

#include <arpa/inet.h>
#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <thread>
#include "packet.h"

class CdpPacket;

/// Called when a CDP packet is received matching a specifications given by the subscriber.
typedef std::function<void(const cdp_packet_info_t& packet_info, const CdpDataItem& data_item)> cdp_callback_t;

// Handles all processes involved with receiving a CDP packet.
class CdpReceiver {
public:

    /////////////////////
    // Class Functions //
    /////////////////////
    // Constructs this.
    explicit CdpReceiver(char * group_string, uint16_t port, char * iface_string);

    // Destroys this.
    ~CdpReceiver(void);


    ////////////////////////
    // Modifier Functions //
    ////////////////////////
    // Stores a callback that will be run when a CDP packet with type matching the specified type is received.
    // The parameters are the CDP Data Item that the subscriber is expecting and the callback function.
    uint32_t Subscribe(uint16_t type, cdp_callback_t callback);

    // Deletes a callback subscription.
    void Unsubscribe(uint32_t id);

    // Deletes all stored subscriptions.
    void ClearSubscriptions(void);

    // Sets up the UDP socket to process CDP packets.
    void Run(void);

    // Closes the UDP socket and kills the receiving thread.
    void Shutdown(void);

private:
  
    // A mutex protecting the use of #__udp_socket.
    std::mutex __udp_socket_mutex;

    // A mutex protecting modification of #__subscriptions.
    std::mutex __subscriptions_mutex;

    // A hash that maps CDP types to callbacks that desire CDP data items matching that type.
    std::map<uint16_t, std::deque<std::pair<uint16_t, cdp_callback_t>>* > __subscriptions;

    // The socket on which to receive CDP packets.
    int __cdp_socket;

    // The socket interface on which we will be receiving CDP packets.
    struct sockaddr_in __ifc_address;

    // Stores if this is shutting down the UDP socket.
    bool __shutting_down;
    uint16_t __id_counter;

    std::thread             __subscriptions_thread;
    std::mutex              __packet_queue_mutex;
    std::condition_variable __packet_queue_var;
    std::deque<CdpPacket>   __packet_queue;

    // Runs all callbacks mapped to type.
    // The parameters are the serial number from the CDP Header, the sequence number from the CDP Header, and the CDP Data Item.
    void _AlertSubscribers(const cdp_packet_info_t& packet_info, const CdpDataItem& data_item);

    // Processes a CDP Packet to determine which callbacks should be run.
    // The parameter is the CDP Packet to process.
    void _ProcessPacket(const CdpPacket& packet);

    // Continually checks on whether we've received any new packets, and when we do, it calls _ProcessPacket on them.
    void _ProcessPackets();

    // Processes the pending UDP packets received.
    void _ProcessPendingRxDatagrams();
};

#endif // _UTILS__CDP__RECEIVER__H_
