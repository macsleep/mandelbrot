/*
  Copyright 2021 Jan Schlieper

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
 */

#include "mandelbrot.h"
#include "stack.h"

static GLuint px = 0, py = 0;
static GLubyte *pixels;
static box4d actual, master = { MX_MIN, MY_MIN, MX_MAX, MY_MAX };
static GLboolean drawBox = GL_FALSE;
static box4i box = { 0, 0, 0, 0 };

void init(void) {
    glShadeModel(GL_FLAT);
}

void display(void) {
    GLint w, h;

    // window size
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT);

    // draw mandelbrot
    glRasterPos2i(0, 0);
    glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // draw zoom box
    if (drawBox) {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2i(box.x1, (h - box.y1));
        glVertex2i(box.x2, (h - box.y1));
        glVertex2i(box.x2, (h - box.y2));
        glVertex2i(box.x1, (h - box.y2));
        glEnd();
    }

    glFlush();
}

void reset(void) {
    GLint w, h;
    GLdouble w_ratio, m_ratio, mpp;

    // window size
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);

    // respect aspect ratio
    w_ratio = (GLdouble) w / (GLdouble) h;
    m_ratio = (master.x2 - master.x1) / (master.y2 - master.y1);
    if (w_ratio > m_ratio) {
        mpp = (master.y2 - master.y1) / (GLdouble) h * (GLdouble) w;
        actual.x1 = master.x1 + (master.x2 - master.x1 - mpp) / 2.0;
        actual.x2 = master.x1 + (master.x2 - master.x1 + mpp) / 2.0;
        actual.y2 = master.y2;
        actual.y1 = master.y1;
    } else {
        actual.x2 = master.x2;
        actual.x1 = master.x1;
        mpp = (master.x2 - master.x1) / (GLdouble) w * (GLdouble) h;
        actual.y1 = master.y1 + (master.y2 - master.y1 - mpp) / 2.0;
        actual.y2 = master.y1 + (master.y2 - master.y1 + mpp) / 2.0;
    }

    // reset coordinates
    px = 0;
    py = 0;

    // start calculations
    glutIdleFunc(idle);
}

void reshape(int w, int h) {
    GLint i, size;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // allocate buffer
    size = w * h * 4 * sizeof (GLubyte);
    pixels = realloc(pixels, size);
    if (pixels == NULL) {
        fprintf(stderr, "realloc failed\n");
        exit(1);
    }

    // zero buffer
    for (i = 0; i < size; i++) pixels[i] = 0;

    reset();
}

void pixel2mandel(int px, int py, double *mx, double *my) {
    // convert pixel coordinates to mandelbrot coordinates
    *mx = ((GLdouble) px / glutGet(GLUT_WINDOW_WIDTH) * (actual.x2 - actual.x1)) + actual.x1;
    *my = ((GLdouble) py / glutGet(GLUT_WINDOW_HEIGHT) * (actual.y2 - actual.y1)) + actual.y1;
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            free(pixels);
            exit(0);
            break;
        default:
            break;
    }
}

void mouse(int button, int state, int x, int y) {
    GLint w, h, tmp;
    GLdouble x_center, y_center, x_half, y_half;

    // window size
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);

    if (button == GLUT_LEFT_BUTTON) {
        switch (state) {
            case GLUT_DOWN:
                // first corner
                box.x1 = x;
                box.y1 = y;

                // second corner
                box.x2 = x;
                box.y2 = y;

                drawBox = GL_TRUE;
                break;
            case GLUT_UP:
                if (box.x1 == box.x2 || box.y1 == box.y2) {
                    pixel2mandel(box.x1, (h - box.y1), &x_center, &y_center);

                    x_half = (actual.x2 - actual.x1) / 2.0;
                    y_half = (actual.y2 - actual.y1) / 2.0;

                    // center around mouse click
                    master.x1 = x_center - x_half;
                    master.y1 = y_center - y_half;
                    master.x2 = x_center + x_half;
                    master.y2 = y_center + y_half;
                } else {
                    // swap if necessary
                    if (box.x1 > box.x2) {
                        tmp = box.x1;
                        box.x1 = box.x2;
                        box.x2 = tmp;
                    }
                    if (box.y1 > box.y2) {
                        tmp = box.y1;
                        box.y1 = box.y2;
                        box.y2 = tmp;
                    }

                    // zoom in on box
                    pixel2mandel(box.x1, (h - box.y1), &master.x1, &master.y2);
                    pixel2mandel(box.x2, (h - box.y2), &master.x2, &master.y1);
                }

                drawBox = GL_FALSE;
                reset();
                break;
            default:
                break;
        }
    }

    if (button == GLUT_RIGHT_BUTTON) {
        switch (state) {
            case GLUT_UP:
                // restore defaults
                master.x1 = MX_MIN;
                master.y1 = MY_MIN;
                master.x2 = MX_MAX;
                master.y2 = MY_MAX;

                drawBox = GL_FALSE;
                reset();
                break;
            default:
                break;
        }
    }

    glutPostRedisplay();
}

void motion(int x, int y) {
    // update second corner
    box.x2 = x;
    box.y2 = y;

    // draw zoom box
    glutPostRedisplay();
}

void idle(void) {
    GLuint i = 0, imax = 0xfff, p;
    GLdouble x = 0.0, y = 0.0, mx, my, xtmp;

    pixel2mandel(px, py, &mx, &my);

    // iterate mandelbrot function
    while ((x*x + y*y <= 2*2) && (i < imax)) {
        xtmp = x*x - y*y + mx;
        y = 2 * x * y + my;
        x = xtmp;
        i++;
    }

    // imax equals black
    if (i >= imax) i = 0;

    // pixel pointer
    p = (py * glutGet(GLUT_WINDOW_WIDTH) + px) * 4 * sizeof (GLubyte);

    // pixel color
    pixels[p + 0] = (i & 0x00f) << 4; // red
    pixels[p + 1] = (i & 0x0f0);      // green
    pixels[p + 2] = (i & 0xf00) >> 4; // blue

    // next pixel
    if (++px >= glutGet(GLUT_WINDOW_WIDTH)) {
        px = 0;
        if (++py >= glutGet(GLUT_WINDOW_HEIGHT)) {
            py = 0;

            // stop calculations
            glutIdleFunc(NULL);
        }

        // new mandelbrot line
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow(argv[0]);
    init();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}

