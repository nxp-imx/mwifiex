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
- DUT: 88W8997-Murata M.2 \(Module: LBEE5XV1YM\) with MCIMX8M-EVK platform
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

- External Access Point: NXP 88W9098 PCIe-UART
- External Client: NXP 88W9098 PCIe-UART
- Channel: 6 \| 36
<a name="sta-throughput"></a>
## STA throughput

External AP: NXP 88W9098 PCIe-UART

STA Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|109|119|120|124|
|WPA2-AES|114|118|121|123|
|WPA3-SAE|112|118|119|123|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz ( HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|116|125|125|130|
|WPA2-AES|115|125|125|130|
|WPA3-SAE|116|125|125|129|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|212|249|240|260|
|WPA2-AES|211|248|240|260|
|WPA3-SAE|211|251|240|260|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz ( VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|136|147|147|152|
|WPA2-AES|136|147|147|152|
|WPA3-SAE|135|147|147|152|

STA Mode Throughput - AC Mode|5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|307|337|342|360|
|WPA2-AES|306|335|342|358|
|WPA3-SAE|303|336|342|358|

STA Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|394|459|474|490|
|WPA2-AES|397|433|475|484|
|WPA3-SAE|394|446|475|490|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput
P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20MHz|1SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|111|117|119|125|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz|2SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|213|244|237|259|

P2P - GO Mode Throughput - AC Mode|5 GHz Band|80 MHz|2SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|417|448|468|490|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20MHz|1SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|118|110|124|119|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz|2SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|209|246|237|257|

P2P - GC Mode Throughput - AC Mode|5 GHz Band|80 MHz|2SS

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|420|445|470|488|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External Client: NXP 88W9098 PCIe-UART

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|107|116|115|124|
|WPA2-AES|110|118|117|126|
|WPA3-SAE|112|119|118|127|

Mobile AP Mode Throughput - BGN Mode|2.4 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|196|230|222|250|
|WPA2-AES|196|231|221|250|
|WPA3-SAE|196|230|221|251|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|116|120|125|129|
|WPA2-AES|116|119|125|129|
|WPA3-SAE|116|119|124|130|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|214|246|238|260|
|WPA2-AES|214|245|238|260|
|WPA3-SAE|214|246|238|260|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|137|141|146|152|
|WPA2-AES|137|141|146|152|
|WPA3-SAE|137|141|146|152|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|298|327|338|359|
|WPA2-AES|296|324|338|356|
|WPA3-SAE|296|323|338|356|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|393|456|472|493|
|WPA2-AES|384|444|471|489|
|WPA3-SAE|387|428|471|489|