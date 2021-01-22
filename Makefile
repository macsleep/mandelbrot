#
# --------------------------------------
#      Mandelbrot Project Makefile
# --------------------------------------
#

OBJ = mandelbrot.o
CFLAGS = -O3 -Wno-deprecated-declarations
LDFLAGS =

# MacOS
CFLAGS += -DDarwin
LDFLAGS += -framework GLUT -framework OpenGL

# Linux
# CFLAGS += -DLinux
# LDFLAGS += -lGL -lGLU -lglut

mandelbrot:	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) mandelbrot

