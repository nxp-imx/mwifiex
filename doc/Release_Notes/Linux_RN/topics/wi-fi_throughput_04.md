[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)
- [STA throughput](#sta-throughput)
- [P2P-GO throughput](#p2pgo-throughput)
- [P2P-GC Throughput](#p2pgc-throughput)
- [Mobile AP Throughput](#mobile-ap-throughput)
- [OpenThread throughput test](#open-thread-throughput-test)

<a name="throughput-test-setup"></a>
## Throughput test setup

- Environment: Shield Room - Over the Air
- External Access Point: Asus RT-AX88U \(FW-3.0.0.4.386\_49674\)
- DUT: Murata M.2 Module LBES5PL2EL with MCIMX8M-EVK platform
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
- Channel: 6 \|36

<a name="sta-throughput"></a>
## STA throughput

External Access Point: Asus RT-AX88U

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|57|59|63|63|
|WPA2-AES|57|57|63|63|
|WPA3-SAE|56|58|63|62|

STA Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|114|118|128|128|
|WPA2-AES|113|122|125|127|
|WPA3-SAE|117|118|128|124|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|58|57|62|66|
|WPA2-AES|59|59|65|63|
|WPA3-SAE|58|60|62|62|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|119|126|129|132|
|WPA2-AES|118|124|130|131|
|WPA3-SAE|115|123|132|131|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|53|75|56|78|
|WPA2-AES|53|74|56|78|
|WPA3-SAE|54|75|56|78|

STA Mode Throughput - AC Mode|5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|161|171|176|176|
|WPA2-AES|159|163|177|178|
|WPA3-SAE|161|168|180|179|

STA Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|325|369|375|391|
|WPA2-AES|325|363|376|385|
|WPA3-SAE|328|366|378|388|

STA Mode Throughput - AX Mode|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|115|113|118|129|
|WPA2-AES|115|110|117|124|
|WPA3-SAE|113|117|118|119|


STA Mode Throughput - AX Mode|2.4 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|209|229|240|252|
|WPA2-AES|210|232|239|251|
|WPA3-SAE|194|233|235|248|

STA Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|115|112|127|129|
|WPA2-AES|114|111|127|127|
|WPA3-SAE|115|112|121|124|

STA Mode Throughput - AX Mode|5 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|218|220|244|250|
|WPA2-AES|218|216|243|243|
|WPA3-SAE|219|219|247|242|


STA Mode Throughput - AX Mode|5 GHz Band|80 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|405|418|461|502|
|WPA2-AES|397|418|461|500|
|WPA3-SAE|400|412|455|497|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|55|55|60|61|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|118|125|128|132|

P2P - GO Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|336|369|385|378|

P2P - GO Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|400|418|449|476|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|53|55|60|61|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|118|125|128|132|

P2P - GC Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|332|365|380|381|

P2P - GC Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|398|412|452|470|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|62|59|64|62|
|WPA2-AES|62|55|64|61|
|WPA3-SAE|57|55|61|61|


Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|118|119|126|129|
|WPA2-AES|118|120|122|128|
|WPA3-SAE|120|119|122|126|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|63|59|61|61|
|WPA2-AES|61|60|62|61|
|WPA3-SAE|62|55|63|64|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|129|107|133|111|
|WPA2-AES|128|125|133|133|
|WPA3-SAE|128|125|133|133|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|70|71|75|76|
|WPA2-AES|74|70|79|77|
|WPA3-SAE|74|68|74|76|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|169|163|177|175|
|WPA2-AES|168|167|173|177|
|WPA3-SAE|167|167|176|177|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|348|352|382|390|
|WPA2-AES|348|350|382|388|
|WPA3-SAE|350|348|378|386|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|115|112|119|125|
|WPA2-AES|114|113|122|124|
|WPA3-SAE|111|115|123|123|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Open Security|212|208|233|234|
|WPA2-AES|205|207|231|232|
|WPA3-SAE|210|202|232|235|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|120|119|128|126|
|WPA2-AES|115|116|125|126|
|WPA3-SAE|116|116|121|128|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|223|217|242|246|
|WPA2-AES|225|212|242|242|
|WPA3-SAE|223|215|245|242|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|402|406|452|498|
|WPA2-AES|398|403|449|482|
|WPA3-SAE|390|399|445|499|

<a name="open-thread-throughput-test"></a>
## OpenThread throughput test

- Environment: closed
- DUT: NXP reference board with 8MMINILPD4-EVKB platform
- Clock rate: 10 MHz
- DUT TX Power: 0 dBm
- OTREF TX Power: 20 dBm

|Protocol|TCP \(kbit/s\)||UDP \(kbit/s\)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Thread leader|81|84|88|91|
|Thread child|82|83|88|91|
