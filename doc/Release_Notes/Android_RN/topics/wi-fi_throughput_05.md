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
|Open Security|105|108|115|120|
|WPA2-AES|107|110|116|119|
|WPA3-SAE|108|106|117|118|

STA Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \( HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|104|111|114|115|
|WPA2-AES|103|112|114|115|
|WPA3-SAE|105|113|114|115|

STA Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|182|224|228|243|
|WPA2-AES|183|216|228|244|
|WPA3-SAE|191|215|248|242|


STA Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|129|136|140|142|
|WPA2-AES|128|136|140|141|
|WPA3-SAE|129|136|140|142|


STA Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|257|291|306|307|
|WPA2-AES|259|291|308|303|
|WPA3-SAE|259|291|309|317|


STA Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|283|312|359|359|
|WPA2-AES|283|309|362|359|
|WPA3-SAE|282|312|356|361|

STA Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|170|144|183|160|
|WPA2-AES|171|147|183|161|
|WPA3-SAE|169|146|182|160|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|203|223|236|248|
|WPA2-AES|200|221|238|249|
|WPA3-SAE|201|220|238|245|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|290|300|371|334|
|WPA2-AES|290|301|360|332|
|WPA3-SAE|281|298|365|334|


STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|303|348|388|335|
|WPA2-AES|302|348|389|337|
|WPA3-SAE|302|350|390|338|


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
|Open Security|123|119|125|126|
|WPA2-AES|123|120|125|126|
|WPA3-SAE|123|120|125|126|


Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|247|239|251|254|
|WPA2-AES|247|239|252|254|
|WPA3-SAE|247|239|252|254|


Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|126|122|128|128|
|WPA2-AES|126|122|128|128|
|WPA3-SAE|126|122|128|128|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|252|243|256|258|
|WPA2-AES|252|244|256|258|
|WPA3-SAE|252|244|256|258|


Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|148|144|150|152|
|WPA2-AES|148|144|150|152|
|WPA3-SAE|148|144|157|152|


Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|328|320|347|322|
|WPA2-AES|329|318|348|314|
|WPA3-SAE|332|256|349|277|


Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|356|293|405|310|
|WPA2-AES|350|265|406|316|
|WPA3-SAE|350|293|406|283|


Mobile AP Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|184|184|190|191|
|WPA2-AES|184|183|190|192|
|WPA3-SAE|185|184|190|192|


Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|138|234|254|238|
|WPA2-AES|138|233|254|238|
|WPA3-SAE|138|234|255|238|


Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|163|291|380|168|
|WPA2-AES|166|267|380|191|
|WPA3-SAE|163|247|380|267|


Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|179|297|405|310|
|WPA2-AES|178|270|405|338|
|WPA3-SAE|179|345|408|284|

**Parent page:** [SD-UART 88W9098](../topics/sd-uart_88w9098.md)

