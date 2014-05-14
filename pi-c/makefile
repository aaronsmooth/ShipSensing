PROG?=main

all: $(PROG)

run: all
	LD_LIBRARY_PATH=/home/pi/libraries/libxbee.libxbee-v3/lib ./$(PROG)

new: clean all

clean:
	-rm $(PROG)

$(PROG): $(PROG).c /home/pi/libraries/libxbee.libxbee-v3/lib/libxbee.so
	gcc $(filter %.c,$^) -g -o $@ -I ../../.. -L /home/pi/libraries/libxbee.libxbee-v3/lib -lxbee -lpthread -lrt 
