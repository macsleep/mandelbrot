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

    // draw zoom box
    if (drawBox) {
        sortbox(&box[0], &box[1]);

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2i(box[1].x1, box[1].y1);
        glVertex2i(box[1].x2, box[1].y1);
        glVertex2i(box[1].x2, box[1].y2);
        glVertex2i(box[1].x1, box[1].y2);
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

    // clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // start calculations
    glutIdleFunc(idle);
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    reset();
}

void push(box4d *box) {
    // increment stack pointer
    stackTop = (stackTop + 1 < STACK_SIZE) ? stackTop + 1 : 0;

    // save coordinates
    memcpy(&stack[stackTop], box, sizeof (box4d));

    // overflow fifo
    if (stackBottom == stackTop) {
        stackBottom = (stackBottom + 1 < STACK_SIZE) ? stackBottom + 1 : 0;
    }
}

int pop(box4d *box) {
    // stack empty
    if (stackTop == stackBottom) return -1;

    // restore coordinates
    memcpy(box, &stack[stackTop], sizeof (box4d));

    // decrement stack pointer
    stackTop = (stackTop > 0) ? stackTop - 1 : STACK_SIZE - 1;

    return 0;
}

void sortbox(box4i *box0, box4i *box1) {
    // x coordinates
    if (box0->x1 > box0->x2) {
        box1->x1 = box0->x2;
        box1->x2 = box0->x1;
    } else {
        box1->x1 = box0->x1;
        box1->x2 = box0->x2;
    }

    // y coordinates
    if (box0->y1 > box0->y2) {
        box1->y1 = box0->y2;
        box1->y2 = box0->y1;
    } else {
        box1->y1 = box0->y1;
        box1->y2 = box0->y2;
    }
}

void pixel2mandel(int px, int py, double *mx, double *my) {
    // convert pixel coordinates to mandelbrot coordinates
    *mx = ((GLdouble) px / glutGet(GLUT_WINDOW_WIDTH) * (actual.x2 - actual.x1)) + actual.x1;
    *my = ((GLdouble) py / glutGet(GLUT_WINDOW_HEIGHT) * (actual.y2 - actual.y1)) + actual.y1;
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;
        case 'R':
        case 'r':
            // check if reset needed
            if (master.x1 == MX_MIN && master.y1 == MY_MIN &&
                    master.x2 == MX_MAX && master.y2 == MY_MAX) break;

            // reset stack
            stackBottom = 0;
            stackTop = 0;

            // restore defaults
            master.x1 = MX_MIN;
            master.y1 = MY_MIN;
            master.x2 = MX_MAX;
            master.y2 = MY_MAX;

            reset();
            break;
        default:
            break;
    }
}

void mouse(int button, int state, int x, int y) {
    GLint w, h;
    GLdouble x_center, y_center, x_half, y_half;

    // window size
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);

    if (button == GLUT_LEFT_BUTTON) {
        switch (state) {
            case GLUT_DOWN:
                // two corners
                box[0].x1 = x;
                box[0].y1 = h - y;
                box[0].x2 = x;
                box[0].y2 = h - y;

                drawBox = GL_TRUE;
                break;
            case GLUT_UP:
                // save
                push(&master);

                if (box[0].x1 == box[0].x2 || box[0].y1 == box[0].y2) {
                    pixel2mandel(box[0].x1, box[0].y1, &x_center, &y_center);

                    x_half = (actual.x2 - actual.x1) / 2.0;
                    y_half = (actual.y2 - actual.y1) / 2.0;

                    // center around mouse click
                    master.x1 = x_center - x_half;
                    master.y1 = y_center - y_half;
                    master.x2 = x_center + x_half;
                    master.y2 = y_center + y_half;
                } else {
                    sortbox(&box[0], &box[1]);

                    // zoom in on box
                    pixel2mandel(box[1].x1, box[1].y1, &master.x1, &master.y1);
                    pixel2mandel(box[1].x2, box[1].y2, &master.x2, &master.y2);
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
                if (pop(&master)) break;
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
    // second corner
    box[0].x2 = x;
    box[0].y2 = glutGet(GLUT_WINDOW_HEIGHT) - y;

    // draw zoom box
    glutPostRedisplay();
}

void idle(void) {
    GLuint i = 0, imax = 0xfff;
    GLubyte r, g, b;
    GLdouble x = 0.0, y = 0.0, mx, my, tmp;

    pixel2mandel(px, py, &mx, &my);

    // iterate mandelbrot function
    while ((x * x + y * y <= 2 * 2) && (i < imax)) {
        tmp = x * x - y * y + mx;
        y = 2 * x * y + my;
        x = tmp;
        i++;
    }

    // imax equals black
    if (i >= imax) i = 0;

    // pixel color
    r = (i & 0x00f) << 4; // red
    g = (i & 0x0f0);      // green
    b = (i & 0xf00) >> 4; // blue

    // draw pixel
    glColor3ub(r, g, b);
    glBegin(GL_POINTS);
    glVertex2i(px, py);
    glEnd();

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

