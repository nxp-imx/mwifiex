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
|Open security|56|56|61|61|
|WPA2-AES|55|56|61|60|
|WPA3-SAE|56|56|60|60|

STA Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \( HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|60|57|62|62|
|WPA2-AES|59|56|62|62|
|WPA3-SAE|59|56|62|62|

STA Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|122|124|134|132|
|WPA2-AES|122|120|133|132|
|WPA3-SAE|121|123|134|132|

STA Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|70|71|76|76|
|WPA2-AES|69|71|75|76|
|WPA3-SAE|70|70|75|76|

STA Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|168|167|177|178|
|WPA2-AES|167|160|173|176|
|WPA3-SAE|164|162|176|175|

STA Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|328|342|368|386|
|WPA2-AES|326|340|365|384|
|WPA3-SAE|321|340|363|385|

STA Mode Throughput - AX Mode - MAC1 - 2.4 GHz Band - 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|114|110|125|124|
|WPA2-AES|114|110|124|122|
|WPA3-SAE|112|108|122|122|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|119|119|126|128|
|WPA2-AES|118|117|125|127|
|WPA3-SAE|118|116|126|125|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|230|226|252|256|
|WPA2-AES|228|224|250|255|
|WPA3-SAE|226|224|251|255|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|333|332|420|420|
|WPA2-AES|327|327|416|415|
|WPA3-SAE|324|324|414|419|

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
|Open security|55|56|61|61|
|WPA2-AES|54|56|61|61|
|WPA3-SAE|54|56|61|61|

Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|118|120|126|128|
|WPA2-AES|116|119|124|126|
|WPA3-SAE|119|117|125|125|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|57|57|62|61|
|WPA2-AES|57|57|61|61|
|WPA3-SAE|57|57|61|60|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|119|121|134|132|
|WPA2-AES|116|120|131|130|
|WPA3-SAE|115|116|130|129|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|73|69|75|71|
|WPA2-AES|72|70|75|71|
|WPA3-SAE|73|69|74|71|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|174|172|178|180|
|WPA2-AES|172|171|178|176|
|WPA3-SAE|171|171|176|179|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|335|329|369|376|
|WPA2-AES|330|326|368|372|
|WPA3-SAE|331|327|367|372|

Mobile AP Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|111|115|125|124|
|WPA2-AES|110|113|124|122|
|WPA3-SAE|109|110|122|120|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|116|119|125|130|
|WPA2-AES|115|116|124|126|
|WPA3-SAE|115|118|121|127|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|225|229|248|249|
|WPA2-AES|224|225|247|245|
|WPA3-SAE|226|224|245|242|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|331|334|417|421|
|WPA2-AES|318|332|414|414|
|WPA3-SAE|328|335|415|413|

**Parent page:** [SD-UART IW611/IW612](../topics/sd-uart_iw612.md)

