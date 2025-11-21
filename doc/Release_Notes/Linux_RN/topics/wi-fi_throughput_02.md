[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)

- [STA throughput](#sta-throughput)

- [P2P-GO throughput](#p2pgo-throughput)

- [P2P-GC throughput](#p2pgc-throughput)

- [Mobile AP throughput](#mobile-ap-throughput)

<a name="throughput-test-setup"></a>
## Throughput test setup

- Environment: Shield Room - Over the Air
- External Access Point: NXP 88W9098 PCIe-UART
- DUT: 88W8997- Murata M.2 \(Module: LBEE5XV1YM\) with MCIMX8M-EVK platform
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

- External Client: NXP 88W9098 PCIe-UART
- Channel: 6 \| 36

<a name="sta-throughput"></a>
## STA throughput

External AP: NXP 88W9098 PCIe-UART

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|114|123|123|127|
|WPA2-AES|111|123|123|127|
|WPA3-SAE|114|123|123|127|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|114|125|128|130|
|WPA2-AES|112|122|128|129|
|WPA3-SAE|111|123|128|129|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|216|248|252|260|
|WPA2-AES|205|248|253|260|
|WPA3-SAE|206|249|254|260|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|139|145|150|155|
|WPA2-AES|140|145|150|156|
|WPA3-SAE|138|147|150|156|

STA Mode Throughput - AC Mode|5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|300|321|341|353|
|WPA2-AES|299|323|345|353|
|WPA3-SAE|300|324|340|354|

STA Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|578|696|665|744|
|WPA2-AES|567|674|663|741|
|WPA3-SAE|564|676|663|741|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20MHz|1SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|114|120|122|129|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz|2SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|217|238|233|259|

P2P - GO Mode Throughput - AC Mode|5 GHz Band|80 MHz|2SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|593|600|670|721|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20MHz|1SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|114|121|122|129|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz|2SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|202|237|237|256|

P2P - GC Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|592|606|665|730|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP 88W9098 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|115|118|124|126|
|WPA2-AES|118|118|124|127|
|WPA3-SAE|116|118|123|126|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|121|117|127|128|
|WPA2-AES|121|117|127|128|
|WPA3-SAE|120|116|124|126|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|240|238|254|259|
|WPA2-AES|241|238|254|259|
|WPA3-SAE|244|239|250|260|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|143|140|149|151|
|WPA2-AES|137|139|144|151|
|WPA3-SAE|138|138|145|150|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|307|302|338|345|
|WPA2-AES|310|305|340|345|
|WPA3-SAE|311|306|341|345|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|689|697|733|741|
|WPA2-AES|672|676|733|718|
|WPA3-SAE|685|682|734|729|