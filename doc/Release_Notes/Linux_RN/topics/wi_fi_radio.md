[Link to the index page](../index.md)

# Wi-Fi radio

- [Client mode](#client-mode)
- [AP mode](#ap-mode)
- [Wi-Fi Direct](#wifi-direct)
- [Concurrent dual Wi-Fi \(CDW\) mode](#concurrent-dual-wifi-cdw-mode)
- [Known limitations for simultaneous mode of operation](#known-limitations-for-simultaneous-mode-of-operation)

<a name="client-mode"></a>
## Client mode

|Feature        |88W9098 PCIe-UART|88W8997 PCIe-UART|AW693 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW610 SDIO-UART|IW416 SDIO-UART|IW610 USB-USB|
|---------------|-------|-------|-----|-------|-----------|-------|-------|-----|-----|-----|
|**802.11n High Throughput**|||||||||||
|2.4 GHz band supported channel bandwidth: 20 MHz|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|2.4 GHz band supported channel bandwidth: 40 MHz|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|5 GHz band supported channel bandwidth: 20 MHz|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|5 GHz band supported channel bandwidth: 40 MHz|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|Short/long guard interval \(400 ns/800 ns\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Data rates up to 72 Mbit/s \(MCS 0 to MCS 7\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Data rates up to 150 Mbit/s \(MCS 0 to MCS 7\)|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|Data rates up to 300 Mbit/s \(MCS 0 to MCS 15\)|Y|Y|Y|Y|N|Y|N|N|N|N|
|1 spatial stream \(1x1\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|2 spatial stream \(2x2\)|Y|Y|Y|Y|N|Y|N|N|N|N|
|HT protection mechanisms|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Explicit Beamformee|Y|Y|Y|Y|Y|Y|N|N|N|N|
|Aggregated MAC Protocol Data Unit\(AMPDU\) RX support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Aggregated MAC Service Data Unit\(AMSDU\) -4k RX support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|20 MHz/40 MHz coexistence|Y|Y|Y|Y|Y|Y|N|N|N|N|
|TX MCS rate adaptation \(BGN\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|RX and TX space time block coding for 2x2 \(STBC\)|N|Y|N|N|N|Y|N|N|N|N|
|RX low-density parity check \(LDPC\)|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|AMSDU over AMPDU support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11ac Very High Throughput**|||||||||||
|5 GHz band supported channel bandwidth: 20 MHz|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|5 GHz band supported channel bandwidth: 40 MHz|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|5 GHz band supported channel bandwidth: 80 MHz|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|Data rates up to 433.3 Mbit/s \(MCS0 to MCS9\)|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|Data rates up to 866.7 Mbit/s \(MCS0 to MCS9\)|Y|Y|Y|Y|N|Y|N|N|N|N|
|Short/Long Guard Interval \(400ns/800ns\)|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|SU-AMPDU Aggregation|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|MU-MIMO Beamformee \(Explicit and Implicit\)|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|SU-Beamformee|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|MU-MIMO RX – Wave 2|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|RTS/CTS with BW Signaling|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|Operation Mode Notification|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|Backward compatibility with non-VHT devices|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|TX VHT MCS Rate Adaptation|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|LDPC|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|256 QAM Modulation – MCS 8 and MCS9|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11ax High Efficiency**|||||||||||
|5 GHz band supported channel bandwidth: 20 MHz|Y|N|Y|Y|Y|N|N|Y|N|Y|
|5 GHz band supported channel bandwidth: 40 MHz|Y|N|Y|Y|Y|N|N|N|N|N|
|5 GHz band supported channel bandwidth: 80 MHz|Y|N|Y|Y|Y|N|N|N|N|N|
|2.4 GHz band supported channel bandwidth: 20 MHz|Y|N|Y|Y|Y|N|N|Y|N|Y|
|2.4 GHz band supported channel bandwidth: 40 MHz<sup>1</sup>|Y|N|Y|Y|Y|N|N|N|N|N|
|Data rates up to 1.2 Gbit/s \(MCS 0 to MCS 11\) - 2x2|Y|N|Y|Y|N|N|N|N|N|N|
|Data rates up to 600 Mbit/s \(MCS 0 to MCS 11\) - 1x1|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Operating Mode Indication \(OMI\) control|Y|N|Y|Y|Y|N|N|N|N|N|
|2x/4x HE Long Training Field \(LTF\)|Y|N|Y|Y|Y|N|N|N|N|N|
|Target wake-up Time|Y|N|Y|Y|Y|N|N|Y|N|Y|
|1024 QAM modulation – MCS10-MCS11<sup>2</sup>|Y|N|Y|Y|Y|N|N|N|N|N|
|256 QAM modulation – MCS8 and MCS9|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Spatial reuse|N|N|Y|N|Y|N|N|Y|N|Y|
|SU beamforming|Y|N|Y|Y|Y|N|N|N|N|N|
|UL \(TX\) and DL \(RX\) MU-MIMO|Y|N|Y|Y|Y|N|N|Y|N|Y|
|UL \(TX\) and DL \(RX\) OFDMA|Y|N|Y|Y|Y|N|N|Y|N|Y|
|OFDMA \(UL/DL, 484 RU\)|Y|N|Y|Y|Y|N|N|N|N|N|
|BSS coloring|Y|N|Y|Y|Y|N|N|Y|N|Y|
|**802.11a – 802.11b – 802.11g**|||||||||||
|802.11b/g data rates up to 54 Mbit/s|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|802.11a data rates up to 54 Mbit/s|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|TX rate adaptation \(BG\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Fragmentation/defragmentation|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|ERP protection, slot time, preamble|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|ERP Protection using mac ctrl command \(RTS-CTS/Self-CTS\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11d – 802.11h**|||||||||||
|802.11d regulatory domain/ operating class/country info|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Per-path regulatory power table <sup>1</sup>|N|N|N|N|Y|N|Y|N|N|N|
|802.11h – Dynamic Frequency Selection \(DFS\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|DFS radar Detection in follower Mode \(Follow AP\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11e QoS**|||||||||||
|EDCA \[Enhanced Distributed Channel Access\] / WMM \(Wireless Multi-Media\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11i security**|||||||||||
|Open source WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WEP-64/128 \| WPA Supplicant|Y|N|Y|Y|Y|N|N|Y|N|Y|
|WPA-PSK TKIP \| WPA Supplicant|Y|N|Y|Y|N|N|N|Y|N|Y|
|WPA2-PSK AES \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA+WPA2 PSK Mixed Mode \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA3-SAE \(Simultaneous Authentication of Equals\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA2+WPA3 PSK Mixed Mode \(WPA3 Transition Mode\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Wi-Fi Enhanced Open - OWE \(Opportunistic Wireless Encryption\) \| WPA Supplicant|Y|N|Y|Y|Y|N|Y|Y|N|Y|
|802.1x EAP authentication Methods \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA2-Enterprise GCMP \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|WPA2-Enterprise Mixed Mode \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA3-Enterprise \(Suite-B\) \|National Security Algorithm \(CSNA\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|802.11w - PMF \(Protected Management Frames\) \| WPA Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WAPI \| WPA Supplicant|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|STA - WEP-64/128 \| Embedded Supplicant|Y|N|Y|Y|Y|N|N|Y|N|Y|
|STA - WPA-PSK TKIP \| Embedded Supplicant|Y|N|Y|Y|Y|N|N|Y|N|Y|
|STA - WPA2-PSK AES \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|STA - WPA+WPA2 PSK Mixed Mode \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|STA - WPA3-SAE \(Simultaneous authentication of Equals\) \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|STA - 802.11w - PMF \(Protected Management Frames\) \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA3 Enterprise|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11r Fast BSS Transition \(FT\)**|||||||||||
|FT over Air and over DS \(Distribution System\) \[Open, WPA2 security\]|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11k**|||||||||||
|802.11k|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11v**|||||||||||
|802.11v|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11z**|||||||||||
|802.11z \(Host based TDLS\)|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|**802.11az**|||||||||||
|New generation Wi-Fi Location|N|N|Y|N|Y|N|N|Y|N|Y|
|**802.11mc**|||||||||||
|Wi-Fi location<sup>1</sup><sup>6</sup>|Y|N|Y|Y|Y|N|N|Y|N|Y|
|**FIPS**|||||||||||
|FIPS support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**FIPS encryption**|||||||||||
|RC4|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|AES\_ECB|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|AES\_WRAP|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|AES\_CCM \(128 bits\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|AES\_CCM \(256 bits\)|Y|N|Y|Y|Y|N|Y|Y|N|Y|
|AES\_GCM \(128, 256 bits\)|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|**WPS/WSC2.0 functionality**|||||||||||
|PIN Config Method - 8 Digit/4 Digit|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|PIN Config Method - Static/Dynamic PIN|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|PBC - Virtual Push Button Config Method|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|PBC Session Overlap Detection|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|STA as Enrollee|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Backward compatibility with WPS1.0 Devices|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Open source WPA supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**DPP functionality**|||||||||||
|Wi-Fi Easy Connect|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**Power save mode**|||||||||||
|Deep sleep|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|IEEE power save|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|U-APSD / WMM power save <sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**Wi-Fi 6E**|||||||||||
|6 GHz in-band association|N|N|Y|N|N|N|N|N|N|N|
|6 GHz in-band discovery|N|N|Y|N|N|N|N|N|N|N|
|Out of band discovery|N|N|Y|N|N|N|N|N|N|N|
|**General features**|||||||||||
|EU adaptivity support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Wake on Wireless (WoW) in-band|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Wake on Wireless (WoW) out-of-band|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Auto TX<sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Cloud keep alive (TX)<sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Cloud keep alive (TX and RX)<sup>1</sup>|N|N|Y|N|Y|N|N|Y|N|Y|
|MAC Address randomization\(in Scan\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Host-based MLME<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Driver load time parameters for manufacturing mode|Y|N|Y|Y|Y|N|N|N|N|N|
|Extended channel switch announcement \(ECSA\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Independent reset \(In-band\)<sup>4</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Wi-Fi agile multiband|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Wireless Apple CarPlay \(R5\)|Y|N|Y|Y|Y|N|Y|N|N|N|
|CSI<sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|CSI Monitor Mode <sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|Packet coalescing <sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|mDNS \(Bonjour\) offload|N|Y|Y|N|Y|Y|Y|N|Y|N|
|mDNS wake on match|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|IPv6 NS offload|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Extended range <sup>1</sup>|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Clock sync <sup>1</sup>|Y|Y|Y|Y|Y|Y|N|N|N|N|
|DCM|Y|N|Y|Y|Y|N|N|Y|N|Y|
|UNII\_4 Channel Support|Y|N|Y|Y|Y|N|N|Y|N|Y|
|NAPI Support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Auto reconnect|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|Band steering \(AGO + AGO and P2P\)|Y|N|Y|Y|N|N|N|N|N|N|
|Monitor Mode <sup>1</sup>|Y|Y|N|Y|Y|Y|Y|Y|Y|Y|
|Packet transmission with Monitor Mode|Y|N|N|Y|Y|N|N|Y|N|Y|
|Packet injection using scapy tool|Y|N|N|Y|Y|N|N|N|N|N|
|Wireless Android \( auto projection mode\)|Y|N|Y|Y|Y|N|N|N|N|N|
|Android automotive OS|Y|N|Y|Y|Y|N|Y|N|N|N|
|Specific scan \(scancfg\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Network scan \(iwlist scan\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Cancel-able scan|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Passive to active scan|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Software Antenna Diversity<sup>1</sup>|N|N|Y|N|Y|N|Y|Y|Y|Y|
|EasyMesh <sup>1</sup>|Y|N|N|Y|Y|N|N|N|N|N|
|Wi-Fi Aware \(NAN\)<sup>1</sup>|N|N|Y|N|Y|N|N|Y|N|Y|
|Wi-Fi Commissioning using PAF |Y|Y|N|Y|Y|Y|Y|Y|Y|Y|
|Vendor specific IE \(Custom IE\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Ambient Motion Index \(AMI\)|Y|N|N|Y|Y|N|N|Y|N|N|
|Secure Host Interface|N|N|Y|N|N|N|N|N|N|N|

1. Contact your support representative to use this feature.<br/>
2. MCS10 and MCS11 not supported on 88W9098 in 2.4 GHz band.<br/>
3. The feature is enabled by default in software.<br/>
4. To avoid any version mismatch, copy both combo firmware and standalone Wi-Fi firmware to the directory */lib/firmware/nxp/* of the host system.
5. Feature not supported in this release.
6. Not supported for new designs

<a name="ap-mode"></a>
## AP mode

|Feature        |88W9098 PCIe-UART|88W8997 PCIe-UART|AW693 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW610 SDIO-UART|IW416 SDIO-UART|IW610 USB-USB|
|---------------|-------|-------|-----|-------|-----------|-------|-------|-----|-----|-----|
|**802.11n High Throughput**|||||||||||||||||||||
|2.4 GHz band supported channel bandwidth: 20 MHz|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|2.4 GHz band supported channel bandwidth: 40 MHz|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|5 GHz band supported channel bandwidth: 20 MHz|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|5 GHz band supported channel bandwidth: 40 MHz|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|1 spatial stream \(1x1\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|2 spatial stream \(2x2\)|Y|Y|Y|Y|N|Y|N|N|N|N|
|Short/long guard interval \(400 ns/800 ns\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|802.11n data rates up to 72 Mbit/s \(MCS0 to MCS7\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|802.11n data rates up to 150 Mbit/s \(MCS0 to MCS7\)|Y|Y|Y|Y|Y|Y|Y|N|Y|N|
|802.11n data rates up to 300 Mbit/s \(MCS0 to MCS15\)|Y|Y|Y|Y|N|Y|N|N|N|N|
|TX MCS rate adaptation \(BGN\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Aggregated MAC protocol data unit \(AMPDU\) TX and RX support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Aggregated MAC service data unit \(AMSDU\) - 4k RX support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|HT protection mechanisms|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|RX and TX space time block coding \(STBC\)|N|Y|N|N|N|Y|N|N|N|N|
|20/40 MHz coexistence|Y|Y|Y|Y|Y|Y|N|N|N|N|
|Explicit beamformer|Y|N|Y|Y|Y|N|N|N|N|N|
|RX Low-density parity check \(LDPC\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11b/g features**|||||||||||
|802.11 b/g data rates up to 54 Mbit/s|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|TX rate adaptation \(BG\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|ERP protection, slot time, preamble|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Handling of associated STAs with IEEE PS - null data|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11ac Very High Throughput**|||||||||||
|5 GHz band supported channel bandwidth: 20 MHz|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|5 GHz band supported channel bandwidth: 40 MHz|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|5 GHz band supported channel bandwidth: 80 MHz|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|Short/Long Guard Interval \(400ns/800ns\)|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|802.11ac data rates up to 433.3 Mbps \(MCS 0 to MCS 9\) 1SS|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|802.11ac Data rates up to 866.7 Mbps \(MCS 0 to MCS 9\) 2SS|Y|Y|Y|Y|N|Y|N|N|N|N|
|Single User- Aggregated MAC Protocol Data Unit \(SU-AMPDU\) Aggregation|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|RTS/CTS with BW Signaling|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|Backward Compatibility with non-VHT devices|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|TX VHT MCS Rate Adaptation|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|Operation mode notification|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|SU Explicit beamformer|Y|N|Y|Y|N|N|N|N|N|N|
|Low-density parity check \(LDPC\)|Y|Y|Y|Y|Y|Y|Y|N|N|N|
|**802.11ax High Efficiency**|||||||||||
|5 GHz band supported channel bandwidth: 20 MHz|Y|N|Y|Y|Y|N|N|Y|N|Y|
|5 GHz band supported channel bandwidth: 40 MHz|Y|N|Y|Y|Y|N|N|N|N|N|
|5 GHz band supported channel bandwidth: 80 MHz|Y|N|Y|Y|Y|N|N|N|N|N|
|2.4 GHz band supported channel bandwidth: 20 MHz|Y|N|Y|Y|Y|N|N|Y|N|Y|
|2.4 GHz band supported channel bandwidth: 40 MHz<sup>1</sup>|Y|N|Y|Y|Y|N|N|N|N|N|
|Operating Mode Indication \(OMI\) Control|Y|N|Y|Y|Y|N|N|N|N|N|
|2x/4x HE-Long Training Field \(LTF\)|Y|N|Y|Y|N|N|N|N|N|N|
|1024 QAM|Y|N|Y|Y|Y|N|N|N|N|N|
|BSS color|Y|N|Y|Y|Y|N|N|Y|N|Y|
|HE SU beamformer \(explicit\)|Y|N|Y|Y|N|N|N|N|N|N|
|**802.11d**|||||||||||
|802.11d - Regulatory Domain/Operating Class/Country Info|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11h**|||||||||||
|802.11h - Dynamic Frequency Selection \(DFS\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Zero Wait DFS|Y|N|N|Y|N|N|N|N|N|N|
|**802.11e QoS**|||||||||||
|EDCA \[Enhanced Distributed Channel Access\] / WMM \(Wireless Multi-Media\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**802.11az**|||||||||||
|New generation Wi-Fi Location <sup>1</sup>|N|N|Y|N|Y|N|N|N|N|N|
|**802.11i security**|||||||||||
|Hostapd support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WEP-64/128 \| hostapd|Y|N|Y|Y|Y|N|N|Y|N|Y|
|WPA-PSK TKIP \| hostapd|Y|N|Y|Y|N|N|N|Y|N|Y|
|WPA2-PSK AES \| hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA+WPA2 PSK Mixed Mode \| hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA3-SAE \(Simultaneous Authentication of Equals\) \| hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA2+WPA3 PSK Mixed Mode \(WPA3 Transition Mode\) \| hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Wi-Fi Enhanced Open - OWE \(Opporturnistic Wireless Encryption\) \| hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|802.1x EAP Authentication Methods \| hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA2-Enterprise Mixed Mode \| hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA3-Enterprise \(Suite-B\) \|National Security Algorithm \(CSNA\) \| hostapd|Y|N|Y|Y|Y|N|Y|N|N|N|
|802.11w - PMF \(Protected Management Frames\) Hostpad|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WAPI \| wpa\_supplicant|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Embedded Authenticator|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WEP-64/128 \| Embedded Supplicant|Y|N|Y|Y|Y|N|N|Y|N|Y|
|WPA-PSK TKIP \| Embedded Supplicant|Y|N|Y|Y|Y|N|N|Y|N|Y|
|WPA2-PSK AES \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA+WPA2 PSK Mixed Mode \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WPA3-SAE \(Simultaneous Authentication of Equals\) \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|802.11w - PMF \(Protected Management Frames\) \| Embedded Supplicant|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**WPS/WSC2.0 functionality**|||||||||||
|PIN Config Method - 8 Digit/4 Digit|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|PIN Config Method - Static/Dynamic PIN|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|PBC - Virtual Push Button Config Method|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|PBC Session Overlap Detection|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|AP Setup Locked State - PIN Method|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|MMH as Wireless Registrar|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|MMH as Enrollee|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Opensource Hostapd|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**Wi-Fi 6E features**|||||||||||
|BSS establishment|N|N|Y|N|N|N|N|N|N|N|
|BSS channel signaling|N|N|Y|N|N|N|N|N|N|N|
|Signaling of capabilities in 6 GHz band|N|N|Y|N|N|N|N|N|N|N|
|HE beaconing|N|N|Y|N|N|N|N|N|N|N|
|**General features**|||||||||||
|EU adaptivity support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Wake on Wireless (WoW) In-band |Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Wake on Wireless (WoW) Out-band |Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Automatic channel selection \(ACS\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Host-based MLME<sup>2</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|MBSS|Y|Y|Y|Y|Y|Y|Y|Y|N|Y|
|Extended channel switch announcement \(ECSA\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Driver load time parameters for manufacturing mode|Y|N|Y|Y|N|N|N|N|N|N|
|Max supported stations|48|8|64|48|16|8|8|8|8|8|
|Independent reset \(in-band\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Independent reset \(out-of-band\)<sup>3</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Hidden SSID \(broadcast SSID disabled\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|MAC address filter \(allowed/denied list\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Maximum STA MAC address filtering|48|8|64|48|16|8|8|8|8|8|
|STA age out feature for associated clients|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Extended range \(partially advertise\)<sup>1</sup>|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Configurable retry limit|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Configurable unicast data rate|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Configurable broadcast/multicast data rate|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|uAP events|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|DFS radar detection \(leader\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|UNII\_4 channel support|Y|N|Y|Y|Y|N|N|Y|N|Y|
|Host sleep \(W0W\) in band and out-of-band|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|STA ageout \(time out for associated/idle clients\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|NAPI support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Vendor specific ie \(custom IE\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|EasyMesh<sup>1</sup><sup>4</sup>|Y|N|N|Y|Y|N|N|Y|N|Y|
|Wi-Fi Aware \(NAN\)<sup>1</sup>|Y|N|Y|Y|Y|N|N|N|N|N|
|Agile Channel Switching<sup>1</sup>|N|N|Y|N|N|N|N|Y|N|Y|
|Vendor defined TX power config \(TXpower Config V3\)|Y|N|Y|Y|N|N|N|N|N|N|
|Secure Host Interface|N|N|Y|N|N|N|N|N|N|N|

1.  Contact your support representative to use this feature.<br/>
2. Feature enabled by default in software.<br/>
3.  To avoid any version mismatch, copy both the combo firmware and the standalone Wi-Fi firmware to the directory */lib/firmware/nxp/* of the host system.<br/>
4. Feature not supported in this release.

<a name="wifi-direct"></a>
## Wi-Fi Direct

|Feature        |88W9098 PCIe-UART|88W8997 PCIe-UART|AW693 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8997 SDIO-UART|88W8987 SDIO-UART|IW610 SDIO-UART|IW416 SDIO-UART|IW610 USB-USB|
|---------------|-------|-------|-----|-------|-----------|-------|-------|-----|-----|-----|
|**P2P basic functionality**|||||||||||
|WFD Client Mode|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|P2P for Miracast|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|P2P Device Mode|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Simultaneous AP-STA operation \(same channel\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Multiple Wi-Fi MAC|Y|N|Y|Y|N|N|N|N|N|N|
|RF Test Mode functionality|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|TX power config<sup>1</sup>|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Deep sleep on unload|N|N|N|N|Y|N|Y|N|N|N|
|Auto FW recovery on fatal error|Y|Y|Y|Y|Y|Y|Y|Y|Y|N|
|Auto ARP and Ping support|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|Dual MAC Connection Services \(DMCS\)|Y|N|Y|Y|N|N|N|N|N|N|
|SDIO over SPI Support\(SDIO over SPI\)|Y|N|Y|Y|N|N|N|Y|N|Y|
|Packet filtering / Memory Efficient Filtering \(MEF\)|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**P2P basic functionality**|||||||||||
|Autonomous GO mode|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|WFD client mode|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|P2P for Miracast|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|P2P device mode|Y|Y|Y|Y|Y|Y|Y|Y|Y|Y|
|**DRCS**|||||||||||
|DRCS|Y|N|Y|Y|Y|N|N|Y|Y|Y|
|AP – P2P\(Client\)|Y|N|Y|Y|Y|N|Y|Y|Y|Y|
|STA – P2P\(GO\)|Y|N|Y|Y|Y|N|Y|Y|Y|Y|
|AP – P2P\(GO\)|Y|N|Y|Y|Y|N|Y|Y|Y|Y|
|AP-STA-P2P|Y|N|Y|Y|Y|N|N|Y|N|Y|
|AP-AP-STA|Y|N|Y|Y|Y|N|N|Y|N|Y|
|AP – AP \(MBSS\)|Y|N|Y|Y|Y|N|N|Y|N|Y|
|AP – STA|Y|N|Y|Y|Y|N|Y|Y|Y|Y|

1. Contact your support representative to use this feature.

<a name="concurrent-dual-wifi-cdw-mode"></a>
## Concurrent dual Wi-Fi \(CDW\) mode

The section applies to 88W9098 (dual MAC, dual band, dual channel).
Radio 0 always operates in 5 GHz. Radio 1 always operates in 2.4 GHz. One Wi-Fi interface from MAC 1 operates in Radio 0 and one Wi-Fi interface from MAC 2 operates in Radio 1.

- Interfaces of Wi-Fi Radio 0 (MAC 1 - 5 GHz): mlan0, uap0, and wfd0
- Interfaces of Wi-Fi Radio 1 (MAC 2 - 2.4 GHz): mmlan0, muap0, and mwfd0

|mlan0|uap0|wfd0|mmlan0|muap0|mwfd0|Use case|
|-----|----|----|------|-----|-----|--------|
|—|Yes|—|—|Yes|—|AP + AP CDW Mode|
|Yes|—|—|Yes|—|—|STA + STA CDW Mode|
|Yes|—|—|—|Yes|—|AP + STA CDW Mode|
|—|Yes|—|Yes|—|—|AP + STA CDW Mode|

<a name="known-limitations-for-simultaneous-mode-of-operation"></a>
## Known limitations for simultaneous mode of operation

- uAP/P2P-GO beacons are paused unconditionally whenever STA/P2P-GC performs scan and are resumed automatically once the scan is complete.
- Radio control commands, Antenna configuration commands, 802.11d – Country Info are not unified across two interfaces.
- Custom IE Buffers are shared between two interfaces. IE-Buffer Index used by one interface cannot be used by another interface.
- STA can operate only in infrastructure mode.









