[Link to index page](../index.md)

# Known issues

**Wi-Fi**
- Known issue with open source wpa_supplicant v2.11- AP fails to start on new DFS channel after Radar Detection.
- Dynamic Bandwidth operation is being optimized
- Channel avoidance operation for LTE Coex is being optimized further

**Bluetooth/BLE**
- BIG Sync Failure observed in BMR with 7.5ms ISO interval
- Distorted audio observe on BMR-1 for Config Sequential STEREO at 48_2 kHz after 2-BIS Suspend & Resume
- During 4 –CIS Streaming , Disconnection with CTO observed with 2 set of Samsung Falaxy Earbuds Pro
- During LE CIS+HFP, Observed HFP Connection drop during HFP + 2-CIS, when 01 EarBud moved OOR & again back to range followed by re-connection & resuming audio stream
- During LE CIS + PBAP + Classic HID, Observed minor audio glitches on CIS SNK while HID & PBAP data is ongoing
- During LE CIS+HFP, CIS sink fails to reconnect to the DUT, when turned OFF/ON particular earbud

