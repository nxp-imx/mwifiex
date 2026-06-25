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
    - Driver Load Parameters:
      cal_data_cfg=none, cfg80211_wext=0xf, host_mlme=1, amsdu_deaggr=1, net_rx=1, tx_skb_clone=1, tx_work=1
      
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

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|56|58|63|61|
|WPA2-AES|56|58|62|61|
|WPA3-SAE|57|59|62|61|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|58|63|64|63|
|WPA2-AES|57|62|66|64|
|WPA3-SAE|56|63|65|64|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|70|73|72|75|
|WPA2-AES|70|73|73|76|
|WPA3-SAE|69|74|72|74|

STA Mode Throughput - AX Mode|2.5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|90|96|98|100|
|WPA2-AES|88|94|99|99|
|WPA3-SAE|90|93|98|99|

STA Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|94|99|103|102|
|WPA2-AES|91|99|102|102|
|WPA3-SAE|92|98|103|103|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|61|58|64|63|

P2P - GO Mode Throughput - BGN Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|63|58|65|63|

P2P - GO Mode Throughput - HE Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|97|93|101|98|

P2P - GO Mode Throughput - HE Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|99|94|101|98|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|63|58|62|62|

P2P - GC Mode Throughput - BGN Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|63|58|65|64|

P2P - GC Mode Throughput - HE Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|99|93|102|100|

P2P - GC Mode Throughput - HE Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|99|93|103|100|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|62|57|64|62|
|WPA2-AES|62|57|64|62|
|WPA3-SAE|62|57|64|62|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|60|58|66|64|
|WPA2-AES|59|58|66|64|
|WPA3-SAE|60|58|66|63|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|76|71|79|74|
|WPA2-AES|76|71|79|74|
|WPA3-SAE|76|70|79|74|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|97|92|101|98|
|WPA2-AES|96|92|101|97|
|WPA3-SAE|96|92|101|97|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|99|94|103|99|
|WPA2-AES|99|94|103|99|
|WPA3-SAE|99|94|103|99|

<a name="openthread-throughput-test"></a>
## OpenThread throughput test

- Environment: closed
- DUT: NXP reference board with 8MMINILPD4-EVKB platform
- Clock rate: 10 MHz
- DUT TX Power: 0 dBm
- OTREF TX Power: 13 dBm

|Protocol|TCP (kbit/s)||UDP (kbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Thread leader|84|84|92|92|
|Thread child|84|84|92|92|




