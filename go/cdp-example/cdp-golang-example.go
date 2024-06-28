// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0

package main

import (
    "os"
    "fmt"
    "bytes"
    "net"
    "strings"
    "golang.org/x/net/ipv4"
    "encoding/binary"
    "unsafe"
)

const (
    // Maximum size of any UDP packet received
    RECEIVE_BUFFER_SIZE = 65535
    // The expected value of the CDP Header STRING field
    CDP_STRING_WORD = "CDP0002\x00"
    // The expected value of the CDP Header MARK field (equivalent to 0x3230434c)
    CDP_MARK_WORD = 842023756
    POSITION_V3_TYPE = 0x0135
)
// The CDP Packet Header has a total size of 20 bytes
// CDP Header container type
type CDP_HEADER struct {
    mark          uint32   // Mark: The magic word '0x3230434C' in little-endian (842023756 in decimal) (4 bytes)
    sequence      uint32   // Sequence: The sequence number of the CDP packet (4 bytes)
    word          [8]byte  // Word: The ASCII string 'CDP0002\0' (8 bytes)
    serial_number uint32   // Serial number: Unique identifier of the reporting device (4 bytes)
}
// The Data Item Header has a total size of 4 bytes
// CDP Data Header container type
type CDP_DATA_ITEM struct {
    di_type uint16  // DI Type: The type of the CDP Data Item (2 bytes)
    size    uint16  // Size: The size of the Data Item, not including the Data Item Header (2 bytes)
}
// The Position v3 (type 0x0135) data item has a total size of 30 bytes
// Position v3 Data Item container type
type CDP_POSITION_V3 struct {
    serial_number uint32  // Serial Number: The serial number of the reporting device (4 bytes)
    network_time  uint64  // Network Time: The timestamp when the sensor recorded the data (8 bytes)
    x_in_mm       int32   // X- Coordinate: The x-coordinate from the origin (4 bytes)
    y_in_mm       int32   // Y-Coordinate: The y-coordinate from the origin (4 bytes)
    z_in_mm       int32   // Z-Coordinate: The z-coordinate from the origin (4 bytes)
    quality       uint16  // Quality: The quality of the computed position (2 bytes)
    anchor_count  byte    // Anchor Count: The number of anchors involved in the calculation of this position (1 byte)
    flags         byte    // Flags: 1 bit = inactive mode. 1 bit = was not calculated. 6 bits = unused. (1 byte)
    smoothing     uint16  // Smoothing: The effective smoothing factor (2 bytes)
}

func main() {
    // Ensure correct usage of the program
    if (len(os.Args) != 4) {
        fmt.Printf("Usage: %s GROUP PORT INTERFACE\n", os.Args[0])
        fmt.Printf("        GROUP - IP to listen on (ie: 239.255.76.67)\n")
        fmt.Printf("         PORT - Port to listen on (ie: 7667)\n")
        fmt.Printf("    INTERFACE - Source local interface IP address (This should be your local interface that is on the same subnet as the CUWB Network)\n")
        os.Exit(1)
    }
    var group_string, port, iface string = os.Args[1], os.Args[2], os.Args[3]
    // Create server address
    server_address := group_string + ":" + port

    // Begin socket setup
    var int_name *net.Interface  // Store the net inferface by name
    if (net.ParseIP(iface) != nil) {
        net_address := net.ParseIP(iface)
        // Get all interfaces on the machine in the form of a slice
        ifaces, err := net.Interfaces()
        if (err != nil) {
            fmt.Printf("Error gathering interfaces: ", err)
            os.Exit(1)
        }
        // Loop through all individual interfaces in the ifaces slice
        for _, individual_net_iface := range ifaces {
            // Get address of the individual net interface
            iface_addresses, err := individual_net_iface.Addrs()
            if (err != nil) {
                fmt.Printf("Error gathering address: ", err)
                os.Exit(1)
            }
            for _, address := range iface_addresses {
                // Interface address is in the format IP/Netmask
                netmask_position := strings.Index(address.String(), "/")
                if (netmask_position > -1) {
                  // Truncate the net interface address string to only its IP
                    address_ip := address.String()[:netmask_position]
                    current_ip := net.ParseIP(address_ip)
                    if (net_address.String() == current_ip.String()) {
                        int_name = &individual_net_iface
                    }
                }
            }
        }
    } else {
        int_name, _ = net.InterfaceByName(iface)
    }
    group := net.ParseIP(group_string)

    // Set up connection
    packet_connection, err := net.ListenPacket("udp4", server_address)
    if (err != nil) {
        fmt.Printf("Error making packet connection: ", err)
        os.Exit(1)
    }
    defer packet_connection.Close()
    new_packet_connection := ipv4.NewPacketConn(packet_connection)
    // Set up multicast for group
    if net.ParseIP(group_string).IsMulticast() {
        if err := new_packet_connection.JoinGroup(int_name, &net.UDPAddr{IP: group}); err != nil {
            fmt.Printf("Error with joining interface to group: ", err)
            os.Exit(1)
        }
    }
    if err := new_packet_connection.SetControlMessage(ipv4.FlagDst, true); err != nil {
        fmt.Printf("Error setting control message: ", err)
        os.Exit(1)
    }

    fmt.Printf("Started CDP Go Example\n")

    // Loop on reception of packet
    for {
        // Create the buffer for incoming UDP packets
        data := make([]byte, RECEIVE_BUFFER_SIZE)
        // Block on receiving a UDP packet
        packet_size, _, _, err := new_packet_connection.ReadFrom(data)
        if (err != nil) {
            fmt.Printf("Error on reception of packet: ", err)
        }
        // Initialize a variable that stores the amount of data in current packet that has been parsed
        parsed_bytes := 0
        // The CDP Packet Header is extracted first
        // Ensure that the size of the received data is at least 20 bytes
        if packet_size >= int(unsafe.Sizeof(CDP_HEADER{})) {
            // Get the CDP Header
            // All fields are transmitted in little endian
            var cdp_header CDP_HEADER
            cdp_header.mark = binary.LittleEndian.Uint32(data[0:4])
            cdp_header.sequence = binary.LittleEndian.Uint32(data[4:8])
            for i := 0; i < 8; i++ {
                cdp_header.word[i] = data[i + 8]
            }
            cdp_header.serial_number = binary.LittleEndian.Uint32(data[16:20])
            // Increment parsed bytes
            parsed_bytes += int(unsafe.Sizeof(CDP_HEADER{}))
            data = append(data[:0], data[int(unsafe.Sizeof(CDP_HEADER{})):]...)
            // Ensure the CDP Header is valid (that is, that the mark matches the 'magic word' and the string matches the specified ASCII string)
            if (cdp_header.mark == CDP_MARK_WORD && strings.Compare(string(cdp_header.word[:8]), CDP_STRING_WORD) == 0) {
                // Loop through all bytes in packet
                for parsed_bytes < packet_size {
                    var data_item CDP_DATA_ITEM
                    data_item.di_type = binary.LittleEndian.Uint16(data[0:2])
                    data_item.size = binary.LittleEndian.Uint16(data[2:4])

                    // At this point, if the line below was not commented out, all data items received's types (in decimal) and sizes would print
                    // fmt.Printf("%d %d\n", data_item.di_type, data_item.size)

                    // Increment parsed bytes with the size of the header (4 bytes)
                    parsed_bytes += int(unsafe.Sizeof(CDP_DATA_ITEM{}))
                    // Move along data
                    data = append(data[:0], data[4:]...)
                    // For this example we only care about type 0x0135, which is Position V3, all other types will be ignored
                    if (data_item.di_type == POSITION_V3_TYPE) {
                        // Create a new instance of CDP_POSITION_V3 to hold the new data
                        var position_v3_item CDP_POSITION_V3
                        // Unpack all the values using the binary library
                        position_v3_item.serial_number = binary.LittleEndian.Uint32(data[0:4])
                        position_v3_item.network_time = binary.LittleEndian.Uint64(data[4:12])
                        // The x, y, and z coordinates must be handled differently because
                        //     they are signed ints rather unsigned ints
                        x_buf := bytes.NewBuffer(data[12:16])
                        binary.Read(x_buf, binary.LittleEndian, &position_v3_item.x_in_mm)
                        y_buf := bytes.NewBuffer(data[16:20])
                        binary.Read(y_buf, binary.LittleEndian, &position_v3_item.y_in_mm)
                        z_buf := bytes.NewBuffer(data[20:24])
                        binary.Read(z_buf, binary.LittleEndian, &position_v3_item.z_in_mm)
                        position_v3_item.quality = binary.LittleEndian.Uint16(data[24:26])
                        position_v3_item.anchor_count = (data[26])
                        position_v3_item.flags = (data[27])
                        position_v3_item.smoothing = binary.LittleEndian.Uint16(data[28:30])
                        // Print the data pulled from the data item (flags and smoothing are ignored in this example for conciseness)
                        fmt.Printf("Positon V3 - Serial Number: %d, Network Time: %d, Coordinates: (%d, %d, %d), Quality: %d, Anchor Count: %d\n",
                                   position_v3_item.serial_number, position_v3_item.network_time, position_v3_item.x_in_mm, position_v3_item.y_in_mm,
                                   position_v3_item.z_in_mm, position_v3_item.quality, position_v3_item.anchor_count)
                    }
                    // Move along to the next data item
                    parsed_bytes += int(data_item.size)
                    data = append(data[:0], data[data_item.size:]...)
                }
            }
        }
    }
}
