[Link to index page](../index.md)

# Wi-Fi throughput

- [Throughput test setup](#throughput-test-setup)
- [STA throughput](#sta-throughput)

- [P2P-GO throughput](#p2p-go-throughput)

- [P2P-GC throughput](#p2p-gc-throughput)

- [Mobile AP throughput](#mobile-ap-throughput)

<a name="throughput-test-setup"></a>
## Throughput test setup

-   Environment: Shield Room - Over the Air
-   External Access Point: Netgear RAX120 \(FW\-1.0.1.122\)
-   DUT: Murata 88Q9098 M.2 \(Module: LBEE6ZZ1\) with MCIMX8M-EVK platform
    - Driver Load Parameters:
      cal_data_cfg=none, cfg80211_wext=0xf, host_mlme=1, amsdu_deaggr=1, net_rx=1, tx_skb_clone=1, tx_work=1

    - iPerf commands:

        -   TCP server
        ```
        # iperf -s -i1 -fm -w 2M
        ```

        -   TCP client
        ```
        # iperf -c <ip_address> -i1 -fm -w 2M -t60 -P5
        ```

        -   UDP server
        ```
        # iperf -s -u -i1 -fm -w 2M
        ```

        -   UDP client
        ```
        # iperf -c <ip_address> -i1 -fm -w 2M -t60 -b 800 -P5
        ```

        **Note:** For optimized throughput, add iPerf parameters such as TCP window size and parallel streams. The above-described iPerf parameters are an example.

-   External Client: NXP IW620 PCIe-UART
-   Channel: 6 \| 36

## STA throughput
<a name="sta-throughput"></a>

External Access Point: Netgear RAX120

STA Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|114|120|123|127|
|WPA2-AES|115|121|122|127|
|WPA3-SAE|116|123|123|126|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|116|123|126|132|
|WPA2-AES|113|123|126|128|
|WPA3-SAE|114|123|126|128|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|221|252|254|265|
|WPA2-AES|220|250|254|262|
|WPA3-SAE|219|250|254|263|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|142|149|150|158|
|WPA2-AES|140|147|150|157|
|WPA3-SAE|141|148|150|154|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|331|338|359|348|
|WPA2-AES|327|337|357|356|
|WPA3-SAE|324|337|357|356|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|694|687|746|753|
|WPA2-AES|691|686|742|749|
|WPA3-SAE|691|683|742|748|

STA Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|184|188|195|198|
|WPA2-AES|182|188|195|197|
|WPA3-SAE|182|188|195|197|


STA Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|227|223|250|239|
|WPA2-AES|223|218|249|241|
|WPA3-SAE|220|226|250|238|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|456|463|497|498|
|WPA2-AES|450|457|494|503|
|WPA3-SAE|453|461|494|503|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|824|889|903|950|
|WPA2-AES|812|886|896|949|
|WPA3-SAE|817|885|898|948|


## P2P-GO throughput
<a name="p2p-go-throughput"></a>

P2P - GO Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|121|121|125|125|

P2P - GO Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|251|248|258|260|

P2P - GO Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|701|700|730|741|

## P2P-GC throughput
<a name="p2p-gc-throughput"></a>

P2P - GC Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|125|121|129|126|

P2P - GC Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|250|244|258|256|

P2P - GC Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|710|694|744|745|

## Mobile AP throughput
<a name="mobile-ap-throughput"></a>
External client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode| MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|123|123|127|128|
|WPA2-AES|123|123|127|128|
|WPA3-SAE|123|123|127|127|


Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|125|124|129|129|
|WPA2-AES|124|124|129|129|
|WPA3-SAE|124|124|129|129|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|250|249|258|260|
|WPA2-AES|251|249|258|260|
|WPA3-SAE|251|249|258|260|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|148|150|152|156|
|WPA2-AES|147|150|152|156|
|WPA3-SAE|147|150|152|156|

Mobile AP Mode Throughput - AC Mode| MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|347|335|359|350|
|WPA2-AES|345|341|358|356|
|WPA3-SAE|344|341|358|356|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|704|692|740|737|
|WPA2-AES|699|687|739|728|
|WPA3-SAE|702|707|739|737|

Mobile AP Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|189|187|194|194|
|WPA2-AES|188|187|194|194|
|WPA3-SAE|188|187|194|194|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|248|249|255|257|
|WPA2-AES|247|248|255|256|
|WPA3-SAE|248|248|255|257|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|482|471|500|505|
|WPA2-AES|480|468|497|502|
|WPA3-SAE|480|468|497|500|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|868|942|923|997|
|WPA2-AES|860|938|912|992|
|WPA3-SAE|858|938|912|992|