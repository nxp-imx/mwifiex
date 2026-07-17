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
- External Access Point: Asus RT-AX88U \(FW-3.0.0.4.386\_49674\)
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

- External Client: NXP 88W9098 PCIe-UART
- Channel: 6 – 36

<a name="sta-throughput"></a>
## STA throughput

External AP: Netgear RAX200

STA Mode Throughput - BGN Mode – 2.4 GHz Band – 20 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|116|123|128|122|
|WPA2-AES|107|121|115|127|
|WPA3-SAE|110|114|126|120|


STA Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|118|113|131|122|
|WPA2-AES|105|119|124|126|
|WPA3-SAE|114|112|130|119|


STA Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|224|217|259|254|
|WPA2-AES|206|225|252|258|
|WPA3-SAE|214|218|260|251|


STA Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|137|142|160|148|
|WPA2-AES|128|149|151|154|
|WPA3-SAE|133|141|156|147|

STA Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|325|323|371|352|
|WPA2-AES|336|318|361|353|
|WPA3-SAE|335|321|369|347|


STA Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|642|438|748|460|
|WPA2-AES|739|628|742|658|
|WPA3-SAE|738|627|740|660|

STA Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|181|177|185|184|
|WPA2-AES|178|174|182|176|
|WPA3-SAE|179|175|181|177|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|253|243|255|245|
|WPA2-AES|251|238|251|244|
|WPA3-SAE|250|239|252|244|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|495|473|495|479|
|WPA2-AES|429|472|479|475|
|WPA3-SAE|430|471|481|477|

STA Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|870|872|877|743|
|WPA2-AES|865|870|861|683|
|WPA3-SAE|867|871|863|685|

<a name="p2pgo-throughput-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode – 2.4 GHz Band – 20MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|WPA2-AES|117|112|121|117|

P2P - GO Mode Throughput - AN Mode – MAC1 – 5 GHz Band – 40 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|247|243|257|258|

P2P - GO Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|685|690|716|730|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode – 2.4 GHz Band – 20MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|112|113|118|116|

P2P - GC Mode Throughput - AN Mode – MAC1 – 5 GHz Band – 40 MHz 

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|245|246|257|256|

P2P - GC Mode Throughput - AC Mode – 5 GHz Band – 80 MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|WPA2-AES|687|687|722|729|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP 88W9098 PCIe-UART

Mobile AP Mode Throughput - BGN Mode – 2.4 GHz Band – 20MHz

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|119|112|122|120|
|WPA2-AES|109|118|114|127|
|WPA3-SAE|114|105|120|119|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 20 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|128|117|133|126|
|WPA2-AES|120|125|124|132|
|WPA3-SAE|126|114|130|123|

Mobile AP Mode Throughput - AN Mode – 5 GHz Band – 40 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|254|243|262|256|
|WPA2-AES|246|251|254|264|
|WPA3-SAE|253|244|261|258|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 20 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|148|142|155|150|
|WPA2-AES|141|149|147|156|
|WPA3-SAE|149|140|153|149|


Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 40 MHz \(VHT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|336|334|359|353|
|WPA2-AES|330|340|348|349|
|WPA3-SAE|344|332|356|351|

Mobile AP Mode Throughput - AC Mode – 5 GHz Band – 80 MHz \(HT\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|694|700|748|726|
|WPA2-AES|684|703|736|734|
|WPA3-SAE|683|698|738|724|

Mobile AP Mode Throughput - AX Mode – MAC2 – 2.4 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|175|164|181|180|
|WPA2-AES|174|166|179|195|
|WPA3-SAE|165|160|173|186|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 20 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|251|206|249|245|
|WPA2-AES|243|212|240|253|
|WPA3-SAE|249|204|247|242|

Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 40 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|472|432|504|443|
|WPA2-AES|461|444|495|484|
|WPA3-SAE|469|430|501|427|


Mobile AP Mode Throughput - AX Mode – MAC1 – 5 GHz Band – 80 MHz \(HE\)

|Protocol|TCP \(Mbit/s\)| |UDP \(Mbit/s\)| |
|--------|--------------|---|--------------|---|
|Direction|TX|RX|TX|RX|
|Open Security|862|786|817|810|
|WPA2-AES|820|754|808|818|
|WPA3-SAE|854|756|811|809|

**Parent page:** [PCIe-UART 88W9098](../topics/pcie-uart_88w9098.md)

