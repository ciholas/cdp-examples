# Ciholas, Inc. - www.ciholas.com
# Licensed under: creativecommons.org/licenses/by/4.0

import socket
import struct
import sys

# Maximum size of any UDP packet received
RECEIVE_BUFFER_SIZE = 65536
# The expected value of the CDP Header MARK field
MARK = 0x3230434c
# The expected value of the CDP Header STRING field
CDP_STRING_WORD = 'CDP0002\0'

# The CDP Packet Header has a total size of 20 bytes that can be broken down into:
#   Mark: The magic word '0x3230434C' in little-endian (842023756 in decimal) (4 bytes)
#   Sequence: The sequence number of the CDP packet (4 bytes)
#   String: The ASCII string 'CDP0002\0' (8 bytes)
#   Serial Number: Unique identifier of the reporting device (4 bytes)
CDP_HEADER_SIZE = 20
# The Data Item Header has a total size of 4 bytes that can be broken down into:
#   Type: The type of the CDP Data Item (2 bytes)
#   Size: The size of the Data Item, not including the Data Item Header (2 bytes)
DI_HEADER_SIZE = 4
# The Position V3 (type 0x0135) data item has a total size of 30 bytes which can be broken down into:
#   Serial Number: The serial number of the reporting device (4 bytes)
#   Network time: The timestamp when the sensor recorded the data (8 bytes)
#   Coordinates: The coordinates from the origin (12 bytes); this field can be further broken down into x, y, and z coordinates (each 4 bytes)
#   Quality: The quality of the computed position (2 bytes)
#   Anchor Count: The number of anchors involved in the calculation of this position (1 byte)
#   Flags: 1 bit = inactive mode. 1 bit = was not calculated. 6 bits = unused. (1 byte)
#   Smoothing: The effective smoothing factor (2 bytes)
CDP_DATA_ITEM_POSITION_V3 = 0x0135

# Ensure correct usage of the program
if len(sys.argv) != 4:
    print('Usage: {} GROUP PORT INTERFACE'.format(sys.argv[0]))
    print('        GROUP - IP to listen on (ie: 239.255.76.67)')
    print('         PORT - Port to list on (ie: 7667)')
    print('    INTERFACE - Source local interface IP address (This should be your local interface that is on the same subnet as the CUWB Network)')
    sys.exit(1)
else:
    group_string = sys.argv[1]
    port = int(sys.argv[2])
    interface = sys.argv[3]

try:
    # The socket on which to receive CDP packets
    listen_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # Bind the socket.
    if sys.platform == 'win32':
        listen_socket.bind((interface, port))
    else:
        listen_socket.bind((group_string, port))
    # Setup the multicast for the GROUP
    listen_socket.setsockopt(socket.SOL_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(group_string)+socket.inet_aton(interface))
except Exception as e:
    print('Failed to setup socket. Ensure IP, PORT, and INTERFACE are correct')
    print(e)
    sys.exit(1)

print('Started CDP Python Example Program')

# The setup of the socket is complete, so data can now be pulled from it
# Loop on reception of packet
while True:
    # Block on receiving a UDP packet
    # The return value of the recvfrom method is a tuple
    data, address = listen_socket.recvfrom(RECEIVE_BUFFER_SIZE)
    
    # At this point, if the line below was included, raw data should be printed to the console. If no data is printed, verify that the CUWB Network is running
    # print(data)

    # The CDP Packet Header is extracted first
    # Ensure packet data is at least 20 characters long
    if len(data) < CDP_HEADER_SIZE:
        raise ValueError('Packet Size Error')

    # Get the CDP Header
    # '<II8sI' represents '<' for little endian style, 'I' for 4-byte Mark integer, 'I' for 4-byte sequence integer, '8s' for 8-byte string, 'I' for 4-byte serial number
    mark, sequence, string, serial = struct.unpack("<II8sI", data[:CDP_HEADER_SIZE])

    # Ensure the CDP Header is valid (that is, that the mark matches the 'magic word' and that the string matches the specified ASCII string)
    string = string.decode()
    if mark != MARK or string != CDP_STRING_WORD:
        raise ValueError('Unrecognized CDP Header')
    data = data[CDP_HEADER_SIZE:]

    # Loop while data items are available
    while len(data) >= DI_HEADER_SIZE:

        # Extract the Data Item Header
        # '<HH' represents '<' for little endian style, 'H' for 2-byte Type integer, 'H' for 2-byte Size integer
        di_type, di_size = struct.unpack('<HH', data[:DI_HEADER_SIZE])
        data = data[DI_HEADER_SIZE:]
        # Ensure that the remaining is at least as long as required by the specified size
        if len(data) < di_size:
            print('Type: 0x{:04X}, Expected data size: {}, Actual data size: {}'.format(di_type, di_size, len(data)))
            break

        # Remove the data for this Data Item from data
        di_data = data[:di_size]
        data = data[di_size:]
        # For this example, we only care about type 0x0135, which is Position V3, all other types will be ignored
        if di_type == CDP_DATA_ITEM_POSITION_V3:
            # '<I' corresponds to a 4-byte integer in little endian for the serial_number
            # Also note that the result of unpack is always a tuple, so a comma after the serial_number variable assigns the value to the variable and not the tuple
            serial_number, = struct.unpack('<I', di_data[:4])
            di_data = di_data[4:]
            # '<Q' corresponds to a 8-byte integer in little endian for the network_time
            network_time, = struct.unpack('<Q', di_data[:8])
            di_data = di_data[8:]
            # '<iii' corresponds to 3 4-byte (signed) integers in little endian for the x, y, and z coodinates
            x,y,z = struct.unpack('<iii', di_data[:12])
            di_data = di_data[12:]
            # '<H' corresponds to a 2-byte integer in little endian for the quality
            quality, = struct.unpack('<H', di_data[:2])
            di_data = di_data[2:]
            # '<B' corresponds to a 1-byte integer in little endian for anchor_count
            anchor_count, = struct.unpack('<B', di_data[:1])
            di_data = di_data[1:]
            # '<B' corresponds to a 1-byte integer in little endian for flags
            flags, = struct.unpack('<B', di_data[:1])
            di_data = di_data[1:]
            # '<H' corresponds to a 2-byte integer in little endian for smoothing
            smoothing, = struct.unpack('<H', di_data[:2])
            di_data = di_data[2:]

            # Print out all the fields in the Position V3 data item
            # Flags and Smoothing will be ignored for conciseness
            print('Position V3 - Serial Number: {}, Network Time: {}, Coordinates: ({}, {}, {}) Quality: {}, Anchor Count: {}'.format(serial_number, network_time, x, y, z, quality, anchor_count))

    # After reading data from CDP packet, ensure no data is left over
    if data:
        raise ValueError('Incomplete CDP Packet')
