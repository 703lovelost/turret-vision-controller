C = gcc
CFLAGS += -fPIC
CFLAGS += --std=gnu99

all: control-turret

control-turret: parser serialport main.c
	$(C) main.c parser.o serialport.o -o control-turret

parser: parser.c
	$(C) -o parser.o -c parser.c $(CFLAGS)

serialport: serialport.c
	$(C) -o serialport.o -c serialport.c $(CFLAGS)

clean:
	rm -f *.o control-turret