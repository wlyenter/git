include ../.config
include ../inc.mk

SRC = $(wildcard ./src/*.c )
OBJS = $(patsubst %c,%o,$(SRC))

LIBS  = -lJsonrpc -lHash -L$(TCSLIBS_DIR) -lpthread -levent_core -levent_pthreads -lm

CFLAGS +=-I../Libs/jsonrpc/inc/ -I../Libs/hash/inc -g -g3 -Wall

PRO_INC = -I./inc  
all: open_text  

open_text: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 
	mv $@ $(RELEASE_DIR)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(PRO_INC)

menuconfig:
	$(MAKE) -C scripts/lxdialog all
	/bin/bash  scripts/Menuconfig menuconfig.in
	cp .config ../.config

.PHONY:clean
clean:
	rm -f ./src/*.o ./scripts/lxdialog/*.o ./scripts/lxdialog/lxdialog open_text 

