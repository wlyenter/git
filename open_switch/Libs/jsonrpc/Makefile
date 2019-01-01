include ../../.config
include ../../inc.mk

RM = rm -f
LDFLAGS =
CFLAGS += -g -Wall

#ROOTPATH = $(shell pwd)

#export CC
#export AR
##export CFLAGS
#export LDFLAGS
#export ARCH_PATH

LDFLAGS =  -lpthread -lm
CFLAGS+= -I../../Apps/inc/ -I./inc $(EXTCFLAGS) -g  -Wall
#got all .o files in .c source dir
SRC_DIRS = ./src 
SOURCES = $(foreach dir,$(SRC_DIRS), $(wildcard $(dir)/*.c))
OBJS = $(SOURCES:.c=.o)
TARGET = libControl.a 

#.PHONY: $(TARGET)

#all: $(TARGET)

#info:
#	@echo CC = $(CC)
#	@echo ARCH_PATH = $(ARCH_PATH)
#	@echo ROOTPATH = $(ROOTPATH)
#	@echo TCSLIBS_DIR = $(TCSLIBS_DIR)
	
$(TARGET):$(OBJS)
	$(AR) crs $@ $(OBJS)
	mkdir -p lib
	mv $@ lib
	echo $(TCSLIBS_DIR)
	mkdir -p $(TCSLIBS_DIR)
	cp lib/$@ $(TCSLIBS_DIR)
	#$(MAKE) -C test

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	$(RM) $(TARGET) lib/$(TARGET)
	rm -f ./src/*.o

distclean: clean

