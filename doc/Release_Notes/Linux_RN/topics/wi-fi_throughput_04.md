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
- Channel: 6 \|36

<a name="sta-throughput"></a>
## STA throughput

External Access Point: Asus RT-AX88U

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|57|57|63|62|
|WPA2-AES|57|57|62|62|
|WPA3-SAE|56|56|62|62||

STA Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|116|116|131|129|
|WPA2-AES|116|113|130|126|
|WPA3-SAE|117|114|131|127|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|60|62|62|65|
|WPA2-AES|59|62|62|64|
|WPA3-SAE|59|62|62|64|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|120|130|134|134|
|WPA2-AES|119|128|134|133|
|WPA3-SAE|121|128|134|133|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|72|75|76|78|
|WPA2-AES|72|75|76|78|
|WPA3-SAE|72|75|75|78|

STA Mode Throughput - AC Mode|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|167|165|175|174|
|WPA2-AES|165|164|174|173|
|WPA3-SAE|166|164|174|174|

STA Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|332|367|374|387|
|WPA2-AES|331|365|371|385|
|WPA3-SAE|330|364|373|385|

STA Mode Throughput - AX Mode|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|112|114|124|125|
|WPA2-AES|110|111|124|123|
|WPA3-SAE|110|110|120|122|


STA Mode Throughput - AX Mode|2.4 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|218|220|244|239|
|WPA2-AES|215|217|242|238|
|WPA3-SAE|216|216|241|239|

STA Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|116|117|125|124|
|WPA2-AES|115|115|125|122|
|WPA3-SAE|115|115|124|124|

STA Mode Throughput - AX Mode|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|222|220|242|248|
|WPA2-AES|219|219|242|248|
|WPA3-SAE|221|219|244|246|


STA Mode Throughput - AX Mode|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|410|447|461|505|
|WPA2-AES|407|448|461|503|
|WPA3-SAE|407|449|461|504|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|57|57|62|62|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|122|124|130|130|

P2P - GO Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|329|342|371|378|

P2P - GO Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|400|398|461|487|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|57|58|62|62|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|120|122|129|131|

P2P - GC Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|336|329|373|381|

P2P - GC Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|401|418|457|499|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|58|57|62|62|
|WPA2-AES|57|57|62|62|
|WPA3-SAE|57|57|62|62|


Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|123|120|128|127|
|WPA2-AES|122|119|127|126|
|WPA3-SAE|123|118|128|125|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|58|58|63|63|
|WPA2-AES|58|57|62|62|
|WPA3-SAE|58|57|62|61|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|129|125|133|133|
|WPA2-AES|128|124|133|132|
|WPA3-SAE|128|125|133|133|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|70|68|73|73|
|WPA2-AES|70|68|73|73|
|WPA3-SAE|61|68|73|72|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|170|173|174|176|
|WPA2-AES|170|166|173|176|
|WPA3-SAE|170|165|173|177|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|355|344|367|384|
|WPA2-AES|353|341|366|381|
|WPA3-SAE|342|343|366|380|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|113|110|124|125|
|WPA2-AES|110|109|122|122|
|WPA3-SAE|112|110|124|123|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|218|219|240|243|
|WPA2-AES|215|214|239|241|
|WPA3-SAE|217|215|240|240|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|114|113|126|125|
|WPA2-AES|112|110|125|124|
|WPA3-SAE|111|111|125|122|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|228|226|243|254|
|WPA2-AES|229|225|243|254|
|WPA3-SAE|227|226|243|254|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|404|423|451|500|
|WPA2-AES|402|419|449|499|
|WPA3-SAE|402|416|449|500|

<a name="open-thread-throughput-test"></a>
## OpenThread throughput test

- Environment: closed
- DUT: NXP reference board with 8MMINILPD4-EVKB platform
- Clock rate: 10 MHz
- DUT TX Power: 0 dBm
- OTREF TX Power: 20 dBm
-	Iperf commands:
- iperf -s -u -p 9006 -i 1 -V -B fd00:db8:0:0:7c32:a842:d468:e38a
- iperf -c fd00:db8:0:0:7c32:a842:d468:e38a -p 9006 -u -i 1 -b 250K -t 30 -l 1000 -B fd00:db8:0:0:291e:229a:b49:5eec -V

|Protocol|TCP \(kbit/s\)||UDP \(kbit/s\)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Thread leader|81|81|90|90|
|Thread child|82|81|90|90|
