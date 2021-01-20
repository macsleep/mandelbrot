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

void init(void) {
    glShadeModel(GL_FLAT);
}

void display(void) {
    GLint w, h;

    // window size
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT);

    // Mandelbrot
    glRasterPos2i(0, 0);
    glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // zoom box
    if (drawBox) {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2i(box[0], h - box[1]);
        glVertex2i(box[2], h - box[1]);
        glVertex2i(box[2], h - box[3]);
        glVertex2i(box[0], h - box[3]);
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

    // keep aspect
    w_ratio = (GLdouble) w / (GLdouble) h;
    m_ratio = (mx_max_orig - mx_min_orig) / (my_max_orig - my_min_orig);
    if (w_ratio > m_ratio) {
        mpp = (my_max_orig - my_min_orig) / (GLdouble) h;
        mx_min = mx_min_orig + (mx_max_orig - mx_min_orig - mpp * (GLdouble) w) / 2.0;
        mx_max = mx_min_orig + (mx_max_orig - mx_min_orig + mpp * (GLdouble) w) / 2.0;
        my_max = my_max_orig;
        my_min = my_min_orig;
    } else {
        mx_max = mx_max_orig;
        mx_min = mx_min_orig;
        mpp = (mx_max_orig - mx_min_orig) / (GLdouble) w;
        my_min = my_min_orig + (my_max_orig - my_min_orig - mpp * (GLdouble) h) / 2.0;
        my_max = my_min_orig + (my_max_orig - my_min_orig + mpp * (GLdouble) h) / 2.0;
    }

    // start calculations
    px = 0;
    py = 0;
    glutIdleFunc(idle);
}

void reshape(int w, int h) {
    GLint i, size;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // image buffer
    size = w * h * 4 * sizeof (GLubyte);
    pixels = realloc(pixels, size);
    if(pixels == NULL) {
        fprintf(stderr, "realloc failed\n");
        exit(1);
    }
    for (i = 0; i < size; i++) pixels[i] = 0;

    // calculate
    reset();
}

void pixel2mandel(int px, int py, double *mx, double *my) {
    *mx = ((GLdouble) px / glutGet(GLUT_WINDOW_WIDTH) * (mx_max - mx_min)) + mx_min;
    *my = ((GLdouble) py / glutGet(GLUT_WINDOW_HEIGHT) * (my_max - my_min)) + my_min;
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

void mouse(int button, int state, int x, int y) {
    GLint w, h, tmp;

    // window size
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);

    if (button == GLUT_LEFT_BUTTON) {
        switch (state) {
            case GLUT_DOWN:
                // box corner
                box[0] = x;
                box[1] = y;

                // box corner
                box[2] = x;
                box[3] = y;

                drawBox = GL_TRUE;

                break;
            case GLUT_UP:
                // check zoom
                if(box[0] == box[2] || box[1] == box[3]) break;

                // swap if necessary
                if(box[0] > box[2]) {
                        tmp = box[0];
                        box[0] = box[2];
                        box[2] = tmp;
                }
                if(box[1] > box[3]) {
                        tmp = box[1];
                        box[1] = box[3];
                        box[3] = tmp;
                }

                // convert box to Mandelbrot coordinates
                pixel2mandel(box[0], h - box[1], &mx_min_orig, &my_max_orig);
                pixel2mandel(box[2], h - box[3], &mx_max_orig, &my_min_orig);

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
                mx_min_orig = MX_MIN;
                mx_max_orig = MX_MAX;
                my_min_orig = MY_MIN;
                my_max_orig = MY_MAX;

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
    // box corner
    box[2] = x;
    box[3] = y;

    glutPostRedisplay();
}

void idle(void) {
    GLuint i = 0, imax = 0xfff, p;
    GLdouble x = 0.0, y = 0.0, mx, my, xtmp;

    // pixel to Mandelbrot coordinates
    pixel2mandel(px, py, &mx, &my);

    // iterate
    while ((x*x + y*y <= 2*2) && (i < imax)) {
        xtmp = x*x - y*y + mx;
        y = 2 * x * y + my;
        x = xtmp;
        i++;
    }

    // imax equals black
    if (i == imax) i = 0;

    // set color
    p = (py * glutGet(GLUT_WINDOW_WIDTH) + px)*4 * sizeof(GLubyte);
    pixels[p + 0] = (i & 0x00f) << 4;
    pixels[p + 1] = (i & 0x0f0);
    pixels[p + 2] = (i & 0xf00) >> 4;

    // next pixel
    if (++px >= glutGet(GLUT_WINDOW_WIDTH)) {
        px = 0;
        if (++py >= glutGet(GLUT_WINDOW_HEIGHT)) {
            glutIdleFunc(NULL);
            py = 0;
        }
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

