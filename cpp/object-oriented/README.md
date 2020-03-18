# CDP C++ Example

## Compiling

Build the CDP C++ example with
```bash
rake clean source
```

## Running

Run the program with
```bash
output/cdp-cpp-example GROUP PORT
```

GROUP is the multicast IP to listen/transmit on  
PORT is the UDP PORT to listen/transmit on

## Extending

See the example handler function in `src/main.cpp` for how to handle receiving specific data items

See the transmit example in the `src/main.cpp` file
