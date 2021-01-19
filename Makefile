#
# --------------------------------------
#      Mandelbrot Project Makefile
# --------------------------------------
#

OBJ = mandelbrot.o
CFLAGS = -O3 -Wno-deprecated-declarations
LDFLAGS = -framework GLUT -framework OpenGL
# LDFLAGS = -lGL -lGLU -lglut

mandelbrot:	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) mandelbrot

