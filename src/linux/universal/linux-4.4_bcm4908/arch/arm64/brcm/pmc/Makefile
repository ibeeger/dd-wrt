#/*
#<:copyright-BRCM:2013:DUAL/GPL:standard 
#
#   Copyright (c) 2013 Broadcom 
#   All Rights Reserved
#
#Unless you and Broadcom execute a separate written software license
#agreement governing use of this software, this software is licensed
#to you under the terms of the GNU General Public License version 2
#(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
#with the following added to such license:
#
#   As a special exception, the copyright holders of this software give
#   you permission to link this software with independent modules, and
#   to copy and distribute the resulting executable under terms of your
#   choice, provided that you also meet, for each linked independent
#   module, the terms and conditions of the license of that module.
#   An independent module is a module which is not derived from this
#   software.  The special exception does not apply to any modifications
#   of the software.
#
#Not withstanding the above, under no circumstances may you combine
#this software in any way with any other Broadcom software provided
#under a license other than the GPL, without Broadcom's express prior
#written consent.
#
#:> 
#*/

BRCM_CHIP=4908
EXTRA_CFLAGS += -DWL4908
EXTRA_CFLAGS	+= -I$(srctree)/include/asm-generic -I$(srctree)/arch/arm64/brcm/include/bcm963xx -I$(srctree)/arch/arm64/brcm/include/pmc  -I$(srctree)/arch/arm64/brcm/include/rdpa  -I$(srctree)/arch/arm64/brcm/include/shared  -I$(srctree)/arch/arm64/brcm/pmc



# remove optimization for debug 
#EXTRA_CFLAGS += -O0 -g
EXTRA_CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/$(BRCM_BOARD)
EXTRA_CFLAGS += -I$(INC_BRCMSHARED_PUB_PATH)/pmc
obj-y	 := pmc_drv.o
obj-$(CONFIG_BCM96838) += clk_rst.o pmc_pcie.o pmc_sysfs.o
obj-$(CONFIG_BCM963138) += pmc_cpu_core.o clk_rst.o pmc_switch.o pmc_sar.o pmc_pcie.o pmc_sysfs.o pmc_usb.o pmc_dsl.o pmc_sata.o pmc_apm.o pmc_rdp.o pmc_neon.o
obj-$(CONFIG_BCM963148) += clk_rst.o pmc_switch.o pmc_sar.o pmc_pcie.o pmc_sysfs.o pmc_usb.o pmc_dsl.o pmc_sata.o pmc_apm.o pmc_rdp.o
obj-$(CONFIG_BCM963381) += pmc_switch.o pmc_sar.o pmc_pcie.o pmc_usb.o pmc_dsl.o pmc_sysfs.o pmc_pcm.o
obj-$(CONFIG_BCM96848) += pmc_pcie.o pmc_sysfs.o clk_rst.o pmc_pcm.o pmc_usb.o
obj-$(CONFIG_BCM94908) += clk_rst.o pmc_pcie.o pmc_switch.o pmc_sysfs.o pmc_fpm.o pmc_rdp.o pmc_usb.o pmc_sata.o pmc_pcm.o
obj-$(CONFIG_BCM96858) += clk_rst.o pmc_sysfs.o pmc_xrdp.o pmc_lport.o pmc_wan.o pmc_pcie.o pmc_pcm.o pmc_usb.o pmc_sata.o

-include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s
