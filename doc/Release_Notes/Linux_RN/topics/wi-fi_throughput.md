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
|Open Security|114|119|123|127|
|WPA2-AES|114|118|123|125|
|WPA3-SAE|113|118|123|124|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|116|123|126|129|
|WPA2-AES|114|123|126|128|
|WPA3-SAE|114|122|126|127|

STA Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|219|253|254|265|
|WPA2-AES|219|250|254|259|
|WPA3-SAE|218|250|254|263|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|20 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|332|340|360|358|
|WPA2-AES|328|336|358|356|
|WPA3-SAE|326|338|358|356|

STA Mode Throughput - AC Mode|MAC1|5 GHz Band|40 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|332|340|360|358|
|WPA2-AES|328|336|358|356|
|WPA3-SAE|326|338|358|356|


STA Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|698|688|747|752|
|WPA2-AES|693|688|743|749|
|WPA3-SAE|693|685|744|748|

STA Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|183|188|192|191|
|WPA2-AES|181|188|192|192|
|WPA3-SAE|181|187|192|194|


STA Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|229|228|250|246|
|WPA2-AES|221|229|250|246|
|WPA3-SAE|220|231|250|246|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|456|462|498|506|
|WPA2-AES|450|458|496|499|
|WPA3-SAE|449|454|496|498|

STA Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|835|892|914|953|
|WPA2-AES|818|876|901|944|
|WPA3-SAE|817|871|902|946|


## P2P-GO throughput
<a name="p2p-go-throughput"></a>

P2P - GO Mode Throughput - BGN Mode|MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|125|125|129|130|

P2P - GO Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|251|250|260|260|

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
|WPA2-AES|125|124|129|129|

P2P - GC Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|250|249|258|259|

P2P - GC Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|WPA2-AES|706|715|745|749|

## Mobile AP throughput
<a name="mobile-ap-throughput"></a>
External client: NXP IW620 PCIe-UART

Mobile AP Mode Throughput - BGN Mode| MAC2|2.4 GHz Band|20 MHz

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|123|123|127|128|
|WPA2-AES|123|123|127|128|
|WPA3-SAE|123|121|127|128|


Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|20 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|125|125|130|130|
|WPA2-AES|125|125|129|130|
|WPA3-SAE|125|125|129|130|

Mobile AP Mode Throughput - AN Mode|MAC1|5 GHz Band|40 MHz (HT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|251|251|260|262|
|WPA2-AES|251|251|260|261|
|WPA3-SAE|251|251|260|261|

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
|Open Security|346|335|360|356|
|WPA2-AES|345|338|358|352|
|WPA3-SAE|344|334|358|351|

Mobile AP Mode Throughput - AC Mode|MAC1|5 GHz Band|80 MHz (VHT)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|707|719|745|750|
|WPA2-AES|703|716|740|745|
|WPA3-SAE|703|717|734|745|

Mobile AP Mode Throughput - AX Mode|MAC2|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|188|187|194|195|
|WPA2-AES|187|187|194|194|
|WPA3-SAE|187|187|194|194|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|20 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|246|248|255|257|
|WPA2-AES|246|248|254|257|
|WPA3-SAE|246|248|254|257|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|40 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|483|476|501|505|
|WPA2-AES|480|478|498|496|
|WPA3-SAE|481|475|498|500|

Mobile AP Mode Throughput - AX Mode|MAC1|5 GHz Band|80 MHz (HE)

|Protocol|TCP \(Mbit/s\)|TCP \(Mbit/s\) |UDP \(Mbit/s\)|UDP \(Mbit/s\)|
|--------|--------------|---------------|--------------|--------------|
|Direction|TX|RX|TX|RX|
|Open Security|869|953|927|993|
|WPA2-AES|861|951|917|989|
|WPA3-SAE|864|950|917|993|