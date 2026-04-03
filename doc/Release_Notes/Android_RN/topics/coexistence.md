[Link to index page](../index.md)

# Coexistence

- [Wi-Fi and Bluetooth coexistence](#wifi-and-bluetooth-coex)
- [P2P-GO and STA simultaneous mode operations](#p2pgo-sta-simultaneous)
    - [How to apply changes](#apply-changes)

<a name="wifi-and-bluetooth-coex"></a>
## Wi-Fi and Bluetooth coexistence

Feature list for Wi-Fi and Bluetooth coexistence

|Features|88W9098 PCIe-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8987 SDIO-UART|
|--------|-----------------|-----------------|---------------------|-----------------|
|**BCA-TDM mode \(shared antenna\)**|||||||||||||||||||
|AP + Bluetooth + Bluetooth LE Coex|N|N|Y|Y|
|P2P + Bluetooth Coex|N|N|Y|Y|
|P2P + Bluetooth LE Coex|N|N|Y|Y|
|P2P + Bluetooth + Bluetooth LE Coex|N|N|Y|Y|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth Coex|N|N|Y|N|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth LE Coex|N|N|Y|N|
|**BCA-TDM mode \(separate antenna\)**||||||||||||||||||
|STA + Bluetooth Coex|Y|Y|Y|N|
|STA + Bluetooth LE Coex|Y|Y|Y|N|<sup>1</sup>|
|STA + Bluetooth + Bluetooth LE Coex|Y|Y|Y|N|<sup>1</sup>|
|AP + Bluetooth Coex|Y|Y|Y|N|<sup>1</sup>|
|AP + Bluetooth LE Coex|Y|Y|Y|N|<sup>1</sup>|
|AP + Bluetooth + Bluetooth LE Coex|Y|Y|Y|N|Ysup>1</sup>|
|P2P + Bluetooth Coex|Y|Y|Y|N|<sup>1</sup>|
|P2P + Bluetooth LE Coex|Y|Y|Y|N|<sup>1</sup>|
|P2P + Bluetooth + Bluetooth LE Coex|Y|Y|Y|N|<sup>1</sup>|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth Coex|Y|Y|Y|N|<sup>1</sup>|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth LE Coex|Y|Y|Y|N|<sup>1</sup>|
|**External coex**||||||||||||||||||
|External Coex \(hardware interface\)|Y|Y|N|Y|

1. Features are tested on the i.MX8M Mini Host platform with NXP reference board.

<a name="p2pgo-sta-simultaneous"></a>
## P2P-GO and STA simultaneous mode operations

In this case, P2P-GO and STA modes are active simultaneously where STA has connected to an external AP. If the external-AP switches channel, the P2P-GO stops in case of i.MX Android. The Wi-Fi firmware does not support Dual Channel Radio Concurrency. It means P2P-GO and STA cannot stay on two different channels simultaneously.

In Android, there is only a single wpa\_supplicant instance. It would disable the least prioritized interface \(P2P\) and stop the P2P. In this wpa\_supplicant behavior, the firmware does not get a chance to move the P2P-GO to the same channel as STA.

**Workaround:** The variable `num_multichan_concurrent` can be assigned value 2 in the function `sme_send_authentication` of the file "sme.c" \[`filepath: android_build/external/wpa_supplicant_8/wpa_supplicant/`\] in wpa\_supplicant source.

After this change, supplicant will not stop the P2P-GO and allow the firmware to move P2P-GO on the same channel as STA.

<a name="apply-changes"></a>
### How to apply changes

In wpa\_supplicant source, the function `wiphy_info_iface_comb_process` at line 195 of file `src/drivers/driver_nl80211_capa.c`, the `num_channels` variable should be set to 2.

```
if (combination_has_p2p && combination_has_mgd) {
            -        unsigned int num_channels =
            -               nla_get_u32(tb_comb[NL80211_IFACE_COMB_NUM_CHANNELS]);
            +      unsigned int num_channels = 2;
```

After you have implemented the changes described above, build wpa\_supplicant. Now, you can see P2P-GO on the same channel as STA when external-AP switches the channel.

**Parent page:**[Feature lists](../topics/feature_lists.md)


