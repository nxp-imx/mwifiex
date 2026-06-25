[Link to the index page](../index.md)

# Coexistence

-   [Wi-Fi and Bluetooth coexistence](#wifi-and-bluetooth-coexistence)

-   [Wi-Fi and Bluetooth/802.15.4 coexistence](#wifi-and-bluetooth-802154-coexistence)


## Wi-Fi and Bluetooth coexistence
<a name="wifi-and-bluetooth-coexistence"></a>
|Feature|88W9098 PCIe-UART|AW693 PCIe-UART|IW623 SDIO-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8987 SDIO-UART|IW610 SDIO-UART|IW416 SDIO-UART|IW610 USB-USB|
|-------|-----------------|---------------|---------------|-----------------|---------------------|-----------------|---------------|---------------|-------------|
|**BCA-TDM mode \(shared antenna\)**||||||||||
|STA + Bluetooth Coex|N|N|N|N|Y|Y|N|Y|N|
|STA + Bluetooth LE Coex|N|N|N|N|Y|Y|Y|Y|Y|
|STA + Bluetooth + Bluetooth LE Coex|N|N|N|N|Y|Y|N|Y|N|
|STA + LE Audio with the 3 CIS mono support|N|N|N|N|Y|N|N|N|N|
|AP + Bluetooth Coex|N|N|N|N|Y|Y|N|Y|N|
|AP + Bluetooth LE Coex|N|N|N|N|Y|Y|Y|Y|Y|
|AP + Bluetooth + Bluetooth LE Coex|N|N|N|N|Y|Y|N|Y|N|
|P2P + Bluetooth Coex|N|N|N|N|Y|Y|N|Y|N|
|P2P + Bluetooth LE Coex|N|N|N|N|Y|Y|Y|Y|Y|
|P2P + Bluetooth + Bluetooth LE Coex|N|N|N|N|Y|Y|N|Y|N|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth Coex|Y|Y|Y|Y|N|N|N|N|N|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth LE Coex|Y|Y|Y|Y|N|N|N|N|N|
|**BCA-TDM mode \(separate antenna\)**||||||||||
|STA + Bluetooth Coex|Y|Y|Y|Y|Y|N|N|N|N|
|STA + Bluetooth LE Coex|Y|Y|Y|Y|Y|N|Y|N|Y|
|STA + Bluetooth + Bluetooth LE Coex|Y|Y|Y|Y|Y|N|N|N|N|
|STA + Bluetooth + Bluetooth LE Audio with low isolation|N|N|N|N|Y|N|N|N|N| 
|AP + Bluetooth Coex|Y|Y|Y|Y|Y|N|N|N|N|
|AP + Bluetooth LE Coex|Y|Y|Y|Y|Y|N|N|N|N|
|AP + Bluetooth + Bluetooth LE Coex|Y|Y|Y|Y|Y|N|N|N|N|
|P2P + Bluetooth Coex|Y|Y|Y|Y|Y|N|N|N|N|
|P2P + Bluetooth LE Coex|Y|Y|Y|Y|Y|N|Y|N|Y|
|P2P + Bluetooth + Bluetooth LE Coex|Y|Y|Y|Y|Y|N|N|N|N|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth Coex|Y|Y|Y|Y|N|N|N|N|N|
|AP\(5GHz\) + AP\(5GHz\) + Bluetooth LE Coex|Y|Y|Y|Y|N|N|N|N|N|
|**External coexistence<sup>1</sup>**||||||||||
|External Coex \(hardware interface\)|Y|Y|Y|Y|Y|N|N|N|N|

1. IW611/IW612 chipset features are tested on the i.MX 8M Mini host platform with NXP reference board.

## Wi-Fi and Bluetooth/802.15.4 coexistence
<a name="wifi-and-bluetooth-802154-coexistence"> </a>

|Feature|88W9098 PCIe-UART|AW693 PCIe-UART|IW623 SDIO-UART|88W9098 SDIO-UART|IW611/IW612 SDIO-UART|88W8987 SDIO-UART|IW610 SDIO-UART|IW416 SDIO-UART|IW610 USB-USB|
|-------|-----------------|---------------|---------------|-----------------|---------------------|-----------------|---------------|---------------|-------------|
|**BCA-TDM mode \(separate antenna\)**||||||||||
|STA + Bluetooth + 802.15.4 Coex|N|N|N|N|Y|N|N|N|N|
|STA + Bluetooth LE + 802.15.4 Coex|N|N|N|N|Y|N|Y|N|Y|
|STA + Bluetooth + Bluetooth LE + 802.15.4 Coex|N|N|N|N|Y|N|N|N|N|
|AP + Bluetooth + 802.15.4 Coex|N|N|N|N|Y|N|N|N|N|
|AP + Bluetooth LE + 802.15.4 Coex|N|N|N|N|Y|N|Y|N|Y|
|AP + Bluetooth + Bluetooth LE + 802.15.4 Coex|N|N|N|N|Y|N|N|N|N|
|P2P + Bluetooth + 802.15.4 Coex|N|N|N|N|Y|N|N|N|N|
|P2P + Bluetooth LE + 802.15.4 Coex|N|N|N|N|Y|N|Y|N|Y|
|P2P + Bluetooth + Bluetooth LE + 802.15.4 Coex|N|N|N|N|Y|N|N|N|N|
|**Security**|||||||||||
|Secure Boot|N|Y|Y|N|Y|N|Y|N|Y|

**Note:** When the dual A2DP \(A2DP SRC+SRC &amp; SRC+SNK\) feature is enabled on firmware using vendor-specific commands then it will affect the Wi-Fi throughput until it gets disabled.

1. IW611/IW612 chipset features are tested on the i.MX 8M Mini host platform with NXP reference board.


