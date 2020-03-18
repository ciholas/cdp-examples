# Ciholas, Inc. - www.ciholas.com
# Licensed under: creativecommons.org/licenses/by/4.0

import socket
import sys
import cdp

# Maximum size of any UDP packet received
RECEIVE_BUFFER_SIZE = 65536

# Ensure correct usage of the program
if len(sys.argv) != 4:
    print('Usage: {} GROUP PORT INTERFACE'.format(sys.argv[0]))
    print('        GROUP - IP to listen on (ie: 239.255.76.67)')
    print('         PORT - Port to listen on (ie: 7667)')
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

    # Now the CDP module can be used to easily parse and decode the data
    cdp_packet = cdp.CDP(data)
    # For this example, we only care about type 0x0135, which is Position V3, all other types will be ignored
    # The CDP module makes it easy to extract only specfic data items
    for di_data in cdp_packet.data_items_by_type.get(0x0135, []):
        serial_number = di_data.serial_number
        network_time = di_data.network_time
        x, y, z = di_data.x, di_data.y, di_data.z
        quality = di_data.quality
        anchor_count = di_data.anchor_count
        flags = di_data.flags
        smoothing = di_data.smoothing

        # Print out all the fields in the Position V3 data item
        # Flags and Smoothing will be ignored for conciseness
        print('Position V3 - Serial Number: {}, Network Time: {}, Coordinates: ({}, {}, {}) Quality: {}, Anchor Count: {}'.format(serial_number, network_time, x, y, z, quality, anchor_count))
