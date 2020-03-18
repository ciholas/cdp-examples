# CDP C Example

## Compiling

Build the CDP C example with:
```bash
rake clean source
```

## Running

Run the program with
```bash
output/cdp-c-example GROUP PORT
```

GROUP is the multicast IP to listen on  
PORT is the UDP PORT to listen on

## Extending

See the example handler function in `src/main.c` for how to handle specific data items
