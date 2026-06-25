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
|Open Security|117|118|126|126|
|WPA2-AES|116|118|125|124|
|WPA3-SAE|116|118|126|123|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|116|120|125|128|
|WPA2-AES|116|120|125|127|
|WPA3-SAE|116|120|125|127|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|222|251|252|262|
|WPA2-AES|227|249|251|260|
|WPA3-SAE|226|248|251|260|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|142|147|149|156|
|WPA2-AES|142|148|149|155|
|WPA3-SAE|142|148|149|155|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|321|338|352|358|
|WPA2-AES|315|331|350|356|
|WPA3-SAE|316|335|349|355|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|362|364|438|415|
|WPA2-AES|359|363|443|405|
|WPA3-SAE|359|359|442|400|

STA Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|183|184|195|196|
|WPA2-AES|181|184|195|195|
|WPA3-SAE|182|184|195|197|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|219|222|241|256|
|WPA2-AES|219|222|241|244|
|WPA3-SAE|219|222|241|253|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|336|324|392|370|
|WPA2-AES|335|328|385|379|
|WPA3-SAE|314|335|385|390|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|361|355|428|410|
|WPA2-AES|360|356|428|412|
|WPA3-SAE|362|361|429|410|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|124|124|128|129|

P2P - GO Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|249|248|258|260|

P2P - GO Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|370|348|418|369|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|124|123|128|128|

P2P - GC Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|249|246|256|258|

P2P - GC Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|369|373|440|412|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP 88W9098 PCIe-UART

Mobile AP Mode Throughput - BGN Mode| MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|122|123|127|127|
|WPA2-AES|123|122|126|127|
|WPA3-SAE|122|122|126|127|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|124|124|128|129|
|WPA2-AES|124|124|128|129|
|WPA3-SAE|124|124|128|129|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|249|248|257|258|
|WPA2-AES|249|248|257|258|
|WPA3-SAE|249|248|257|258|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|145|149|151|155|
|WPA2-AES|145|149|151|154|
|WPA3-SAE|145|149|151|154|

Mobile AP Mode Throughput - AC Mode| MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|329|340|348|348|
|WPA2-AES|328|333|347|344|
|WPA3-SAE|328|338|347|350|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|370|342|440|415|
|WPA2-AES|368|362|440|415|
|WPA3-SAE|372|375|439|416|

Mobile AP Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|187|181|193|193|
|WPA2-AES|187|181|193|193|
|WPA3-SAE|186|182|193|193|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|248|238|256|254|
|WPA2-AES|246|239|256|253|
|WPA3-SAE|246|240|255|253|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|351|345|399|388|
|WPA2-AES|352|343|400|386|
|WPA3-SAE|352|353|399|375|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|377|365|426|387|
|WPA2-AES|371|369|427|386|
|WPA3-SAE|371|363|426|385|