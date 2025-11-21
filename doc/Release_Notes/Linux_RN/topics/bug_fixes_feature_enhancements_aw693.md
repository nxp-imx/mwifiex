[Link to index page](../index.md)

# Bug fixes/feature enhancements

## Firmware version 18.99.2.p145.34 to 18.99.2.p145.35

**Wi-Fi**
- Fixed random kernel crash while performing stress test of automatic firmware recovery.
- In the RFTM test, firmware crash observed on the TX tests when changing the tx power and data rate in loop.

## Firmware version 18.99.2.p145.35 to 18.99.2.p145.70

**Wi-Fi**
- Added CSI - Ambient Motion Index (AMI) feature support.

## Firmware version 18.99.2.p145.70 to 18.99.2.p145.99

**Wi-Fi**
- Command timeouts observed during roaming between two APs while running UDP Tx traffic with Video.
- Kernel crash observed on P2P-AGO when attempting to add two P2P groups operating on the same band and frequency

## Firmware version 18.99.2.p145.99 to 18.99.2.p145.147

**Wi-Fi**
- Resolved kernel panic caused by corrupted rgpower binary.
- Addressed command timeout (0x24) in STAUT during roaming stress between 2.4 GHz and 5 GHz BSS.
- Resolved Wi-Fi firmware hang during concurrent AP and STAUT operation.
- Updated VLP mode TPC for FCC to use perform back-off from customer-certified power limits instead of EIRP limits.
- Enhanced handling of SU and RU TX power table entries to support negative power levels.
- Addressed Wi-Fi throughput drop to 0 Kbps during simultaneous Bluetooth inquiry and WLAN transmission.

**Bluetooth**
- BT Crash After S2R Due to Corrupted UART Packet from Controller
- BLE Link Loss Observed During SCO Connection with HV3 Packet Type
- Role switch from Central to Peripheral fails as DUT does not transmit an ID packet in response to the FHS from Ref1, resulting in role switch fail