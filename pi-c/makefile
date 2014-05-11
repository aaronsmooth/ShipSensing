PROG?=main

all: $(PROG)

run: all
	LD_LIBRARY_PATH=../libxbee.libxbee-v3/lib ./$(PROG)

new: clean all

clean:
	-rm $(PROG)

$(PROG): $(PROG).c ../libxbee.libxbee-v3/lib/libxbee.so
	gcc $(filter %.c,$^) -g -o $@ -I ../../.. -L ../../../lib -lxbee -lpthread -lrt
