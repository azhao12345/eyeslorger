CC = g++

CFLAGS = `pkg-config --cflags opencv`
LDFLAGS = `pkg-config --libs opencv`

all: test

test: test.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o *~ test
