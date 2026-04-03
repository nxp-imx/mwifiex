[Link to index page](../index.md)

# Bluetooth

- [Bluetooth classic](#bluetooth-classic)
- [Bluetooth LE](#bluetooth-le)

<a name="bluetooth-classic"></a>
## Bluetooth classic

Feature list for Bluetooth radio

|Features|88W9098 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8987 SDIO-UART|
|--------|-----------------|-----------------|---------------------|-----------------|
|**General features**||||||||||||||||||
|Bluetooth Class 1.5 and Class 2 support|Y|Y|Y|Y|
|Scatternet support|Y|Y|Y|Y|
|Maximum of seven simultaneous ACL connections – central links|Y|Y|Y|Y|
|Automatic Packet Type Selection|Y|Y|Y|Y|
|Bluetooth - 2.1 to 5.0 Specification Support|Y|Y|Y|Y|
|Independent reset \(In-band &amp; OOB\) <sup>1</sup> <sup>2</sup>|Y|Y|Y|Y|
|Low power sniff|Y|Y|Y|Y|
|Bluetooth Truncated Paging|Y|Y|Y|Y|
|Erroneous Data Reporting|Y|Y|Y|Y|
|Encryption Pause and Resume|Y|Y|Y|Y|
|Extended Inquiry Response|Y|Y|Y|Y|
|Link Supervision Timeout Changed Event|Y|Y|Y|Y|
|Non-Automatically-Flushable Packet Boundary Flag|Y|Y|Y|Y|
|Sniff Sub rating|Y|Y|Y|Y|
|AES Encryption<sup>1</sup>|Y|Y|N|Y|
|Enhanced Power Control|Y|Y|Y|Y|
|HCI Read Encryption Key Size command|Y|Y|Y|Y|
|Payload – 27bytes to 234 bytes|Y|Y|Y|Y|
|Enhancements to L2CAP for Low Energy|Y|Y|Y|Y|
|PCM Loopback Mode|Y|Y|Y|Y|
|Enhancements to GAP for Low Energy|Y|Y|Y|Y|
|SCO/eSCO over PCM|Y|Y|Y|Y|
|APCF Feature support|Y|Y|Y|Y|
|Maximum 16 Bluetooth LE connections \(Central role\)|Y|Y|Y|Y|
|**Bluetooth packet types supported**||||||||||||||||||
|ACL \(DM1, DH1, DM3, DH3, DM5, DH5, 2-DH1, 2-DH3, 2-DH5, 3-DH1, 3-DH3, 3-DH5\)|Y|Y|Y|Y|
|SCO \(HV1, HV3\)|Y|Y|Y|Y|
|eSCO \(EV3, EV4, EV5, 2EV3, 3EV3, 2EV5, 3EV5\)|Y|Y|Y|Y|
|Bluetooth Profiles Supported|
|A2DP Source/Sink|Y|Y|Y|Y|
|AVRCP Target/Controller|Y|Y|Y|Y|
|HFP Gateway|Y|Y|Y|Y|
|OPP Server/Client|Y|Y|Y|Y|
|SPP|Y|Y|Y|Y|
|HID|Y|Y|Y|Y|
|GAP|Y|Y|Y|Y|
|HFP Dev<sup>1</sup>|Y|Y|N|N|
|PAN<sup>1</sup>|Y|Y|Y|Y|
|**Bluetooth dual profiles supported**<sup>1</sup>|
|Dual A2DP \(2 Source\)|Y|Y|Y|N|
|Dual A2DP \(1 Source + 1 Sink\)|Y|Y|Y|N|
|Dual HFP \(1 WBS/1NBS\) PCM|Y|Y|N|N|
|Dual HFP \(2 NBS\) PCM\)|Y|Y|N|N|
|**Bluetooth audio features**||||||||||||||||||
|PCM NBS central/peripheral|Y|Y|Y|Y|
|PCM WBS central/peripheral|Y|Y|Y|Y|
|AAC and LDAC audio codec support<sup>1</sup>|Y|Y|N|N|

1. Contact your support representative to use this feature.
2. In-band \(Independent Reset\)IR can directly work with M.2 based modules on i.MX but OOB IR needs the external uSD muRata adaptor board with M.2 module
<a name="bluetooth-le"></a>
## Bluetooth LE

|Features|88W9098 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8987 SDIO-UART|IW610 SDIO-UART|IW610 USB-USB|
|--------|-----------------|-----------------|---------------------|-----------------|---------------|-------------|
|**Bluetooth profiles**||||||||||||||||||
|Bluetooth LE GATT|Y|Y|Y|Y|Y|Y|
|Bluetooth LE HOGP|Y|Y|Y|Y|Y|Y|
|Bluetooth LE GAP|Y|Y|Y|Y|Y|Y|
|Low Energy Physical Layer|Y|Y|Y|Y|Y|Y|
|Low Energy Link Layer|Y|Y|Y|Y|Y|Y|
|**Bluetooth 4.0**||||||||||||||||||
|Enhancements to HCI for Low Energy|Y|Y|Y|Y|Y|Y|
|Low Energy Direct Test Mode|Y|Y|Y|Y|Y|Y|
|Bluetooth LE - 1Mbit/s support|Y|Y|Y|Y|Y|Y|
|**Bluetooth 4.1**||||||||||||||||||
|Low duty Cycle Directed Advertising|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Dual Mode Topology|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Privacy v1.1|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Link Layer Topology|Y|Y|Y|Y|Y|Y|
|**Bluetooth 4.2**|||||||||||||||||||| | |  | | | 
|Bluetooth LE secure connection|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Link Layer Privacy v1.2|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Data Length Extension|Y|Y|Y|Y|Y|Y|
|Link Layer Extended Scanner Filter Policies|Y|Y|Y|Y|Y|Y|
|**Bluetooth 5.0**||||||||||||||||||
|Bluetooth LE 2 Mbps Support|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Multiple Advertisement \(4 or 6\*\) Sets<sup>3</sup>|Y|Y|Y|Y|Y|Y|\*|
|Bluetooth LE Extended Advertisement|N|N|Y|Y|Y|Y|
|High Duty Cycle Directed Advertising|Y|Y|Y|Y|Y|Y|
|Bluetooth LE Long Range|Y|Y|N|Y|Y|Y|
|**Bluetooth 5.2**||||||||||||||||||
|Bluetooth LE Power Control<sup>3</sup>|N|N|Y|N|Y|Y|
|Isochronous Channel<sup>3</sup>|N|N|N|N|
|**BCA-TDM mode \(shared antenna\)**||||||||||||||||||
|STA + Bluetooth Coex|N|N|Y|Y|N|N|
|STA + Bluetooth LE Coex|N|N|Y|Y|Y|Y
|STA + Bluetooth + Bluetooth LE Coex|N|N|Y|Y|N|N|
|AP + Bluetooth Coex|N|N|Y|Y|N|N|
|AP + Bluetooth LE Coex|N|N|Y|Y|Y|Y|
|**LE audio features** <sup>4</sup> <sup>5</sup>|
|CIS source|N|N|Y|N|N|N|
|CIS sink|N|N|Y|N|N|N|
|CIG validation|N|N|Y|N|N|N|
|Phy: 1M/2M|N|N|Y|N|N|N|
|Mono \(for 1 and 2 streams\) and stereo \(for 1 stream\)|N|N|Y|N|N|N|
|Unframed mode|N|N|Y|N|N|N|
|Sequential packing|N|N|Y|N|N|N|
|CIS encrypted audio|N|N|Y|N|N|N|
|ISO interval for LE audio: 7.5 ms, 10 ms, 20 ms, 30 ms|N|N|Y|N|N|N|
|Bit rate: up to 96 kbps|N|N|Y|N|N|N|
|1-CIS over one LE ACL and 2-CIS over two separate LE ACL|N|N|Y|N|N|N|

3. Not Validated using Android BSP. Contact your NXP representative for more details.
4. Contact your support representative to use this feature.
5. IW611/IW612 firmware supported feature.

**Parent topic:** [Feature lists](../topics/feature_lists.md)


