CC := arm-none-linux-gnueabihf-gcc
CFLAGS = -I ./ -Wall -Werror
LDFLAGS = -static
OBJS = userspace_i2c_ops.o userspace_ds1307.o
BIN = us_ds1307_driver

default: $(BIN)
.PHONY: default clean

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $*.o

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o $(BIN)

