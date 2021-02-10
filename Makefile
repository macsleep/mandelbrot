#
# --------------------------------------
#      Mandelbrot Project Makefile
# --------------------------------------
#

OBJ = mandelbrot.o
CFLAGS = -O3 -Wunused-variable -Wno-deprecated-declarations
LDFLAGS =

# MacOS
CFLAGS += -DDarwin
LDFLAGS += -framework OpenGL -framework GLUT

# Linux
# CFLAGS += -DLinux
# LDFLAGS += -lGL -lGLU -lglut

mandelbrot:	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) mandelbrot

