[Link to index page](../index.md)

# Wi-Fi radio

- [Client mode](#client-mode)
- [AP mode](#ap-mode)
- [Wi-Fi Direct](#wifi-direct)
- [AP-STA mode](#ap-sta-mode)

<a name="client-mode"></a>
## Client mode

Feature list for Wi-Fi radio and client mode

|Features|88W8997 PCIe-UART|88W9098 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW416 SDIO-UART|
|--------|-----------------|-----------------|-----------------|---------------------|-----------------|-----------------|---------------|
|**802.11n – High throughput \(HT\)**|||||||||||||||||||||
|2.4 GHz band operation supported channel bandwidth: 20 MHz|Y|Y|Y|Y|Y|Y|Y|
|2.4 GHz band supported channel bandwidths: 40 MHz|Y|Y|Y|Y|Y|Y|Y|
|5 GHz band supported channel bandwidths: 20 MHz|Y|Y|Y|Y|Y|Y|Y|
|5 GHz band supported channel bandwidths: 40 MHz|Y|Y|Y|Y|Y|Y|Y|
|Short/long guard interval \(400 ns/800 ns\)|Y|Y|Y|Y|Y|Y|Y|
|802.11 data rates – Up to 72 Mbit/s \(MCS 0 to MCS 7\)|Y|Y|Y|Y|Y|Y|Y|
|802.11 data rates – Up to 150 Mbit/s \(MCS 0 to MCS 7\)|Y|Y|Y|Y|Y|Y|Y|
|802.11 data rates - Up to 300 Mbit/s \(MCS 0 to MCS 15\)|Y|Y|Y|N|Y|N|N|
|One spatial stream \(1x1\)|Y|Y|Y|Y|Y|Y|Y|
|Two spatial streams \(2x2\)|Y|Y|Y|N|Y|N|N|
|HT protection mechanisms|Y|Y|Y|Y|Y|Y|Y|
|Explicit Beamformee|Y|Y|Y|N|Y|N|N|
|Aggregated MAC protocol data unit \(AMPDU\) RX support|Y|Y|Y|Y|Y|Y|Y|
|Aggregated MAC service data unit \(AMSDU\) -4k RX support|Y|Y|Y|Y|Y|Y|Y|
|20/40 MHz coexistence|Y|Y|Y|Y|Y|N|N|
|TX MCS rate adaptation \(BGN\)|Y|Y|Y|Y|Y|Y|Y|
|RX and TX space time block coding \(STBC\)|Y|Y|Y|N|Y|N|N|
|RX Low Density Parity Check \(LDPC\)|Y|Y|Y|Y|Y|Y|N|
|AMSDU over AMPDU support|Y|Y|Y|Y|Y|Y|Y|
|**802.11ac – Very high throughput \(VHT\)**|||||||||||||||||||||
|5 GHz band supported channel bandwidths: 20 MHz|Y|Y|Y|Y|Y|Y|N|
|5 GHz band supported channel bandwidths: 40 MHz|Y|Y|Y|Y|Y|Y|N|
|SU-AMPDU Aggregation|Y|Y|Y|Y|Y|Y|N|
|5 GHz band supported channel bandwidths: 80 MHz|Y|Y|Y|Y|Y|Y|N|
|802.11ac data rates - Up to 433.3 Mbps \(MCS 0 to MCS 9\) -1x1|Y|Y|Y|Y|Y|Y|N|
|802.11ac Data rates - Up to 866.7 Mbps \(MCS 0 to MCS 9\) -2x2|Y|Y|Y|N|Y|N|N|
|Short/long guard interval \(400ns/800ns\)|Y|Y|Y|Y|Y|Y|N|
|MU-MIMO RX – Wave 2|Y|Y|Y|Y|Y|Y|N|
|SU-beamformee|Y|Y|Y|Y|Y|Y|N|
|MU-MIMO beamformee \(explicit and implicit\)|Y|Y|Y|Y|Y|Y|N|
|RTS/CTS with BW signaling|Y|Y|Y|Y|Y|N|N|
|Operation mode notification|Y|Y|Y|Y|Y|Y|N|
|Backward compatibility with non-VHT devices|Y|Y|Y|Y|Y|Y|N|
|TX VHT MCS rate adaptation|Y|Y|Y|Y|Y|Y|N|
|**802.11ax – High efficiency \(HE\)**|||||||||||||||||||||
|5 GHz band supported channel bandwidth: 20MHz|N|Y|Y|Y|N|N|N|
|5 GHz band supported channel bandwidth: 40 MHz|N|Y|Y|Y|N|N|N|
|5 GHz band supported channel bandwidth: 80MHz|N|Y|Y|Y|N|N|N|
|802.11ax data rates – Up to 1.2 Gbps \(MCS 0 to MCS 11\) – 2x2|N|Y|Y|N|N|N|N|
|Operating mode indication \(OMI\) control|N|Y|Y|Y|N|N|N|
|2x/4x HE-long training field \(LTF\)|N|Y|Y|Y|N|N|N|
|UL \(TX\) and DL \(RX\) MU-MIMO|N|Y|Y|Y|N|N|N|
|UL \(TX\) and DL \(RX\) OFDMA|N|Y|Y|Y|N|N|N|
|256 QAM modulation – MCS 8 and MCS9|N|Y|Y|Y|N|N|N|
|1024 QAM modulation – MCS 10 and MCS11|N|Y|Y|Y|N|N|N|
|SU beamforming|N|Y|Y|Y|N|N|N|
|TWT|N|Y|Y|Y|N|N|N|
|Spatial reuse|N|N|N|Y|N|N|N|
|OFDMA \(ul/dl, 484 RU\)|N|Y|Y|Y|N|N|N|
|BSS coloring|N|Y|Y|Y|N|N|N|
|**802.11a/b/g features**|||||||||||||||||||||
|802.11b/g data rates – Up to 54 Mbit/s|Y|Y|Y|Y|Y|Y|Y|
|802.11a data rates – Up to 54 Mbit/s|Y|Y|Y|Y|Y|Y|Y|
|Fragmentation/defragmentation|Y|Y|Y|Y|Y|Y|Y|
|ERP Protection using mac ctrl command \(RTS-CTS/Self-CTS\)|Y|Y|Y|Y|Y|Y|Y|
|ERP protection, slot time, preamble|Y|Y|Y|Y|Y|Y|Y|
|TX rate adaptation \(BG\)|Y|Y|Y|Y|Y|Y|Y|
|**802.11d and 802.11h features**|||||||||||||||||||||
|802.11d – Regulatory domain/operating class/country info|Y|Y|Y|Y|Y|Y|Y|
|Per-path regulatory power table<sup>1<sup>|N|N|N|Y|N|Y|N|
|802.11h – Dynamic frequency selection \(DFS\)|Y|Y|Y|Y|Y|Y|Y|
|DFS radar detection in peripheral mode \(Follow AP\)|Y|Y|Y|Y|Y|Y|Y|
|**802.112 – QoS**|||||||||||||||||||||
|EDCA \[enhanced distributed channel access\] / WMM \(wireless multi-media\)|Y|Y|Y|Y|Y|Y|Y|
|**802.11i security features**|||||||||||||||||||||
|Opensource WPA Supplicant Support|Y|Y|Y|Y|Y|Y|Y|
|WPA2-PSK AES \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|
|WPA3-SAE \(Simultaneous Authentication of Equals\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|
|WPA2+WPA3 PSK Mixed Mode \(WPA3 Transition Mode\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|
|Wi-Fi Enhanced Open - OWE \(Opportunistic Wireless Encryption\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|
|802.1x EAP Authentication Methods \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|
|WPA2-Enterprise Mixed Mode \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|
|WPA3-Enterprise \(Suite-B\) \|National Security Algorithm \(CSNA\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|N|
|802.11w - PMF \(Protected Management Frames\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|
|WPA3 Enterprise|Y|Y|Y|Y|Y|Y|Y|
|**802.11mc features**|||||||||||||||||||||
|Wi-Fi location<sup>1<sup>|N|Y|Y|Y|N|N|N|
|**802.11az features**|||||||||||||||||||||
|New generation Wi-Fi Location<sup>1<sup>|N|N|N|Y|N|N|N|
|**WPA3 SAE \(R3\) security features**|||||||||||||||||||||
|Simultaneous authentication of equals \(SAE\)|Y|Y|Y|Y|Y|Y|Y|
|SAE Connectivity and PMK Caching|Y|Y|Y|Y|Y|Y|Y|
|WPA2 Personal Compatibility|Y|Y|Y|Y|Y|Y|Y|
|Anti-Clogging|Y|Y|Y|Y|Y|Y|Y|
|SAE Finite Cyclic Group – Group-19, Group 20, Goup-21|Y|Y|Y|Y|Y|Y|Y|
|Reflection Attack|Y|Y|Y|Y|Y|Y|Y|
|Suite B – 192-bit security ECC p384|Y|Y|Y|Y|Y|Y|N|
|Suite B – 192-bit security RSA 3K|Y|Y|Y|Y|Y|Y|N|
|Wi-Fi enhanced open|Y|Y|Y|Y|Y|Y|Y|
|WPA3 host-based|Y|Y|Y|Y|Y|Y|Y|
|**802.11r – Fast BSS transition \(FT\) features**|||||||||||||||||||||
|FT over air and over distribution system \(DS\) \(open, WPA2-PSK\)|Y|Y|Y|Y|Y|Y|Y|
|**802.11k features**|||||||||||||||||||||
|802.11k|Y|Y|Y|Y|Y|Y|Y|
|**802.11v features**|||||||||||||||||||||
|802.11v|Y|Y|Y|Y|Y|Y|Y|
|**FIPS feature**|||||||||||||||||||||
|FIPS support|Y|Y|Y|Y|Y|Y|Y|
|**802.11w – Protected management frame \(PMF\) features**|||||||||||||||||||||
|PMF require and capable|Y|Y|Y|Y|Y|Y|Y|
|Unicast management frames - Encryption/decryption - using CCMP|Y|Y|Y|Y|Y|Y|Y|
|Broadcast management frames - Encryption/decryption - using BIP|Y|Y|Y|Y|Y|Y|Y|
|SA query request/response|Y|Y|Y|Y|Y|Y|Y|
|PMF Support using Opensource WPA|Y|Y|Y|Y|Y|Y|Y|
|**Power save mode features**|||||||||||||||||||||
|Deep sleep|Y|Y|Y|Y|Y|Y|Y|
|IEEE power save|Y|Y|Y|Y|Y|Y|Y|
|**General features**|||||||||||||||||||||
|EU adaptivity support|Y|Y|Y|Y|Y|Y|Y|
|MAC address randomization \(in scan\)|Y|Y|Y|Y|Y|Y|Y|
|Host based MLME<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|
|Extended channel switch announcement \(ECSA\)|Y|Y|Y|Y|Y|Y|Y|
|DCM|N|Y|Y|Y|N|N|N|
|Wireless Android auto<sup>2<sup>|N|Y|Y|Y|N|Y|N|
|mDNS \(Bonjour\) offload|Y|Y|Y|Y|Y|Y|Y|
|Extended range<sup>1<sup>|N|Y|Y|Y|N|N|N|
|Wi-Fi agile multiband|N|Y|Y|Y|N|N|N|
|Auto Reconnect|N|N|N|Y|N|N|N|
|Independent reset \(In-band\)<sup>4</sup>|Y|Y|Y|Y|Y|Y|Y|
|Specific scan \(scancfg\)|Y|Y|Y|Y|Y|Y|Y|
|Network scan \(iwlist scan\)|Y|Y|Y|Y|Y|Y|Y|
|Cancellable scan|Y|Y|Y|Y|Y|Y|Y|
|Passive to active scan|Y|Y|Y|Y|Y|Y|Y|

1. Contact your support representative to use this feature.
2. Not validated using Android BSP. Contact your NXP representative for more details.
3. Feature enabled by default in the software.
4. To avoid any version mismatch, copy both the combo firmware and the standalone Wi-Fi firmware to the firmware directory \(*/lib/firmware/nxp/*\) of the host system.


<a name="ap-mode"></a>
## AP mode

Feature list for Wi-Fi radio and AP mode

|Features|88W8997 PCIe-UART|88W9098 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW416 SDIO-UART|
|--------|-----------------|-----------------|-----------------|---------------------|-----------------|-----------------|---------------|
|**802.11n – High throughput \(HT\)**|||||||||||||||||||||
|Short/long guard interval \(400 ns/800 ns\)|Y|Y|Y|Y|Y|Y|Y|
|802.11n data rates – Up to 72 Mbit/s \(MCS0 to MCS7\)|Y|Y|Y|Y|Y|Y|Y|
|5 GHz band supported channel bandwidths: 20 MHz|Y|Y|Y|Y|Y|Y|Y|
|5 GHz band supported channel bandwidths: 40 MHz|Y|Y|Y|Y|Y|Y|Y|
|802.11n data rates – Up to 150 Mbit/s \(MCS0 to MCS7\)|Y|Y|Y|Y|Y|Y|Y|
|802.11n data rates - Up to 300 Mbit/s \(MCS0 to MCS15\)|Y|Y|Y|N|Y|N|N|
|TX MCS rate adaptation \(BGN\)|Y|Y|Y|Y|Y|Y|Y|
|20/40 MHz coexistence|Y|Y|Y|Y|Y|N|N|
|Aggregated MAC protocol data unit \(AMPDU\) TX and RX support|Y|Y|Y|Y|Y|Y|Y|
|Aggregated MAC service data unit \(AMSDU\) - 4k RX support|Y|Y|Y|Y|Y|Y|Y|
|HT protection mechanisms|Y|Y|Y|Y|Y|Y|Y|
|RX low density parity check \(LDPC\)|Y|Y|Y|Y|Y|Y|N|
|HT Duplicate mode \(MCS32\)|Y|Y|Y|Y|Y|Y|Y|
|**802.11b/g features**|||||||||||||||||||||
|802.11b/g data rates – Up to 54 Mbit/s|Y|Y|Y|Y|Y|Y|Y|
|TX rate adaptation \(BG\)|Y|Y|Y|Y|Y|Y|Y|
|ERP protection, slot time, preamble|Y|Y|Y|Y|Y|Y|Y|
|Handling of associated STAs with IEEE PS - null data|Y|Y|Y|Y|Y|Y|Y|
|**802.11ac – Very high throughput \(VHT\)**|||||||||||||||||||||
|5 GHz band supported channel bandwidths: 20 MHz|Y|Y|Y|Y|Y|Y|N|
|5 GHz band supported channel bandwidths: 40 MHz|Y|Y|Y|Y|Y|Y|N|
|5 GHz band supported channel bandwidths: 80 MHz|Y|Y|Y|Y|Y|Y|N|
|Short/Long Guard Interval \(400ns/800ns\)|Y|Y|Y|Y|Y|Y|N|
|802.11ac data rates – Up to 433.3 Mbps \(MCS 0 to MCS 9\)|Y|Y|Y|Y|Y|Y|N|
|802.11ac data rates - Up to 866.7 Mbps \(MCS 0 to MCS 9\)|Y|Y|Y|N|Y|N|N|
|Single user \(SU\)-Aggregated MAC protocol data unit \(SU-AMPDU\) aggregation|Y|Y|Y|Y|Y|Y|N|
|RTS/CTS with BW signaling|Y|Y|Y|Y|Y|Y|N|
|Backward Compatibility with non-VHT devices|Y|Y|Y|Y|Y|Y|N|
|TX VHT MCS rate adaptation|Y|Y|Y|Y|Y|Y|N|
|Operation Mode Notification|Y|Y|Y|Y|Y|Y|N|
|Explicit Beamformer|Y|Y|Y|N|Y|N|N|
|SU-Beamformee|Y|Y|Y|Y|Y|Y|N|
|256 QAM Modulation - MCS8 and MCS9|Y|Y|Y|Y|Y|Y|N|
|**802.11ax – High efficiency \(HE\) features**|||||||||||||||||||||
|5 GHz band supported channel bandwidth: 20MHz|N|Y|Y|Y|N|N|N|
|5 GHz band supported channel bandwidth: 40 MHz|N|Y|Y|Y|N|N|N|
|5 GHz band supported channel bandwidth: 80 MHz|N|Y|Y|Y|N|N|N|
|Operating mode indication \(OMI\) control|N|Y|Y|Y|N|N|N|
|2x/4x HE-Long training field \(LTF\)|N|Y|Y|Y|N|N|N|
|256 QAM modulation – MCS8 and MCS9|N|Y|Y|Y|N|N|N|
|1024 QAM modulation – MCS10 and MCS11|N|Y|Y|Y|N|N|N|
|SU beamforming|N|Y|Y|N|N|N|N|
|**802.11d features**|||||||||||||||||||||
|802.11d - Regulatory domain/operating class/country info|Y|Y|Y|Y|Y|Y|Y|
|**802.11h features**|||||||||||||||||||||
|802.11h – Dynamic frequency selection \(DFS\)|Y|Y|Y|Y|Y|Y|Y|
|**802.11e – QoS features**|||||||||||||||||||||
|Enhanced distributed channel access \(EDCA\) / wireless multi-media \(WMM\)|Y|Y|Y|Y|Y|Y|Y|
|**802.11i security features**|||||||||||||||||||||
|Hostapd Support|Y|Y|Y|Y|Y|Y|Y|
|WPA2-PSK AES \| hostapd|Y|Y|Y|Y|Y|Y|Y|
|WPA3-SAE \(Simultaneous Authentication of Equals\) \| Hostapd|Y|Y|Y|Y|Y|Y|Y|
|WPA2+WPA3 PSK Mixed Mode \(WPA3 Transition Mode\) \| Hostapd|Y|Y|Y|Y|Y|Y|Y|
|Wi-Fi Enhanced Open - OWE \(Opporturnistic Wireless Encryption\) \| Hostapd|Y|Y|Y|Y|Y|Y|Y|
|802.1x EAP Authentication Methods \| Hostapd|N|N|N|N|N|N|Y|
|802.11w - PMF \(Protected Management Frames\) \| Hostapd|Y|Y|Y|Y|Y|Y|Y|
|**WPA3 SAE \(R3\) security features**|||||||||||||||||||||
|Simultaneous authentication of equals \(SAE\)|Y|Y|Y|Y|Y|Y|Y|
|SAE connectivity and PMK caching|Y|Y|Y|Y|Y|Y|Y|
|Wi-Fi enhanced open|Y|Y|Y|Y|Y|Y|N|
|WPA3 Enterprise Suite-B Host \(host based\)|Y|Y|Y|Y|Y|Y|N|
|**802.11w – Protected management frame \(PMF\) features**|||||||||||||||||||||
|PMF require and capable|Y|Y|Y|Y|Y|Y|Y|
|Unicast management frames - Encryption/decryption - using CCMP|Y|Y|Y|Y|Y|Y|Y|
|Support using Hostapd|Y|Y|Y|Y|Y|Y|Y|
|Broadcast management frames - Encryption/decryption - using BIP|Y|Y|Y|Y|Y|Y|Y|
|SA query request/response|Y|Y|Y|Y|Y|Y|Y|
|**General features**|||||||||||||||||||||
|EU adaptivity support|Y|Y|Y|Y|Y|Y|Y|
|Automatic channel selection \(ACS\)|Y|Y|Y|Y|Y|Y|Y|
|Host-based MLME<sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|
|Extended channel switch announcement \(ECSA\)|Y|Y|Y|Y|Y|Y|Y|
|Max supported stations|8|16|16|16|8|8|8|
|Independent reset \(In-band\)|Y|Y|Y|Y|Y|Y|Y|
|Hidden SSID \(broadcast SSID disabled\)|Y|Y|Y|Y|Y|Y|Y|
|MAC Address Filter \(Allowed/Denied List\)|Y|Y|Y|Y|Y|Y|Y|
|STA age out feature for associated clients|Y|Y|Y|Y|Y|Y|Y|
|Vendor defined txpower config \(txpower config V3\)|N|Y|Y|Y|N|N|N|
|Maximum STA MAC address filtering|16|64|64|16|16|16|16|
|Android Packet Filtering \(APF\)|Y|Y|Y|Y|Y|Y|Y|

 1. Feature enabled by default in the software.

<a name="wifi-direct"></a>
## Wi-Fi Direct

Feature list for Wi-Fi Direct/P2P

|Features|88W8997 PCIe-UART|88W9098 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW416 SDIO-UART|
|--------|-----------------|-----------------|-----------------|---------------------|-----------------|-----------------|---------------|
|**P2P basic functionality**|||||||||||||||||||||
|Autonomous GO mode|Y|Y|Y|Y|Y|Y|Y|
|WFD client mode|Y|Y|Y|Y|Y|Y|Y|
|P2P device mode|Y|Y|Y|Y|Y|Y|Y|
<a name="ap-sta-mode"></a>
## AP/STA mode

Feature list for Wi-Fi AP/STA mode

|Features|88W8997 PCIe-UART|88W9098 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW416 SDIO-UART|
|--------|-----------------|-----------------|-----------------|---------------------|-----------------|-----------------|---------------|
|Software antenna diversity<sup>1</sup>|N|N|N|Y|N|Y|Y|
|RF test mode functionality|Y|Y|Y|Y|Y|Y|Y|
|TX power config|Y|Y|Y|Y|Y|Y|Y|
|AP-STA functionality \(same channel\)|Y|Y|Y|Y|Y|Y|Y|
|Auto firmware recovery on fatal error|Y|Y|Y|Y|Y|N|Y|
|Auto ARP and ping support|Y|Y|Y|Y|Y|Y|Y|
|Secure boot|N|N|N|Y|N|N|N|

1. Contact your support representative to use this feature.

**Parent page:**[Feature lists](../topics/feature_lists.md)


