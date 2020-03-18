# Ciholas, Inc. - www.ciholas.com
# Licensed under: creativecommons.org/licenses/by/4.0

require 'bindata'
require 'ipaddr'
require 'socket'

# Maximum size of any UDP packet received
RECEIVE_BUFFER_SIZE = 65536

# The expected value of the CDP Header MARK field
CDP_MARK_WORD = 0x3230434C
# The expected value of the CDP Header STRING field
CDP_STRING_WORD = 'CDP0002'
# The type for a CDP Position V3 Data Item
CDP_DATA_ITEM_POSITION_V3 = 0x0135
# The CDP Data Item has a header with a total size of 4 bytes and a variable length payload
# CDP Data Item container type
class CdpDataItem < BinData::Record
  endian :little                         # All data items are transmitted in little-endian

  uint16 :identifier                     # Identifier: The type of the CDP Data Item (2 bytes) (type is a reserved word in Ruby)
  uint16 :len                            # Len: The size of the Data Item, not including the Data Item Header (2 bytes) (size is a reserved word in Ruby)
  string :payload, :read_length => :len  # The remaining bytes in the Data Item are the payload
end

# The CDP Packet has a header with a total size of 20 bytes and a variable length array of data items
# CDP Packet container type
class CdpPacket < BinData::Record
  endian :little               # All packets are transmitted in little-endian

  buffer :header, :length => 20 do
    uint32 :mark               # Mark: The magic word '0x3230434C' in little-endian (842023756 in decimal) (4 bytes)
    uint32 :sequence           # Sequence: The sequence number of the CDP packet (4 bytes)
    string :str, :length => 8  # Str: The ASCII string 'CDP0002\0' (8 bytes)
    uint32 :serial             # Serial Number: Unique identifier of the reporting device (4 bytes)
  end
  array :data_items, :type => CdpDataItem, :read_until => :eof  # The remaining bytes are an array of CDP Data Items
end

# The Position V3 (type 0x0135) data item has a total size of 30 bytes
# Position V3 Data Item container type
class CdpPositionV3 < BinData::Record
  endian :little         # All data items are transmitted in little-endian

  uint32 :serial_number  # Serial Number: The serial number of the reporting device (4 bytes)
  uint64 :network_time   # Network Time: The timestamp when the sensor recorded the data (8 bytes)
  int32 :x_in_mm         # X-Coordinate: The x-coordinate from the origin (4 bytes)
  int32 :y_in_mm         # Y-Coordinate: The y-coordinate from the origin (4 bytes)
  int32 :z_in_mm         # Z-Coordinate: The z-coordinate from the origin (4 bytes)
  uint16 :quality        # Quality: The quality of the computed position (2 bytes)
  uint8  :anchor_count   # Anchor Count: The number of anchors involved in the calculation of this position (1 byte)
  uint8  :flags          # Flags: 1 bit = inactive mode. 1 bit = was not calculated. 6 bits = unused. (1 byte)
  uint16 :smoothing      # The effective smoothing factor (2 bytes)
end

# Ensure correct usage of the program
if ARGV.length == 3
  multicast_addr = ARGV[0]
  udp_port = ARGV[1]
  interface_addr = ARGV[2]
else
  puts 'Usage: cdp-ruby-example GROUP PORT INTERFACE'
  puts '        GROUP - IP to listen on (ie: 239.255.76.67)'
  puts '         PORT - Port to listen on (ie 7667)'
  puts '    INTERFACE - Source local interface IP address (This should be your local interface that is on the same subnet as the CUWB Network)'
  exit(1)
end

puts 'Started CDP Ruby Example Program'

# Setup membership
membership = IPAddr.new(multicast_addr).hton + IPAddr.new(interface_addr).hton
# Create a UDP socket for listening to CDP packets
socket = UDPSocket.new
# Allow other apps to use the same port/ifc
socket.setsockopt(:SOL_SOCKET, :SO_REUSEADDR, 1)
# Setup receive ip address
socket.setsockopt(:IPPROTO_IP, :IP_ADD_MEMBERSHIP, membership)
# Bind the socket
socket.bind(Socket::INADDR_ANY, udp_port)

# The setup of the socket is complete, so data can now be pulled from it
# Loop on reception of packet
loop do
  # Block on receiving a UDP packet
  message, addr = socket.recvfrom(RECEIVE_BUFFER_SIZE)
  # At this point, if the line below was not commented out, raw data would be printed to the console. If no data is printed, verify that the CUWB Network is running
  # puts message

  # Extract a CDP Packet from the message
  cdp_packet = CdpPacket.read(message)

  # Ensure the CDP Header is valid (that is, that the mark matches the 'magic word' and that the string matches the specified ASCII string)
  if cdp_packet.header.mark == CDP_MARK_WORD and cdp_packet.header.str[0...6] == CDP_STRING_WORD[0...6]
    begin
      # Loop through all data items in the packet
      cdp_packet.data_items.each do |data_item|
        # For this example, we only care about type 0x0135, which is Position V3, all other types will be ignored
        if data_item.identifier == CDP_DATA_ITEM_POSITION_V3
          # Extract the remaining bytes from the Data Item payload, these are the bytes corresponding to Position V3
          position_v3_item = CdpPositionV3.read(data_item.payload)
          # Print out all the fields in the Position V3 data item
          # Flags and Smoothing will be ignored for conciseness
          puts "Position V3 - Serial Number #{position_v3_item.serial_number}, Network Time #{position_v3_item.network_time}, Coordinates: (#{position_v3_item.x_in_mm}, #{position_v3_item.y_in_mm}, #{position_v3_item.z_in_mm}), Quality: #{position_v3_item.quality}, Anchor Count: #{position_v3_item.anchor_count}"
        end
      end
    end
  else
    raise 'Unrecognized CDP Header'
  end
end
