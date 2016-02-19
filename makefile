CC = g++

CFLAGS = `pkg-config --cflags opencv` -g
LDFLAGS = `pkg-config --libs opencv` -lpng
INCLUDE =  -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBS = -lGLEW -lGL -lGLU -lglut -lm

all: test demo

test: test.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

detect_markers: detect_markers.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $^ -o $@ $(LDFLAGS)

demo: opengl.o readpng.o glFunctions.o inputpipe.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $^ -o $@ $(LDFLAGS)

scenedemo: scenedemo.o inputpipe.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $^ -o $@ $(LDFLAGS)

opengl.o: OpenGL_Demo/opengl_demo.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) -c -o $@ OpenGL_Demo/opengl_demo.cpp

scenedemo.o: scene_demo/opengl_demo.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) -c -o $@ scene_demo/opengl_demo.cpp

clean:
	rm -f *.o *~ test
