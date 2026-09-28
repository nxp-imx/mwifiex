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
- External Access Point: Netgear RAXE500
- DUT: Silex SX-SDMAX6E with i.MX93-EVK platform
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

- External Client: NXP AW693 PCIe-UART
- Channel: 6 \| 36

<a name="sta-throughput"></a>
## STA throughput

External Access Point: Netgear RAXE500

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|118|125|127|128|
|WPA2-AES|118|123|127|127|
|WPA3-SAE|119|123|127|126|

STA Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|221|251|252|260|
|WPA2-AES|220|249|250|270|
|WPA3-SAE|222|248|251|256|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|120|126|127|131|
|WPA2-AES|119|125|127|129|
|WPA3-SAE|120|125|127|128|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|228|254|253|263|
|WPA2-AES|229|251|253|261|
|WPA3-SAE|227|251|253|260|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|147|153|156|158|
|WPA2-AES|147|152|155|154|
|WPA3-SAE|147|152|155|154|

STA Mode Throughput - AC Mode|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|336|346|354|357|
|WPA2-AES|333|344|353|355|
|WPA3-SAE|334|344|353|356|

STA Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|710|714|752|759|
|WPA2-AES|714|711|747|753|
|WPA3-SAE|714|710|747|755|

STA Mode Throughput - AX Mode|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|236|237|255|245|
|WPA2-AES|234|236|254|240|
|WPA3-SAE|235|237|253|246|

STA Mode Throughput - AX Mode|2.4 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|469|437|497|455|
|WPA2-AES|464|435|495|455|
|WPA3-SAE|466|436|495|454|

STA Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|238|243|256|250|
|WPA2-AES|238|240|255|253|
|WPA3-SAE|238|245|255|253|

STA Mode Throughput - AX Mode|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|474|486|502|507|
|WPA2-AES|472|481|499|504|
|WPA3-SAE|473|483|499|504|

STA Mode Throughput - AX Mode|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|939|932|957|953|
|WPA2-AES|922|932|957|953|
|WPA3-SAE|928|932|957|953|

STA Mode Throughput - AX Mode|6 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|238|247|256|258|

STA Mode Throughput - AX Mode|6 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|474|483|500|497|

STA Mode Throughput - AX Mode|6 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|930|933|957|950|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|122|123|128|127|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|249|246|259|258|

P2P - GO Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|710|709|747|745|

P2P - GO Mode Throughput - AX Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|245|247|253|255|

P2P - GO Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|943|934|996|997|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|124|124|128|128|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|250|249|258|259|

P2P - GC Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|710|713|745|747|

P2P - GC Mode Throughput - AX Mode|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|245|247|254|254|

P2P - GC Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|937|954|998|995|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP AW693 PCIe-UART

Mobile AP Mode Throughput - BGN Mode| 2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|119|121|127|126|
|WPA2-AES|118|121|127|125|
|WPA3-SAE|121|122|126|125|

Mobile AP Mode Throughput - BGN Mode| 2.4 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|243|240|254|251|
|WPA2-AES|242|240|253|249|
|WPA3-SAE|240|245|254|253|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|124|125|129|128|
|WPA2-AES|124|125|129|129|
|WPA3-SAE|124|125|129|129|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|248|250|259|245|
|WPA2-AES|247|251|259|254|
|WPA3-SAE|248|251|259|258|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|149|150|156|155|
|WPA2-AES|149|150|156|155|
|WPA3-SAE|149|150|156|155|

Mobile AP Mode Throughput - AC Mode| 5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|341|342|355|355|
|WPA2-AES|337|341|351|354|
|WPA3-SAE|330|340|350|354|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|713|711|752|745|
|WPA2-AES|711|707|747|745|
|WPA3-SAE|712|706|748|741|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|227|242|238|251|
|WPA2-AES|226|242|237|250|
|WPA3-SAE|226|242|237|250|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|424|467|445|488|
|WPA2-AES|425|463|445|486|
|WPA3-SAE|424|464|445|484|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|244|246|251|254|
|WPA2-AES|241|247|251|254|
|WPA3-SAE|240|248|253|255|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|479|479|503|499|
|WPA2-AES|477|478|498|496|
|WPA3-SAE|477|476|499|496|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|935|922|1000|986|
|WPA2-AES|936|920|996|990|
|WPA3-SAE|922|928|996|983|

Mobile AP Mode Throughput - AX Mode|6 GHz Band|20 MHz (HE)
|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|238|242|252|251|

Mobile AP Mode Throughput - AX Mode|6 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|464|472|484|493|

Mobile AP Mode Throughput - AX Mode|6 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|908|912|981|981|