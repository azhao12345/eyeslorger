CC = g++

CFLAGS = `pkg-config --cflags opencv`
LDFLAGS = `pkg-config --libs opencv`

all: detect_markers create_marker calibrate_camera

detect_markers: detect_markers.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

create_marker: create_marker.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

calibrate_camera: calibrate_camera.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o *~ test
