[Link to index page](../index.md)

# Known issues

**Wi-Fi**
- NAN (Neighbor Awareness Networking) ranging failure in unified build configuration


**Bluetooth**
- Link loss is happening on Ref1 headphone \(A2DP sink\) device streaming A2DP audio, in the presence of Ref2 mobile \(HFP AG\) device running HFP call.
- A2DP audio glitches heard on Ref1 headphone \(A2DP sink\) device in the presence of HFP call ongoing on ref2 headphone \(HFP Dev\) device.
- A2DP glitches are observed when the DUT is an A2DP source device with peripheral role and in the presence of Bluetooth inquiry in background.
- During dual HFP \(DUT connected with two reference headphone devices \(HFP Dev\)\), when the active call is terminated on any of the slots, or when an active call is set in Privacy mode, some audio distortion is observed in that disconnected slot.

**Wi-Fi, Bluetooth and 802.15.4 \(OpenThread\) coexistence**
- Wi-Fi throughput in presence of OT peak throughput is under optimization
- Wi-Fi scan timeout is observed in the presence of OT-daemon running in the background.
- Wi-Fi command timeout has been observed following the initiation of the OT-daemon, specifically when the CPU2 power save disable command is not utilized and a calibration file is loaded.

**Notes:**

1.  Before loading Bluetooth-only firmware, the Wi-Fi SDIO driver and firmware loading must be required with the calibration data file.
2.  Bluetooth LE Audio features are in phase 1 where only two simultaneous CIS/BIS streams are validated. This feature is validated with i.MX RTOS and not using Linux BSP. Contact your NXP representative for more details


