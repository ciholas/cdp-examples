# CDP Go Example

## Dependencies

The Golang ipv4 package is required. Install it with
```bash
go get golang.org/x/net/ipv4
```

## Compiling

Compile the program with
```bash
go build -o cdp-golang-example
```


## Running

Run the program with
```bash
./cdp-golang-example GROUP PORT INTERFACE
```

GROUP is the multicast IP to listen on  
PORT is the UDP PORT to listen on  
INTERFACE is the Interface IP address from the CDP settings

## Extending

Unpacking structs for additional data types could be added to collect further information from the CDP stream.
