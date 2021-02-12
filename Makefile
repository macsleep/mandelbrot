#
# --------------------------------------
#      Mandelbrot Project Makefile
# --------------------------------------
#

OBJ = mandelbrot.o

# MacOS
CFLAGS = -O3 -Wno-deprecated-declarations -DDarwin
LDFLAGS = -framework OpenGL -framework GLUT

# Linux
# CFLAGS = -O3
# LDFLAGS = -lGL -lGLU -lglut

# HP-UX
# CFLAGS = -O4
# LDFLAGS = -lXmu -lX11 -lGL -lGLU -lglut

mandelbrot:	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) mandelbrot

