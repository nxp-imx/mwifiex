[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)
- [STA throughput](#sta-throughput)
- [P2P-GO throughput](#p2pgo-throughput)
- [P2P-GC throughput](#p2pgc-throughput)
- [Mobile AP throughput](mobile-ap-throughput)
- [Open Thread throughput test](#openthread-throughput-test)

<a name="throughput-test-setup"></a>
## Throughput test setup

- Environment: Shield Room - Over the Air
- External access point: Asus RT-AX88U
- DUT: IW610-Murata M.2 module: LBES0ZZ2LL with MCIMX8M-EVK platform
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

External AP: Asus RT-AX88U

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|56|59|60|62|
|WPA2-AES|56|57|59|59|
|WPA3-SAE|56|54|61|56|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|57|63|64|66|
|WPA2-AES|57|62|64|65|
|WPA3-SAE|57|62|64|65|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|67|75|77|78|
|WPA2-AES|67|75|77|78|
|WPA3-SAE|67|75|77|78|

STA Mode Throughput - AX Mode|2.5 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|87|96|94|100|
|WPA2-AES|86|93|95|96|
|WPA3-SAE|87|93|95|97|

STA Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|91|100|102|104|
|WPA2-AES|87|99|99|103|
|WPA3-SAE|87|99|98|103|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|59|56|62|62|

P2P - GO Mode Throughput - BGN Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|57|57|64|63|

P2P - GO Mode Throughput - HE Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|92|90|98|95|

P2P - GO Mode Throughput - HE Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|96|92|100|98|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|56|56|62|62|

P2P - GC Mode Throughput - BGN Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|57|57|64|63|

P2P - GC Mode Throughput - HE Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|94|90|98|97|

P2P - GC Mode Throughput - HE Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|94|92|99|99|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|58|55|62|61|
|WPA2-AES|54|55|61|60|
|WPA3-SAE|54|55|62|61|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|61|57|64|63|
|WPA2-AES|57|57|64|63|
|WPA3-SAE|57|57|64|63|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|68|70|77|73|
|WPA2-AES|69|70|77|73|
|WPA3-SAE|69|70|77|73|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|90|89|96|95|
|WPA2-AES|90|88|95|94|
|WPA3-SAE|90|88|96|95|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|96|93|101|98|
|WPA2-AES|96|92|101|98|
|WPA3-SAE|96|92|101|98|

<a name="openthread-throughput-test"></a>
## OpenThread throughput test

- Environment: closed
- DUT: NXP reference board with 8MMINILPD4-EVKB platform
- Clock rate: 10 MHz
- DUT TX Power: 0 dBm
- OTREF TX Power: 20 dBm

|Protocol|TCP (kbit/s)||UDP (kbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Thread leader|83|85|89|92|
|Thread child|85|85|93|93|




