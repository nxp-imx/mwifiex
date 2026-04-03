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
- DUT: 88W9098-Murata \(Module: **LBEE5ZZ1XL**\) with MCIMX8M-EVK platform
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
- External Client: NXP 88W9098 SD-UART
- Channel: 6 – 36

<a name="sta-throughput"></a>
## STA throughput

External AP: Netgear RAX200

STA Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|114|120|124|122|
|WPA2-AES|112|116|122|121|
|WPA3-SAE|110|117|121|120|

STA Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \( HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|117|124|128|130|
|WPA2-AES|118|124|127|127|
|WPA3-SAE|117|123|126|128|

STA Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|215|248|255|254|
|WPA2-AES|216|247|255|254|
|WPA3-SAE|217|248|255|254|

STA Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|143|141|156|151|
|WPA2-AES|143|149|156|150|
|WPA3-SAE|139|148|149|155|

STA Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|322|300|337|308|
|WPA2-AES|321|294|336|317|
|WPA3-SAE|315|299|335|317|

STA Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|356|320|401|328|
|WPA2-AES|355|318|402|326|
|WPA3-SAE|357|317|400|328|

STA Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|165|175|190|191|
|WPA2-AES|166|177|190|191|
|WPA3-SAE|165|177|190|191|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|200|204|214|204|
|WPA2-AES|199|202|220|200|
|WPA3-SAE|197|197|212|199|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|342|275|370|297|
|WPA2-AES|301|330|359|381|
|WPA3-SAE|299|329|362|381|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|350|297|397|380|
|WPA2-AES|353|297|377|370|
|WPA3-SAE|365|300|360|370|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|118|114|120|122|

P2P - GO Mode Throughput - AN Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|246|241|257|258|

P2P - GO Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|379|337|367|386|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|111|114|122|123|

P2P - GC Mode Throughput - AN Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|245|243|256|257|

P2P - GC Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|380|338|339|372|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External Client: NXP 88W9098 SD-UART

Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 20MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|117|113|121|120|
|WPA2-AES|118|114|120|119|
|WPA3-SAE|116|113|120|119|

Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|247|239|251|254|
|WPA2-AES|247|239|252|254|
|WPA3-SAE|247|239|252|254|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|124|119|128|129|
|WPA2-AES|124|118|128|128|
|WPA3-SAE|123|120|128|128|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|246|238|257|259|
|WPA2-AES|247|240|258|258|
|WPA3-SAE|246|239|256|259|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|145|141|150|150|
|WPA2-AES|146|140|150|149|
|WPA3-SAE|144|140|149|150|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|230|245|250|288|
|WPA2-AES|234|256|250|277|
|WPA3-SAE|233|250|250|278|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|255|285|250|300|
|WPA2-AES|254|280|250|298|
|WPA3-SAE|254|280|250|301|

Mobile AP Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|165|165|180|180|
|WPA2-AES|168|166|175|185|
|WPA3-SAE|169|167|176|178|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|241|230|237|239|
|WPA2-AES|231|231|238|243|
|WPA3-SAE|232|229|229|245|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|275|250|275|295|
|WPA2-AES|271|252|271|295|
|WPA3-SAE|269|241|277|298|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open security|297|295|293|328|
|WPA2-AES|291|298|298|291|
|WPA3-SAE|298|341|327|331|

**Parent page:** [SD-UART 88W9098](../topics/sd-uart_88w9098.md)

