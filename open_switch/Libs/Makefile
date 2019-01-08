include ../.config
include ../inc.mk

export CC
export AR

DIRS = jsonrpc hash

all:
	for i in $(DIRS); do \
		$(MAKE) -C $$i || exit 1; \
	done
clean:
	for i in $(DIRS); do \
		$(MAKE) -C $$i clean || exit 1; \
	done

