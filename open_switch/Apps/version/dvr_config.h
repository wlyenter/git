/*
 * Automatically generated by make menuconfig: don't edit
 */
#ifndef __DVR_CONFIG_H__
#define __DVR_CONFIG_H__
#ifdef __cplusplus
extern C {
#endif



/*
 * Platform option
 */
#define CONFIG_ARCH_ARM 1
#undef  CONFIG_ARCH_X86
#undef  AVCHIP_3520
#undef  AVCHIP_3531
#undef  AVCHIP_3516
#undef  AVCHIP_3516A
#undef  AVCHIP_81xx
#define AVCHIP_9G45 1
#undef  AVCHIP_NONE

/*
 * Product option
 */
#define PCID "20335000040145H000"
#define VERSION "B01D01"
#define CUUID "00000001000000020000000100000001"

/*
 * Upgrade option
 */
#define UPGRADE_PARTITION_BOOT 1
#define UPGRADE_PARTITION_ROOTFS 1
#undef  UPGRADE_PARTITION_CONFIG
#define UPGRADE_PARTITION_APP 1

/*
 * Make option
 */
#define TOOLS_CHAIN_DIR "/home/"
#define CP_DIR "/home/wly/git/open_switch"
#undef  CONFIG_RELEASE_VERSION



#ifdef __cplusplus
#endif
#endif
