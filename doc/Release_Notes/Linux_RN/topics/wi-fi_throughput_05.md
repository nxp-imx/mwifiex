[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)
- [STA throughput](#sta-throughput)
- [P2P-GO throughput](#p2pgo-throughput)
- [P2P-GC throughput](p2pgc-throughput)
- [Mobile AP throughput](#mobile-ap-throughput)

<a name="throughput-test-setup"></a>
## Throughput test setup

- Environment: Shield Room - Over the Air
- Access Point: NXP IW620
- DUT: 88W8987-Murata M.2 module: LBEE5QD1ZM with MCIMX8M-EVK platform
    - iPerf commands:

        - TCP server
        ```
        # iperf -s -i1 -fm -w 2M
        ```

        - TCP client
        ```
        # iperf -c <ip_address> -i1 -fm -w 2M -t60 -P5
        ```

        - UDP server
        ```
        # iperf -s -u -i1 -fm -w 2M
        ```

        - UDP client
        ```
        # iperf -c <ip_address> -i1 -fm -w 2M -t60 -b 800 -P5
        ```

        **Note:** For optimized throughput, add iPerf parameters such as TCP window size and parallel streams. The above-described iPerf parameters are an example.

- External Client: NXP IW620 PCIe-UART
- Channel: 6 \| 36

<a name="sta-throughput"></a>
## STA throughput

External AP: NXP IW620

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|56|61|60|62|
|WPA2-AES|57|60|60|62|
|WPA3-SAE|56|61|60|62|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|56|62|61|63|
|WPA2-AES|57|62|61|63|
|WPA3-SAE|57|62|61|63|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|117|131|126|134|
|WPA2-AES|116|130|125|134|
|WPA3-SAE|115|131|125|134|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|68|72|73|75|
|WPA2-AES|68|72|73|74|
|WPA3-SAE|68|72|73|74|

STA Mode Throughput - AC Mode|5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|156|168|168|174|
|WPA2-AES|155|168|166|174|
|WPA3-SAE|153|168|166|174|

STA Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|265|350|289|363|
|WPA2-AES|244|348|269|363|
|WPA3-SAE|250|347|270|363|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|56|59|61|64|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|116|126|124|135|

P2P - GO Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|224|317|250|384|

<a name="p2p-gc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|58|58|61|64|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|115|126|123|135|

P2P - GC Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|238|329|263|358|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|56|58|60|63|
|WPA2-AES|56|58|60|62|
|WPA3-SAE|56|58|60|63|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|58|59|61|64|
|WPA2-AES|57|59|61|64|
|WPA3-SAE|58|59|61|64|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|117|127|126|135|
|WPA2-AES|117|126|125|135|
|WPA3-SAE|116|126|125|135|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|70|70|74|75|
|WPA2-AES|70|68|74|74|
|WPA3-SAE|70|70|74|74|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|149|168|162|180|
|WPA2-AES|147|169|161|180|
|WPA3-SAE|147|168|161|180|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|242|339|274|390|
|WPA2-AES|230|315|259|385|
|WPA3-SAE|229|320|259|385|