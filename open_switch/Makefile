include .config
include inc.mk

ifeq (.config, $(wildcard .config))
		include .config
endif
#include ./inc.mk
RELEASE_DIR=$(PWD)/release/
CC = gcc

export RELEASE_DIR
export CC

DIRS = Libs Apps 
all:
	@if [ ! -d $(RELEASE_DIR) ]; then \
		echo "Ger RELEASE_DIR = "$(RELEASE_DIR); \
		mkdir $(RELEASE_DIR); \
	fi
	for i in $(DIRS); do \
		$(MAKE) -C $$i || exit 1; \
    done

menuconfig:
	touch .config;make -C Apps/ menuconfig

clean:
	for i in $(DIRS); do\
		$(MAKE) -C $$i clean || exit 1; \
    done

distclean: clean
	rm -rf ./release/
		



