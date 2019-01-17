#BUILD_VERSION := $(shell unset LANG; svn info | sed -n '/^Revision/s/Revision:[[:space:]]*\([[:digit:]]\+\)/\1/p'; )
BUILD_TIME := $(shell date '+%Y%m%d'; )
ifeq ($(BUILD_VERSION),)
BUILD_VERSION := 999
endif

CHANNEL_NUM= 16


#CC = /usr/local/arm-2010.09/bin/arm-none-linux-gnueabi-gcc
#AR = /usr/local/arm-2010.09/bin/arm-none-linux-gnueabi-ar

CC = gcc
AR = ar

ARCH_PATH = $(CP_DIR)/release/arch_arm
AVCHIP_PATH = 9G45
TCSLIBS_DIR = $(ARCH_PATH)/lib/
export BUILD_VERSION
export BUILD_TIME
export CHANNEL_NUM
export ARCH_PATH
export AVCHIP_PATH
export TCSLIBS_DIR


