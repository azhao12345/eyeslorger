CC = g++

CFLAGS = `pkg-config --cflags opencv`
LDFLAGS = `pkg-config --libs opencv`
INCLUDE =  -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBS = -lGLEW -lGL -lGLU -lglut -lm

all: test demo

test: test.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

demo: opengl.o detect_markers.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $^ -o $@ $(LDFLAGS)

opengl.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) -c -o $@ OpenGL_Demo/opengl_demo.cpp

clean:
	rm -f *.o *~ test
