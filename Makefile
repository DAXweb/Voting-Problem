CFLAGS=-g -Wall
LDFLAGS=-Wl,-gc-sections -Wl,-relax
CC=gcc
TARGET=voting
OBJECT_FILES=voting.o

all: $(TARGET)

clean:
	rm -f *.o *.hex *.obj *.hex


%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@
