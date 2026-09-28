[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)
- [STA throughput](#sta-throughput)
- [P2P-GO throughput](#p2pgo-throughput)
- [P2P-GC Throughput](#p2pgc-throughput)
- [Mobile AP Throughput](#mobile-ap-throughput)

<a name="throughput-test-setup"></a>
## Throughput test setup

- Environment: Shield Room - Over the Air
- External Access Point: Asus RT-AX88U \(FW-3.0.0.4.386.41700\)
- DUT: Murata 88Q9098 M.2 \(Module: LBEE5ZZ1XL\) with MCIMX8M-EVK platform
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

- External Client: NXP 88W9098 PCIe-UART
- Channel: 6 \| 36

<a name="sta-throughput"></a>
## STA throughput

External Access Point: Asus RT-AX88U

STA Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|114|124|124|128|
|WPA2-AES|113|123|123|129|
|WPA3-SAE|113|123|123|127|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|118|123|126|128|
|WPA2-AES|115|123|126|127|
|WPA3-SAE|116|122|126|127|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|221|251|254|265|
|WPA2-AES|233|249|254|263|
|WPA3-SAE|234|249|254|263|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|142|148|150|158|
|WPA2-AES|141|149|150|157|
|WPA3-SAE|141|148|150|155|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|315|337|353|358|
|WPA2-AES|317|331|351|356|
|WPA3-SAE|311|326|350|351|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|360|357|439|417|
|WPA2-AES|362|374|439|403|
|WPA3-SAE|356|382|438|416|

STA Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|183|181|194|197|
|WPA2-AES|181|180|195|196|
|WPA3-SAE|181|182|194|196|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|231|234|253|254|
|WPA2-AES|223|215|252|251|
|WPA3-SAE|225|216|249|245|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|335|360|397|383|
|WPA2-AES|330|348|398|384|
|WPA3-SAE|335|362|398|389|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|359|370|424|406|
|WPA2-AES|364|369|424|416|
|WPA3-SAE|357|365|425|403|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|125|125|129|130|

P2P - GO Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|247|250|260|261|

P2P - GO Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|248|249|260|261|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|124|120|129|127|

P2P - GC Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|247|244|258|256|

P2P - GC Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|380|396|437|427|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP 88W9098 PCIe-UART

Mobile AP Mode Throughput - BGN Mode| MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|123|123|127|128|
|WPA2-AES|123|123|127|128|
|WPA3-SAE|123|123|127|128|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|126|124|129|129|
|WPA2-AES|125|124|129|129|
|WPA3-SAE|124|124|129|129|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|250|249|258|260|
|WPA2-AES|251|250|258|260|
|WPA3-SAE|251|249|258|260|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|147|150|152|156|
|WPA2-AES|147|150|152|156|
|WPA3-SAE|147|150|152|156|

Mobile AP Mode Throughput - AC Mode| MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|324|338|353|352|
|WPA2-AES|323|325|351|356|
|WPA3-SAE|323|340|351|356|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|362|377|437|429|
|WPA2-AES|365|366|437|424|
|WPA3-SAE|372|366|436|425|

Mobile AP Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|187|187|194|195|
|WPA2-AES|187|186|194|194|
|WPA3-SAE|187|186|194|194|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|244|247|255|257|
|WPA2-AES|241|247|253|257|
|WPA3-SAE|247|246|254|257|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|358|343|395|393|
|WPA2-AES|355|347|395|390|
|WPA3-SAE|355|360|395|391|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|377|370|422|393|
|WPA2-AES|373|370|423|389|
|WPA3-SAE|372|366|422|390|