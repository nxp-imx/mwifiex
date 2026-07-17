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
- DUT: Murata M.2 Module LBES5PL2EL with MCIMX8M-EVK platform
    - Driver load parameters:

`cal_data_cfg=none, cfg80211_wext=0xf, host_mlme=1, amsdu_deaggr=1, net_rx=1, tx_skb_clone=1, tx_work=1`

- iPerf commands:

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

- External Access Point: Asus RT-AX88U \(FW-3.0.0.4.386\_49674\)
- External Client: NXP IW620 PCIe-UART
- Channel: 6 – 36

<a name="sta-throughput"></a>
## STA throughput

External AP: Asus RT-AX88U

STA Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|56|59|60|62|
|WPA2-AES|55|58|60|61|
|WPA3-SAE|55|58|60|61|

STA Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \( HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|57|59|62|63|
|WPA2-AES|57|59|62|63|
|WPA3-SAE|57|59|62|63|

STA Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|124|124|133|132|
|WPA2-AES|123|123|133|131|
|WPA3-SAE|123|122|133|131|

STA Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|70|75|75|78|
|WPA2-AES|69|74|74|78|
|WPA3-SAE|69|74|74|78|

STA Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|167|170|180|177|
|WPA2-AES|165|168|179|174|
|WPA3-SAE|165|164|179|168|

STA Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|321|352|370|390|
|WPA2-AES|317|364|368|389|
|WPA3-SAE|319|364|368|388|

STA Mode Throughput - AX Mode - MAC1 - 2.4 GHz Band - 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|111|114|124|122|
|WPA2-AES|109|113|122|122|
|WPA3-SAE|109|113|120|121|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|113|120|124|129|
|WPA2-AES|112|119|123|128|
|WPA3-SAE|113|120|123|128|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|224|232|248|258|
|WPA2-AES|223|232|246|257|
|WPA3-SAE|225|231|249|256|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|409|424|452|508|
|WPA2-AES|406|426|451|507|
|WPA3-SAE|407|426|451|508|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|57|56|60|61|

P2P - GO Mode Throughput - AN Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|122|125|123|132|

P2P - GO Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|327|3292|356|261|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - AN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|59|56|62|61|

P2P - GC Mode Throughput - AN Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|122|125|129|134|

P2P - GC Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|327|226|353|246|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External Client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 20MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|57|56|62|62|
|WPA2-AES|57|56|61|62|
|WPA3-SAE|56|55|62|62|

Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|113|114|129|126|
|WPA2-AES|113|113|128|124|
|WPA3-SAE|113|112|126|125|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|59|56|61|59|
|WPA2-AES|58|57|60|59|
|WPA3-SAE|58|57|58|58|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|128|125|132|133|
|WPA2-AES|128|124|132|133|
|WPA3-SAE|128|125|132|133|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|67|69|72|75|
|WPA2-AES|67|69|72|75|
|WPA3-SAE|67|69|72|75|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|172|165|178|176|
|WPA2-AES|170|165|176|176|
|WPA3-SAE|170|166|175|176|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|341|363|378|382|
|WPA2-AES|337|361|377|381|
|WPA3-SAE|338|362|377|381|

Mobile AP Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|111|110|119|178|
|WPA2-AES|110|108|118|118|
|WPA3-SAE|107|109|119|117|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|114|110|126|127|
|WPA2-AES|112|109|124|126|
|WPA3-SAE|110|108|122|127|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|230|238|245|255|
|WPA2-AES|231|236|244|253|
|WPA3-SAE|230|237|243|255|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|419|416|467|500|
|WPA2-AES|414|417|463|499|
|WPA3-SAE|415|414|460|500|

**Parent page:** [SD-UART IW611/IW612](../topics/sd-uart_iw612.md)

