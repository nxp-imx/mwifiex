[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)
- [STA throughput](#sta-throughput)
- [P2P-GO throughput](#p2pgo-throughput)
- [P2P-GC throughput](#p2pgc-throughput)
- [Mobile AP throughput](#mobile-ap-throughput)

<a name="throughput-test-setup"></a>
## Throughput test setup {#section_test .section}

-   Environment: Shield Room - Over the Air
-   External Access Point: Asus RT-AX88U \(FW-3.0.0.4.386\_41700\)
-   DUT: 88W8987-Murata \(**Module: LBEE5QD1ZM**\) with MCIMX8M-EVK platform
    -   Driver load parameters:

`cal_data_cfg=none, cfg80211_wext=0xf, host_mlme=1, amsdu_deaggr=1, net_rx=1, tx_skb_clone=1, tx_work=1`

-   iPerf commands:

TCP server:

```
#iperf -s -i1 -fm -w 2M
```

TCP client:

```
# iperf -c <ip_address> -i1 -fm -w 2M -t60 -P5
```

UDP server:

```
# iperf -s -u -i1 -fm -w 2M
```

UDP client:

```
# iperf -c <ip_address> -i1 -fm -w 2M -t60 -b 800 -P5
```

**Note:** You can add iPerf parameters like TCP window size or parallel streams for higher throughput. The above-described iPerf parameters are an example.

-   External Client: NXP 88W8987 SD-UART
-   Channel: 6 – 36

<a name="sta-throughput"></a>
## STA throughput

External AP: Asus RT-AX88U

STA Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|56|59|59|60|
|WPA2-AES|55|59|59|61|
|WPA3-SAE|56|59|59|60|

STA Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|56|62|61|63|
|WPA2-AES|57|62|61|63|
|WPA3-SAE|57|62|61|63|

STA Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|117|131|126|134|
|WPA2-AES|116|130|125|134|
|WPA3-SAE|115|131|125|134|

STA Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|68|72|73|75|
|WPA2-AES|68|72|73|74|
|WPA3-SAE|68|72|73|74|

STA Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|156|168|168|174|
|WPA2-AES|152|166|167|170|
|WPA3-SAE|153|168|166|174|

STA Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|262|348|288|361|
|WPA2-AES|249|344|269|351|
|WPA3-SAE|242|346|269|357|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode – 2.4 GHz Band – 20MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|57|59|62|63|

P2P - GO Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|214|310|250|375|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|54|58|63|63|

P2P - GC Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|236|314|261|379|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput {#section_mobile-ap .section}

External client: NXP 88W8987 SD-UART

Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|56|57|59|62|
|WPA2-AES|55|57|59|61|
|WPA3-SAE|56|56|59|62|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|58|59|61|64|
|WPA2-AES|57|59|61|64|
|WPA3-SAE|58|59|61|64|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|117|127|126|135|
|WPA2-AES|117|126|125|135|
|WPA3-SAE|116|126|125|135|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|70|70|74|75|
|WPA2-AES|70|68|74|74|
|WPA3-SAE|70|70|74|74|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|149|168|162|180|
|WPA2-AES|147|168|168|178|
|WPA3-SAE|147|168|161|180|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|242|347|273|388|
|WPA2-AES|219|312|258|379|
|WPA3-SAE|220|318|258|380|

**Parent page:**[SD-UART 88W8987](../topics/sd-uart_88w8987.md)

