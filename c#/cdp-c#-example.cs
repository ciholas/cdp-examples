// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Runtime.InteropServices;

public class position_v3_parser
{
    // Maximum size of any UDP packet received
    private const int RECEIVE_BUFFER_SIZE = 65536;
    // The expected value of the CDP Header MARK field
    private const int CDP_MARK_WORD = 0x3230434C;
    // The expected value of the CDP Header STRING field
    private const string CDP_STRING_WORD = "CDP0002\0";

    private const int POSITION_V3_TYPE = 0x0135;

    // The CDP Packet Header has a total size of 20 bytes that can be broken down into:
    //   Mark: The magic word '0x3230434C' in little-endian (842023756 in decimal) (4 bytes)
    //   Sequence: The ASCII string 'CDP0002\0' (8 bytes)
    //   Serial Number: Unique identifier of the reporting device (4 bytes)
    private const int CDP_HEADER_SIZE = 20;
    // The Data Item Header has a total size of 4 bytes that can be broken down into:
    //   Type: The type of the CDP Data Item (2 bytes)
    //   Size: The size of the Data Item, not including the Data Item Header (2 bytes)
    private const int DI_HEADER_SIZE = 4;
    // The Position V3 (type 0x0135) data item has a total size of 30 bytes which can be broken down into:
    //   Serial Number: The serial number of the reporting device (4 bytes)
    //   Network Time: The timestamp when the sensor recorded the data (8 bytes)
    //   Coordinates: The coordinates from the origin (12 bytes); this field can be further broken down into x, y, and z coordinates (each 4 bytes)
    //   Quality: The quality of the computted position (2 bytes)
    //   Anchor Count: The number of involved in the calculation of this position (1 byte)
    //   Flags: 1 bit = inactive mode. 1 bit = was not calculated. 6 bits = unused. (1 byte)
    //   Smoothing: The effective smoothing factor (2 bytes)
    private const int POSITION_V3_SIZE = 30;

    public static void Main(string[] args)
    {
        // Ensure correct usage of the program
        if (args.Length != 3)
        {
            Console.WriteLine("dotnet run GROUP PORT INTERFACE\n");
            Console.WriteLine("        GROUP - IP to listen on (ie: 239.255.76.66)");
            Console.WriteLine("         PORT - Port to listen on (ie:7667)");
            Console.WriteLine("    INTERFACE - Source local interface IP address (This should be your local interface that is on the same subnet as the CUWB Network)");
            System.Environment.Exit(1);
        }
        string group_string = args[0];
        int port = Convert.ToInt32(args[1]);
        string iface_string = args[2];
        IPAddress group = IPAddress.Parse(group_string);
        IPAddress iface = IPAddress.Parse(iface_string);
        IPEndPoint endpoint;
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            endpoint = new IPEndPoint(iface, port);
        }
        else
        {
            endpoint = new IPEndPoint(group, port);
        }
        // Create a UDP socket for listening to CDP packets
        Socket cdp_socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        cdp_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
        // Bind the socket
        cdp_socket.Bind(endpoint);
        // Setup the multicast for the group
        cdp_socket.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership, new MulticastOption(group, iface));

        Console.WriteLine("Started CDP C# Example Program");

        // The setup of the socket is complete, so data can now be pulled from it
        // Loop on reception of packet
        while (true)
        {
            byte [] data = new byte[RECEIVE_BUFFER_SIZE];
            // Block on received a UDP packet
            int parsed_bytes = 0;
            int bytes_received = cdp_socket.Receive(data);

            // At point, if the line below was included, data should be printed to the console. If no data is printed, verify that the CUWB Network is running
            // Console.WriteLine(data);
            
            // Ensure that the size of the received data is at least 20 bytes
            if (bytes_received > CDP_HEADER_SIZE)
            {
                // Extract the Packet Header
                UInt32 cdp_mark = BitConverter.ToUInt32(data, 0);
                UInt32 cdp_sequence = BitConverter.ToUInt32(data, 4);
                string cdp_string = Encoding.UTF8.GetString(data, 8, 8);
                UInt32 header_serial_number = BitConverter.ToUInt32(data, 16);
                // Increment parsed bytes with the size of the packet header
                parsed_bytes += CDP_HEADER_SIZE;
                // Ensure the CDP Header is valid (that is, that the mark matches the 'magic word' and that the string matches the specified ASCII string)
                if (cdp_mark == CDP_MARK_WORD && cdp_string.Equals(CDP_STRING_WORD))
                {
                    // Loop through all bytes in packet
                    while (parsed_bytes < bytes_received)
                    {
                        // Extract the Data Item Header
                        UInt16 di_type = BitConverter.ToUInt16(data, parsed_bytes);
                        UInt16 di_size = BitConverter.ToUInt16(data, parsed_bytes + 2);
                        // Increment parsed bytes with the size of the data item header
                        parsed_bytes += DI_HEADER_SIZE;
                        // For this example, we only care about 0x0135, which is Position V3, all other types will be ignored
                        if (di_type == POSITION_V3_TYPE)
                        {
                            // Extract the Position V3 Fields
                            UInt32 serial_number = BitConverter.ToUInt32(data, parsed_bytes);
                            UInt64 network_time = BitConverter.ToUInt64(data, parsed_bytes + 4);
                            Int32 x_in_mm = BitConverter.ToInt32(data, parsed_bytes + 12);
                            Int32 y_in_mm = BitConverter.ToInt32(data, parsed_bytes + 16);
                            Int32 z_in_mm = BitConverter.ToInt32(data, parsed_bytes + 20);
                            UInt16 quality = BitConverter.ToUInt16(data, parsed_bytes + 24);
                            byte anchor_count = data[parsed_bytes + 26];
                            byte flags = data[parsed_bytes + 27];
                            UInt16 smoothing = BitConverter.ToUInt16(data, parsed_bytes + 28);
                            // Print the data pulled from the data item (flags and smoothing are ignored in this example for conciseness)
                            Console.WriteLine("Position V3 - Serial Number: {0:D}, Network Time: {1:D}, Coordinates: ({2:D}, {3:D}, {4:D}), Quality: {5:D}, Anchor Count: {6:D}",
                                              serial_number, network_time, x_in_mm, y_in_mm, z_in_mm, quality, anchor_count);
                        }
                        // Increment the number of parsed_bytes with the size of the data item
                        parsed_bytes += di_size;
                    }
                }
            }
        }
    }
}
