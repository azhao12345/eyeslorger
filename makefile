CC = g++

CFLAGS = `pkg-config --cflags opencv`
LDFLAGS = `pkg-config --libs opencv`

all: test

test: test.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

detect_markers: detect_markers.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

create_marker: create_marker.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o *~ test
