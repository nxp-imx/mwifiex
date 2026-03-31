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

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|114|120|124|122|
|WPA2-AES|112|116|122|121|
|WPA3-SAE|110|117|121|120|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|117|124|128|130|
|WPA2-AES|118|124|127|127|
|WPA3-SAE|117|123|126|128|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|215|248|255|254|
|WPA2-AES|216|247|255|254|
|WPA3-SAE|217|248|255|254|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|143|141|156|151|
|WPA2-AES|143|149|156|150|
|WPA3-SAE|139|148|149|155|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|292|332|346|345|
|WPA2-AES|287|327|345|346|
|WPA3-SAE|285|325|342|345|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|381|370|411|414|
|WPA2-AES|379|370|421|414|
|WPA3-SAE|378|370|415|407|

STA Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|165|175|190|191|
|WPA2-AES|166|177|190|191|
|WPA3-SAE|165|177|190|191|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|219|225|246|256|
|WPA2-AES|220|222|245|254|
|WPA3-SAE|221|221|245|255|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|300|330|358|380|
|WPA2-AES|301|330|359|381|
|WPA3-SAE|299|329|362|381|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|377|370|399|403|
|WPA2-AES|376|369|397|403|
|WPA3-SAE|375|366|400|410|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|123|117|129|130|

P2P - GO Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|234|217|256|258|

P2P - GO Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|371|326|418|376|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|123|118|129|129|

P2P - GC Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|234|215|257|256|

P2P - GC Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|365|317|418|359|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP 88W9098 PCIe-UART

Mobile AP Mode Throughput - BGN Mode| MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|122|119|127|128|
|WPA2-AES|120|119|126|128|
|WPA3-SAE|120|119|126|128|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|125|120|130|130|
|WPA2-AES|125|120|129|130|
|WPA3-SAE|125|122|129|130|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|247|245|257|262|
|WPA2-AES|247|245|258|262|
|WPA3-SAE|247|245|257|262|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|146|142|152|152|
|WPA2-AES|146|142|151|152|
|WPA3-SAE|146|143|152|152|

Mobile AP Mode Throughput - AC Mode| MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|321|331|353|359|
|WPA2-AES|321|330|351|357|
|WPA3-SAE|320|330|351|357|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|392|343|374|382|
|WPA2-AES|391|343|366|387|
|WPA3-SAE|391|343|361|390|

Mobile AP Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open security|165|165|180|180|
|WPA2-AES|168|166|175|185|
|WPA3-SAE|169|167|176|178|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|233|241|253|256|
|WPA2-AES|231|242|253|254|
|WPA3-SAE|232|242|253|255|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|341|342|382|372|
|WPA2-AES|340|342|384|374|
|WPA3-SAE|332|341|382|370|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|350|378|380|400|
|WPA2-AES|360|378|389|401|
|WPA3-SAE|361|378|389|400|