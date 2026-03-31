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
- DUT: u-blox JODY-W683 with MCIMX8M-EVK platform
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

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|119|125|127|127|
|WPA2-AES|119|123|127|125|
|WPA3-SAE|119|123|127|125|

STA Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|118|122|127|124|
|WPA2-AES|119|123|127|125|
|WPA3-SAE|117|122|127|123|

STA Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|223|254|254|263|
|WPA2-AES|227|251|254|253|
|WPA3-SAE|228|253|253|260|

STA Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|148|153|156|157|
|WPA2-AES|147|151|156|157|
|WPA3-SAE|147|152|156|157|

STA Mode Throughput - AC Mode|5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|336|346|359|348|
|WPA2-AES|334|345|357|346|
|WPA3-SAE|335|343|357|355|

STA Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|716|716|755|759|
|WPA2-AES|713|709|750|754|
|WPA3-SAE|712|710|751|755|

STA Mode Throughput - AX Mode|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|235|230|255|237|
|WPA2-AES|235|229|254|240|
|WPA3-SAE|234|231|253|234|

STA Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|239|229|257|248|
|WPA2-AES|237|229|257|248|
|WPA3-SAE|236|224|257|248|

STA Mode Throughput - AX Mode|5 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|475|485|504|506|
|WPA2-AES|472|482|501|489|
|WPA3-SAE|472|483|501|496|

STA Mode Throughput - AX Mode|5 GHz Band|80 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|931|872|957|953|
|WPA2-AES|927|916|957|953|
|WPA3-SAE|919|880|947|953|

STA Mode Throughput - AX Mode|6 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|236|246|255|254|
|WPA3-OWE|235|244|253|251|

STA Mode Throughput - AX Mode|6 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|471|473|500|490|
|WPA3-OWE|469|472|499|492|

STA Mode Throughput - AX Mode|6 GHz Band|80 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|927|867|957|953|
|WPA3-OWE|925|863|957|953|

<a name="p2pgo-throughput"></a>
## P2P-GO throughput

P2P - GO Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|124|125|128|129|

P2P - GO Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|250|250|260|260|

P2P - GO Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|705|702|751|746|

P2P - GO Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|909|863|1002|991|

<a name="p2pgc-throughput"></a>
## P2P-GC throughput

P2P - GC Mode Throughput - BGN Mode|2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|125|125|128|127|

P2P - GC Mode Throughput - AN Mode|5 GHz Band|40 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|251|250|260|260|

P2P - GC Mode Throughput - AC Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|684|711|748|751|

P2P - GC Mode Throughput - AX Mode|5 GHz Band|80 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA2-AES|916|856|1001|963|

<a name="mobile-ap-throughput"></a>
## Mobile AP throughput

External client: NXP AW693 PCIe-UART

Mobile AP Mode Throughput - BGN Mode| 2.4 GHz Band|20 MHz

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|120|125|125|128|
|WPA2-AES|118|123|126|127|
|WPA3-SAE|118|123|125|127|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|20 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|122|125|128|130|
|WPA2-AES|122|126|128|130|
|WPA3-SAE|122|126|128|130|

Mobile AP Mode Throughput - AN Mode|5 GHz Band|40 MHz (HT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|247|253|258|260|
|WPA2-AES|246|253|258|260|
|WPA3-SAE|246|252|258|260|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|20 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|147|152|153|156|
|WPA2-AES|147|151|153|156|
|WPA3-SAE|147|151|153|156|

Mobile AP Mode Throughput - AC Mode| 5 GHz Band|40 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|342|346|357|358|
|WPA2-AES|339|337|355|349|
|WPA3-SAE|337|343|354|356|

Mobile AP Mode Throughput - AC Mode|5 GHz Band|80 MHz (VHT)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|702|725|741|751|
|WPA2-AES|693|720|732|745|
|WPA3-SAE|696|720|738|748|

Mobile AP Mode Throughput - AX Mode|2.4 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|231|244|241|253|
|WPA2-AES|220|243|240|252|
|WPA3-SAE|219|243|241|252|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|20 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|243|247|254|256|
|WPA2-AES|241|249|254|256|
|WPA3-SAE|240|248|254|253|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|475|483|493|504|
|WPA2-AES|461|477|499|500|
|WPA3-SAE|464|483|490|501|

Mobile AP Mode Throughput - AX Mode|5 GHz Band|80 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|Open Security|926|850|992|956|
|WPA2-AES|920|864|988|965|
|WPA3-SAE|923|886|989|949|

Mobile AP Mode Throughput - AX Mode|6 GHz Band|20 MHz (HE)
|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|239|247|251|255|
|WPA3-OWE|238|246|251|254|

Mobile AP Mode Throughput - AX Mode|6 GHz Band|40 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|466|479|492|487|
|WPA3-OWE|466|477|492|496|

Mobile AP Mode Throughput - AX Mode|6 GHz Band|80 MHz (HE)

|Protocol|TCP (Mbit/s)||UDP (Mbit/s)||
|---------|--|--|--|--|
|Direction|TX|RX|TX|RX|
|WPA3-SAE|908|856|962|960|
|WPA3-OWE|904|862|976|956|