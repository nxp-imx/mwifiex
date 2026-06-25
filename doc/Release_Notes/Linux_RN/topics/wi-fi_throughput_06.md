[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)
- [STA throughput](#sta-throughput)
- [P2P-GO throughput](#p2pgo-throughput)
- [P2P-GC throughput](#p2pgc-throughput)
- [Mobile AP throughput](mobile-ap-throughput)

<a name="throughput-test-setup"></a>
## Throughput test setup

- Environment: Shield Room - Over the Air
- Access Point: Asus RT-AX88U \(FW-3.0.0.4.386\_41700\)
- DUT: IW416-Murata \(Module: LBEE5CJ1XK\) with MCIMX8M-EVK platform
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

- External Client: NXP 88W8997 PCIe-UART
- Channel: 6 \| 36

<a name="sta-throughput"></a>
## STA throughput

External AP: Asus RT-AX88U

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|53|56|62|58|
|WPA2-AES|51|58|61|60|
|WPA3-SAE|51|60|62|61|

STA Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|95|121|126|127|
|WPA2-AES|85|101|122|103|
|WPA3-SAE|85|99|122|104|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|53|62|61|65|
|WPA2-AES|53|62|61|64|
|WPA3-SAE|53|62|61|64|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|97|124|124|127|
|WPA2-AES|80|101|122|106|
|WPA3-SAE|83|101|122|104|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|57|61|62|65|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|115|122|125|127|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|62|60|64|65|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|114|123|122|128|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP 88W8997 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|56|62|61|64|
|WPA2-AES|56|60|61|64|
|WPA3-SAE|56|59|61|64|

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|114|121|123|129|
|WPA2-AES|113|124|118|128|
|WPA3-SAE|111|124|115|129|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|57|62|61|65|
|WPA2-AES|57|60|61|65|
|WPA3-SAE|57|61|61|65|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|115|128|125|133|
|WPA2-AES|114|128|120|133|
|WPA3-SAE|114|129|120|133|