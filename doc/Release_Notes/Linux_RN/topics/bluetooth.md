[Link to the index page](../index.md)

# Bluetooth

-   [Bluetooth classic](#bluetooth-classic)
-   [Bluetooth LE](#bluetooth-le)

## Bluetooth classic
<a name="bluetooth-classic"> </a>
<!-- 9 columns -->
|Feature        |88W9098 PCIe-UART|88W8997 PCIe-UART|AW693 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW416 SDIO-UART|
|---------------|-------|-------|-----|-------|-----------|-------|-------|-----|
|**General features**|||||||||
|Bluetooth Class 1.5 and Class 2 support|Y|Y|Y|Y|Y|Y|Y|Y|
|Scatternet support|Y|Y|Y|Y|Y|Y|Y|Y|
|Maximum of seven simultaneous ACL connections - Central links|Y|Y|Y|Y|Y|Y|Y|Y|
|Automatic packet type selection|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth - 2.1 to 5.0 specification support|Y|Y|Y|Y|Y|Y|Y|Y|
|Low power sniff|Y|Y|Y|Y|Y|Y|Y|Y|
|Independent reset \(in-band and OOB<sup>3</sup> \)|Y|Y|Y|Y|Y|N|Y|Y|
|Wake on Bluetooth \(chip to host\)<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|Y|
|Deep sleep \(NXP UART driver\)|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth truncated paging|Y|Y|Y|Y|Y|Y|Y|Y|
|Erroneous data reporting|Y|Y|Y|Y|Y|Y|Y|Y|
|Encryption pause and resume|Y|Y|Y|Y|Y|Y|Y|Y|
|Extended inquiry response|Y|Y|Y|Y|Y|Y|Y|Y|
|Link supervision timeout changed event|Y|Y|Y|Y|Y|Y|Y|Y|
|Non-automatically flushable packet boundary flag|Y|Y|Y|Y|Y|Y|Y|Y|
|Sniff sub rating|Y|Y|Y|Y|Y|Y|Y|Y|
|Enhanced power control|Y|Y|Y|Y|Y|Y|Y|Y|
|HCI read encryption key size command|Y|Y|Y|Y|Y|Y|Y|Y|
|Standalone Bluetooth classic AES encryption|Y|N|Y|Y|Y|N|N|Y|
|Bluetooth classic AES + Bluetooth LE AES encryption|N|N|N|N|Y|N|N|N|
|Payload – 27bytes to 234 bytes|Y|Y|Y|Y|Y|Y|Y|Y|
|Enhancements to L2CAP for low energy|Y|Y|Y|Y|Y|Y|Y|Y|
|PCM loopback mode|Y|N|Y|Y|Y|N|Y|Y|
|Enhancements to GAP for low energy|Y|Y|Y|Y|Y|Y|Y|Y|
|SCO/eSCO over PCM|Y|Y|Y|Y|Y|Y|Y|Y|
|SCO/eSCO over HCI|N|N|N|N|N|N|N|Y|
|Dual SCO/eSCO|Y|N|Y|Y|Y|N|N|N|
|APCF feature support|Y|Y|Y|Y|Y|Y|Y|Y|
|Train nudging|N|N|N|N|Y|N|N|N|
|Generalized interlaced scan|Y|N|Y|Y|Y|N|N|N|
|BR/EDR secure connections|N|N|Y|N|Y|N|N|N|
|4M Baud data rate |Y|Y|Y|Y|Y|Y|Y|Y|
|**Bluetooth packet types supported**|||||||||
|ACL \(DM1, DH1, DM3, DH3, DM5, DH5, 2-DH1, 2-DH3, 2-DH5, 3-DH1, 3-DH3, 3-DH5\)|Y|Y|Y|Y|Y|Y|Y|Y|
|SCO \(HV1, HV3\)|Y|Y|Y|Y|Y|Y|Y|Y|
|eSCO \(EV3, EV4, EV5, 2EV3, 3EV3, 2EV5, 3EV5\)|Y|Y|Y|Y|Y|Y|Y|Y|
|**Bluetooth profiles supported**|||||||||
|A2DP source/sink|Y|Y|Y|Y|Y|Y|Y|Y|
|AVRCP target/controller|Y|Y|Y|Y|Y|Y|Y|Y|
|HFP dev|Y|Y|Y|Y|Y|Y|Y|Y|
|OPP server/client|Y|Y|Y|Y|Y|Y|Y|Y|
|SPP|Y|Y|Y|Y|Y|Y|Y|Y|
|HID|Y|Y|Y|Y|Y|Y|Y|Y|
|GAP|Y|Y|Y|Y|Y|Y|Y|Y|
|HFP AG<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|Y|
|PAN server/client<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|Y|
|PBAP server/client<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|Y|
|MAP server/client<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|Y|
|A2DP SNK + HFP DEV|Y|N|Y|Y|Y|N|N|N|
|A2DP SRC + HFP GW<sup>2</sup>|Y|N|Y|Y|Y|N|N|N|
|**Bluetooth dual profiles supported**|||||||||
|Dual A2DP \(two sources\)|Y|N|Y|Y|Y|N|Y|N|
|Dual A2DP \(one source + one sink\)|Y|N|Y|Y|N|N|Y|N|
|Dual HFP \(two NBS\) PCM|Y|N|Y|Y|Y|N|Y|N|
|Dual HFP \(two WBS\) PCM|Y|N|Y|Y|Y|N|N|N|
|Dual HFP \(two WBS + one NBS\) PCM|Y|N|Y|Y|Y|N|Y|N|
|**Bluetooth audio features**|||||||||
|PCM NBS central/peripheral|Y|Y|Y|Y|Y|Y|Y|Y|
|PCM WBS central/peripheral|Y|Y|Y|Y|Y|Y|Y|Y|
|AAC and LDAC audio codec support|Y|N|Y|Y|Y|N|N|N|
|**RF test mode**|||||||||
|RF test mode functionality|Y|Y|Y|Y|Y|Y|Y|Y|

<!-- 1 In-band independent reset \(IR\) can directly work with M.2 based modules on i.MX but OOB IR needs the external uSD muRata adaptor board with M.2 module. -->
2 Feature tested using Ubuntu 16 platform, not with i.MX platform.
3 Contact your support representative to use this feature.

## Bluetooth LE
 <a name="bluetooth-le"></a>

|Feature        |88W9098 PCIe-UART|88W8997 PCIe-UART|AW693 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW610 SDIO-UART|IW416 SDIO-UART|IW610 USB-USB|
|---------------|-------|-------|-----|-------|-----------|-------|-------|-----|-----|-----|
|**General features**|||||||||||
|Maximum 16 Bluetooth LE connections\(Central role\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Independent reset \(in-band and out-of-band\) <sup>2</sup>|Y|Y|Y|Y|Y|N|Y|Y|Y|Y|
|Wake on Bluetooth LE \(chip to host\)|Y|N|Y|Y|Y|N|Y|N|Y|N|
|Deep sleep \(NXP UART driver\)|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|Standalone Bluetooth LE AES encryption|Y|N|Y|Y|Y|N|N|N|Y|N|
|Bluetooth classic AES + Bluetooth LE AES encryption|N|N|N|N|Y|N|N|N|N|N|
|4M Baud data rate |Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|48KHz sampling rate via I2S for HFP  |N|N|Y|N|N|N|N|N|N|N|
|**Bluetooth profile support**|||||||||||
|Bluetooth LE GATT|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE HOGP|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE GAP|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**Bluetooth LE 4.0 support**|||||||||||
|Low Energy physical layer|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Low Energy link layer|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Enhancements to HCI for Low Energy|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Low Energy direct test mode|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE - 1Mbit/s support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**Bluetooth 4.1 support**|||||||||||
|Low duty cycle directed advertising|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Dual Mode Topology|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE privacy v1.1|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE link layer topology|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**Bluetooth 4.2 support**|||||||||||
|Bluetooth LE secure connection|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|Bluetooth LE link layer privacy v1.2|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE data length extension|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Link layer extended scanner filter policies|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**Bluetooth 5.0 support**|||||||||||
|Bluetooth LE 2 Mbps support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|High duty cycle directed advertising|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE multiple advertisement \(4, or 5**\***, or 6\*\*\) sets|Y|Y|Y|Y|Y\*\*|Y|Y**\***|||||||||||N|N|N|
|Bluetooth LE extended advertisement|Y|N|Y|Y|Y|N|N|Y|Y|Y|
|Bluetooth LE channel selection \<sup>2</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Bluetooth LE long range|Y|N|Y|Y|Y|N|N|Y|Y|Y|
|Bluetooth LE periodic advertisement|N|N|Y|N|Y|N|N|Y|Y|Y|
|**Bluetooth 5.2 support**|||||||||||
|Bluetooth LE power control|N|N|Y|N|Y|N|N|Y|Y|Y|
|Isochronous channel|N|N|Y|N|Y|N|N|N|N|N|
|**Bluetooth LE audio**|||||||||||
|LE Audio Broadcast Audio Source|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Broadcast Audio Sink|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Broadcast Group Validation|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Broadcast Audio Unfamed Mode|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Broadcast Audio Sequential Packing|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Broadcast Audio Mode-Mono/Stereo|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Public Broadcast Audio Source|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Audio Source|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Audio Sink|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Group Validation|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Audio PHY-1M/2M/Coded|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Audio Unfamed Mode|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Audio Sequential Packing|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Audio Mode-Mono/Stereo|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Encrypted Audio|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio ISO Interval for LE Audio 7.5ms 10ms 20ms 29ms|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Unicast Audio Source 2-CIS Stream|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio Bridging A2DP Sink + Unicast Audio Source|N|N|Y|N|Y|N|N|N|N|N|
|LE Audio GAF Layers \(BAP,CAP, CAS, CSIP, MCP, MCS, MICP, VCP, BASS, CCP\)|N|N|Y|N|Y|N|N|N|N|N|
|Bluetooth LE_Audio support for the 3 CIS stream|N|N|N|N|Y|N|N|N|N|N|
|Bluetooth LE_Audio_LE 2 CIS + HFP|N|N|N|N|Y|N|N|N|N|N|
|**RF test mode**|||||||||||
|RF Test Mode functionality|Y|Y|Y|Y|Y|Y|Y|N|Y|N|


<!-- 1 In-band independent reset \(IR\) can directly work with M.2 based modules on i.MX but OOB IR needs the external uSD muRata adaptor board with M.2 module.<br/> -->
2 Contact your support representative to use this feature.<br/>
<!-- 3 The firmware supports Bluetooth LE audio, which is validated using custom host stack \(not part of BSP\).-->



