[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 18.99.2.p230.2 to 18.99.7.p308

**WI-Fi**
- Wakeup card timeout occurs during stress testing when the 5 GHz external AP, with STAUT associated, is repeatedly powered on & off.
- Kernel Panic observed with new rgpower with 6E PSD offset tables.
- Enhancement: Added support for 6GHz VLP Channel prioritization.
- Enhancement: Automatic FW recovery is now supported when Out-of-band Independent Reset mechanism is enabled.
- Enhancement to add "mlanutl mlan0 get_sensor_temperature" command to read back RFU temperature sensor value.
- Enhanced Dynamic Country Code functionality for Ex-STA’s which doesn’t support ECSA.
- Fix for a type-casting issue causing TX power to be set to 0 dBm via iw dev when values exceeded 128 dBm.
- Kernel panic observed during TDLS teardown.
- Kernel panic observed during STA connect/disconnect aging tests while repeatedly bringing the uAP interface up and down.
- FW crash caused by Wakeup Card timeout observed during ping and iperf traffic tests between STA and ex-AP.
- PCIe spurious OOB GPIO interrupts caused unintended independent resets on customer platform.
- AW693 nxp-wifi-tz thermal zone reported incorrect temperature value through the Linux thermal framework sysfs interface.
- In-Band reset failure when only a single interface (e.g., AP) was active during driver unload.
- Fix added to ensure proper DMA descriptor synchronization between host and FW.
- FW crash observed randomly when STA connected to 5GHz AP in 80MHz.
- STA transmit power dropped to 0 dBm when connected to certain 6 GHz LPI APs (advertising both Country Information and Power Constraint IEs).
- iw dev settings on the interface for set txpower was reflected on the opposite interface.
- STAUT connected to a 5 GHz external AP experiences a spurious LINK_LOST (reason 0x0) because the off-channel GAS/ANQP query triggered by wpa_supplicant is misinterpreted as a beacon-miss link loss.
- 8th external STA (ex-STA) failed to connect to the AP-UT when the max_sta=8 driver load parameter was configured.
- FW crash seen randomly when STAUT is connected with 5GHz AP.
- FW crash observed in stress test when executing netmon.
- During AP (2GHz) + AP (5GHz) LTE coex operation, when Type0 messages are sent back-to-back for both interfaces without any delay, stability issues like FW crash or Ex-STA disconnect is seen (Issues are NOT seen for single AP interface or if there is slight delay between the back-to-back Type0 messages.)
- Improvement: Wi-Fi Channel avoidance feature working when uAP is enabled using uap1 MAC2 (single AP mode)
- Kernel panic observed due to an S2MPU fault triggered after Suspend-to-RAM (S2R) in the head unit.
- Fixed Coverity reported issues on Wi-Fi Driver.

**Bluetooth/LE**
- Bluetooth FW download issue with Wi-Fi Deep sleep enable.
- Crash observed during LE Audio CIS Connect/Disconnect stress test
- Rare Glitch/distortion observed in WBS/NBS Uplink audio.
- LMP response timeout observed for SCO Disconnect command when HV3 packets are in use
- Firmware hangs after HCI Reset command following establishment of 16 BLE master links with CI = 500 ms
- Controller becomes unresponsive to subsequent commands when OOB IR Config VSC (0xFC0D) is sent twice consecutively
- SCO setup fails with error 0x1B (SCO Offset Rejected) while the link is in Sniff mode (T_sniff = 0x1000, Attempt = 0x0F)
- Command Disallowed (0x0C) error returned when removing a CIG after disconnecting two CIS links
- HCI_LE_Create_CIS command fails with Command Disallowed error (0x0C)
- Connection timeout observed in scatternet scenario where 1st link is having active eSCO and 2nd link in sniff (T_sniff=800).

**Coex**
- LTE Coex. Wi-Fi Channel Avoidance- With 5GHz AP + 2.4GHz AP scenario, CSA did not happen on 2.4GHz when LTE MSG0 was received.
- Wi-Fi TP going low in case of simultaneous A2DP and Inq when COEX mode set to timeshare.
- [LTE Coex] Wi-Fi Channel Avoidance not working for single AP-only mode with uap1 interface.
- WLAN RX throughput optimization when doing Paging/Inq parallelly with other BT profile active
- Wifi Throughput optimization in low Wi-Fi and BT antenna isolation (~15db).