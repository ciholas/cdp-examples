# CDP Python Example

## Running

Run the program with
```bash
python3 cdp-python-example.py GROUP PORT INTERFACE
```

GROUP is the multicast IP to listen on  
PORT is the UDP PORT to listen on  
INTERFACE is the Interface IP address from the CDP settings


## Extending

Specific handlers can be written for different data types and used with the
corresponding type pulled from the data item header.

Also see [cdp-py](https://github.com/ciholas/cdp-py).
