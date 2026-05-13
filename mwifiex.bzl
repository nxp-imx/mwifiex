# SPDX-License-Identifier: GPL-2.0
# Copyright 2026 NXP
#
# Bazel build configuration for NXP MWIFIEX WiFi drivers
# Converted from Makefile - analyzed with kernel .config
#
# Kernel config used for analysis:
#   CONFIG_64BIT=y
#   CONFIG_WIRELESS_EXT=y
#   CONFIG_WEXT_PRIV=y
#   CONFIG_CFG80211=m
#   CONFIG_PROC_FS=y

load("//build/kernel/kleaf:kernel.bzl", "ddk_module", "kernel_module_group")

# =============================================================================
# Compiler Flags (from Makefile analysis)
# =============================================================================

# Base flags - always needed
_BASE_COPTS = [
    "-DLINUX",
    "-DMLAN_64BIT",  # CONFIG_64BIT=y
    "-DMLAN_RELEASE_VERSION=\"543.p8\"",
    "-DMLAN_EXT_RELEASE_VERSION=\"543.p8\"",
    "-DREL_MILESTONE=\"\"",
    "-DFPNUM=\"92\"",
    "-DDEBUG_LEVEL1",  # CONFIG_DEBUG=1
]

# Android specific flags (CONFIG_IMX_SUPPORT=y, CONFIG_ANDROID_KERNEL=y)
_ANDROID_COPTS = [
    "-DANDROID",
    "-DANDROID_KERNEL",
    "-DIMX_SUPPORT",
    "-DIMX_ANDROID",
    "-Wno-implicit-fallthrough",
    "-DANDROID_SDK_VERSION=36",
]

# Feature flags (from Makefile defaults)
_FEATURE_COPTS = [
    "-DSTA_SUPPORT",       # CONFIG_STA_SUPPORT=y
    "-DREASSOCIATION",     # CONFIG_REASSOCIATION=y
    "-DUAP_SUPPORT",       # CONFIG_UAP_SUPPORT=y
    "-DWIFI_DIRECT_SUPPORT",  # CONFIG_WIFI_DIRECT_SUPPORT=y
    "-DMFG_CMD_SUPPORT",   # CONFIG_MFG_CMD_SUPPORT=y
    "-DSDIO_SUSPEND_RESUME",  # CONFIG_SDIO_SUSPEND_RESUME=y
    "-DMULTI_CHAN_SUPPORT",   # CONFIG_MULTI_CHAN_SUPPORT=y
    "-DDFS_TESTING_SUPPORT",  # CONFIG_DFS_TESTING_SUPPORT=y
    "-DDUMP_TO_PROC",      # CONFIG_DUMP_TO_PROC=y (forced by ANDROID_KERNEL)
]

# CFG80211/WEXT flags (from kernel)
_WIRELESS_COPTS = [
    "-DSTA_CFG80211",   # CONFIG_CFG80211=m
    "-DUAP_CFG80211",   # CONFIG_CFG80211=m
    "-DSTA_WEXT",       # CONFIG_WIRELESS_EXT=y && CONFIG_WEXT_PRIV=y
    "-DUAP_WEXT",       # CONFIG_WIRELESS_EXT=y && CONFIG_WEXT_PRIV=y
]

# Bus support flags
_BUS_COPTS = [
    "-DSDIO",      # CONFIG_SDIO=y (enabled by SD* chips)
    "-DSDIO_MMC",  # Always with SDIO
    "-DPCIE",      # CONFIG_PCIE=y (enabled by PCIE* chips)
    # Note: USB not enabled for i.MX Android builds
]

# Chip support flags (from Makefile for i.MX Android)
# Note: SD8978 is disabled when ANDROID_BUILD=1
_CHIP_COPTS = [
    # SDIO chips
    "-DSD8987",    # CONFIG_SD8987=y
    "-DSD9177",    # CONFIG_SD9177=y
    "-DSD9098",    # CONFIG_SD9098=y
    "-DSDIW610",   # CONFIG_SDIW610=y
    # PCIe chips
    "-DPCIE9098",  # CONFIG_PCIE9098=y
    "-DPCIEAW693", # CONFIG_PCIEAW693=y
]

# Combined compiler options
MWIFIEX_COPTS = _BASE_COPTS + _ANDROID_COPTS + _FEATURE_COPTS + _WIRELESS_COPTS + _BUS_COPTS + _CHIP_COPTS

# =============================================================================
# Source files (from Makefile MLANOBJS/MOALOBJS analysis)
# =============================================================================

# mlan.ko sources - core WLAN driver
MLAN_SRCS = [
    # Core files (always built)
    "mlan/mlan_shim.c",
    "mlan/mlan_init.c",
    "mlan/mlan_txrx.c",
    "mlan/mlan_cmdevt.c",
    "mlan/mlan_misc.c",
    "mlan/mlan_cfp.c",
    "mlan/mlan_module.c",
    "mlan/mlan_wmm.c",
    "mlan/mlan_11n_aggr.c",
    "mlan/mlan_11n_rxreorder.c",
    "mlan/mlan_11n.c",
    "mlan/mlan_11ac.c",
    "mlan/mlan_11ax.c",
    "mlan/mlan_11d.c",
    "mlan/mlan_11h.c",
    # Bus support (CONFIG_SDIO=y, CONFIG_PCIE=y)
    "mlan/mlan_sdio.c",
    "mlan/mlan_pcie.c",
    # STA support (CONFIG_STA_SUPPORT=y)
    "mlan/mlan_meas.c",
    "mlan/mlan_scan.c",
    "mlan/mlan_sta_ioctl.c",
    "mlan/mlan_sta_rx.c",
    "mlan/mlan_sta_tx.c",
    "mlan/mlan_sta_event.c",
    "mlan/mlan_sta_cmd.c",
    "mlan/mlan_sta_cmdresp.c",
    "mlan/mlan_join.c",
    # UAP support (CONFIG_UAP_SUPPORT=y)
    "mlan/mlan_uap_ioctl.c",
    "mlan/mlan_uap_cmdevent.c",
    "mlan/mlan_uap_txrx.c",
]

# moal.ko sources - Linux interface driver
MOAL_SRCS = [
    # Core files (always built)
    "mlinux/moal_main.c",
    "mlinux/moal_ioctl.c",
    "mlinux/moal_shim.c",
    "mlinux/moal_eth_ioctl.c",
    "mlinux/moal_init.c",
    # UAP support (CONFIG_UAP_SUPPORT=y)
    "mlinux/moal_uap.c",
    # PROC_FS support (CONFIG_PROC_FS=y)
    "mlinux/moal_proc.c",
    "mlinux/moal_debug.c",
    # Bus support (CONFIG_SDIO=y, CONFIG_PCIE=y)
    "mlinux/moal_sdio_mmc.c",
    "mlinux/moal_pcie.c",
    # CFG80211 support (CONFIG_CFG80211=m -> STA_CFG80211, UAP_CFG80211)
    "mlinux/moal_cfg80211.c",
    "mlinux/moal_cfg80211_util.c",
    "mlinux/moal_sta_cfg80211.c",
    "mlinux/moal_uap_cfg80211.c",
    # WEXT support (CONFIG_WIRELESS_EXT=y, CONFIG_WEXT_PRIV=y -> STA_WEXT, UAP_WEXT)
    "mlinux/moal_priv.c",
    "mlinux/moal_wext.c",
    "mlinux/moal_uap_priv.c",
    "mlinux/moal_uap_wext.c",
]

# =============================================================================
# Module definition macro
# =============================================================================

def define_mwifiex(name_suffix, kernel_build, kernel_headers):
    """Define mwifiex modules for a specific kernel build.

    Args:
        name_suffix: Suffix for target names (e.g., "imx8mp", "imx")
        kernel_build: Label for kernel_build target (e.g., "//kernel_imx:imx8mp")
        kernel_headers: Label for kernel headers (e.g., "//kernel_imx:all_headers_aarch64")
    """
    mlan_name = "mlan_" + name_suffix
    moal_name = "moal_" + name_suffix
    group_name = "mwifiex_modules_" + name_suffix

    # mlan.ko - MLAN core driver
    ddk_module(
        name = mlan_name,
        out = "mlan.ko",
        srcs = MLAN_SRCS,
        hdrs = [":mwifiex_headers"],
        copts = MWIFIEX_COPTS,
        kernel_build = kernel_build,
        deps = [kernel_headers],
    )

    # moal.ko - MOAL Linux interface driver
    # Depends on mlan.ko for symbols
    ddk_module(
        name = moal_name,
        out = "moal.ko",
        srcs = MOAL_SRCS,
        hdrs = [":mwifiex_headers"],
        copts = MWIFIEX_COPTS,
        kernel_build = kernel_build,
        deps = [
            ":" + mlan_name,
            kernel_headers,
        ],
    )

    kernel_module_group(
        name = group_name,
        srcs = [
            ":" + mlan_name,
            ":" + moal_name,
        ],
    )
