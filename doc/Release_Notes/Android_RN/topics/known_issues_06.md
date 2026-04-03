[Link to index page](../index.md)

# Known issues

**Bluetooth**
- Link loss is happening on Ref1 headphone \(A2DP sink\) device streaming A2DP audio, in the presence of Ref2 mobile \(HFP AG\) device running HFP call.
- A2DP audio glitches heard on Ref1 headphone \(A2DP sink\) device in the presence of HFP call ongoing on ref2 headphone \(HFP Dev\) device.
- A2DP glitches are observed when the DUT is an A2DP source device with peripheral role and in the presence of Bluetooth inquiry in background.
- During dual HFP \(DUT connected with two reference headphone devices \(HFP Dev\)\), when the active call is terminated on any of the slots, or when an active call is set in Privacy mode, some audio distortion is observed in that disconnected slot.
- Firmware hang occurring when BLE connection timeout (0x08) is observed.

**Parent topic:** [SD-UART IW611/IW612](../topics/sd-uart_iw612.md)

