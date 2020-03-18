# CDP Ruby Example

## Dependencies

The bindata package is required. Install it with
```bash
gem install bindata
```

## Running

Run the program with
```bash
ruby cdp-ruby-example.rb GROUP PORT INTERFACE
```

GROUP is the multicast IP to listen on  
PORT is the UDP PORT to listen on  
INTERFACE is the Interface IP address from the CDP settings


## Extending

Specific handlers can be written for different data types and used with the
corresponding type pulled from the data item header.
