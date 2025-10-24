
README describes the wireless hotfix release based on the Q3-2025 release "lf-6.12.34-2.1.0", which includes a Wi-Fi driver and firmware binary files.

Driver tag/branch: lf-6.12.34_2.1.0_hotfix
Firmware tag/branch: lf-6.12.34_2.1.0_hotfix

Driver version: 540.p17

[Issues Fixes]

1. FwImage_9098_PCIE

	FW version: From 17.92.1.p149.76 to 17.92.1.p149.77 
	Wi-Fi Fixes:
		 - Firmware dump triggered automatically during profile switch due to WL_AHB_TIMEOUT on DUTSTA.
		 - Unable to set the txratecfg settings when antcfg is configured on rtap interface.
		 - Incorrect noise floor values observed in open environment conditions.

2. FwImage_9098_SD

	FW version: From 17.92.1.p149.76 to 17.92.1.p149.77 
	Wi-Fi Fixes:
		 - Firmware dump triggered automatically during profile switch due to WL_AHB_TIMEOUT on DUTSTA.
		 - Unable to set the txratecfg settings when antcfg is configured on rtap interface.
		 - Firmware dump triggered during suspend-resume cycle when gtk_rekey_offload=2 is set.
		 - Incorrect noise floor values observed in open environment conditions.

2. FwImage_8997/FwImage_8997_SD

	FW version: From 16.92.21.p153.4 to 16.92.21.p153.5
	Wi-Fi Fixes:
		 - p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

3. FwImage_IW612_SD

	FW version: From 18.99.3.p26.10 to 18.99.3.p26.11
	Wi-Fi Fixes:
		 - False secondary GI detection and throughput drop observed when the duplicate RTS feature is enabled.
		 - uAP and STA fail to perform ranging in associated mode under FTM.
		 - Added validations for bandwidth configurations to avoid user errors in FTM Request.
	Bluetooth Fixes:
		 - Fixed issue where ISCAN/PSCAN re-enable was ineffective due to immediate disable after connection.
	Zigbee:
		 - Integrated DSR fixes in ZB_CLI for Zigbee Coordinator on Linux.

4. FwImage_IW610_SD/FwImage_IW610_USB

	FW version: From 18.25.5.p73 to 18.25.5.p76
	Wi-Fi Fixes:
		 - Wi-Fi firmware hang observed during AP + STAUT concurrent operation scenario.
	Wi-Fi and 15.4 Coex Fixes:
		 - Low OT throughput observed on IW610 when Wi-Fi is connected (idle) and Thread is active.

5. FwImage_AW693_PCIE

	FW version: From 18.99.2.p145.99 to 18.99.2.p145.124
	Bluetooth Fixes:
		 - BLE power not changing when 3f 87 command issued 2nd time with different power.
		 - During 1-BIS, Minor Audio Glitches observed for Config Stereo Sequential/Interleaved at 44_2 kHz.
		 - FW crash occurred, when CIS request is not accepted by Sink.

6. FwImage_8987

	FW version: From 16.92.21.p153.4 to 16.92.21.p153.5
	Wi-Fi Fixes:
		 - p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

7. FwImage_IW416

	FW version: From 16.92.21.p153.4 to 16.92.21.p153.5
	Wi-Fi Fixes:
		 - p2p_find stops immediately upon receiving a connection request from the peer device, but DUT fails to establish the connection.

[Known Issue]

1. FwImage_IW612_SD

	Wi-Fi Issues:
		- NAN Discovery is failing when initiated from a third-party reference device (Phone)
	Bluetooth Issues:
		- Link loss is happening on Ref1 headphone (A2DP sink) device streaming A2DP audio, in the presence of Ref2 mobile (HFP AG) device running   		  HFP call.
		- A2DP audio glitches heard on ref1 headphone (A2DP sink) device, in the presence of HFP call ongoing on ref2 headphone (HFP Dev) device.
		- A2DP glitches are observed while DUT is A2DP source device and peripheral role, in presence of BT inquiry in background.
		- During Dual HFP (DUT connected with 2 reference headphone (HFP Dev) devices), when the active call is terminated on any of the slots or 		  active call put in Privacy mode then audio distortion is observed in that disconnected slot.
	Wi-Fi and 802.15.4 (Open Thread) Coex Issues:
		- Wi-Fi throughput in presence of OT peak throughput is under optimization
		- Wi-Fi scan timeout observed in presence of OT-daemon running in background.
		- Wi-Fi command timeout has been observed following the initiation of the OT-daemon, specifically when the CPU2 power save disable command 		  is not utilized and a calibration file is loaded.

2. FwImage_8987

	Wi-Fi Issues:
		- Tx data path stuck seen for 1 second in PMF enabled case when DELBA command is received.
	
