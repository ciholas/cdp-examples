# CDP C Example

## Compiling

Build the CDP C example with:
```bash
rake clean source
```

## Running

Run the program with
```bash
output/cdp-c-example GROUP PORT IFACE
```

GROUP is the multicast IP to listen on  
PORT is the UDP PORT to listen on
IFACE is the IP of network interface to bind socket to

## Extending

See the example handler function in `src/main.c` for how to handle specific data items
