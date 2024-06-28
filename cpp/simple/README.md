# CDP C++ Example

## Compiling

Compile the program with
```bash
g++ -o cdp-cpp-example main.cpp
```


## Running

Run the program with
```bash
./cdp-cpp-example GROUP PORT IFACE
```

GROUP is the multicast IP to listen on  
PORT is the UDP PORT to listen on
IFACE is the IP of network interface to bind socket to


## Extending

Unpacking structs for additional data types could be added to collect further information from the CDP stream.
