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
    int width_sub, height_sub, skip_pixels, skip_rows;

    if (clearBox) {
        skip_rows = box2.y1;
        skip_pixels = box2.x1 - 1;
        width_sub = box2.x2 - box2.x1 + 1;
        height_sub = box2.y2 - box2.y1 + 1;

        // clear previous zoom
        glRasterPos2i(skip_pixels, skip_rows);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, skip_rows);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_pixels);
        glDrawPixels(width_sub, height_sub, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

        clearBox = GL_FALSE;
    }

    if (drawBox) {
        window2pixel(&box1, &box2);

        // draw zoom box
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2i(box2.x1, box2.y1);
        glVertex2i(box2.x2, box2.y1);
        glVertex2i(box2.x2, box2.y2);
        glVertex2i(box2.x1, box2.y2);
        glEnd();

        clearBox = GL_TRUE;
    }

    glFlush();
}

void reset(void) {
    GLdouble w_ratio, m_ratio, mpp;

    // respect aspect ratio
    w_ratio = (GLdouble) width / (GLdouble) height;
    m_ratio = (master.x2 - master.x1) / (master.y2 - master.y1);
    if (w_ratio > m_ratio) {
        mpp = (master.y2 - master.y1) / (GLdouble) height * (GLdouble) width;
        actual.x1 = master.x1 + (master.x2 - master.x1 - mpp) / 2.0;
        actual.x2 = master.x1 + (master.x2 - master.x1 + mpp) / 2.0;
        actual.y2 = master.y2;
        actual.y1 = master.y1;
    } else {
        actual.x2 = master.x2;
        actual.x1 = master.x1;
        mpp = (master.x2 - master.x1) / (GLdouble) width * (GLdouble) height;
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
    GLint size, i;

    // save for global use
    width = w;
    height = h;

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // allocate buffer
    size = width * height * 4 * sizeof (GLubyte);
    pixels = realloc(pixels, size);
    if (pixels == NULL) {
        fprintf(stderr, "realloc failed\n");
        exit(1);
    }

    // zero buffer
    for (i = 0; i < size; i++) pixels[i] = 0;

    // clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    reset();
}

void push(box4d *box1) {
    // increment stack pointer
    stackTop = (stackTop + 1 < STACK_SIZE) ? stackTop + 1 : 0;

    // save coordinates
    memcpy(&stack[stackTop], box1, sizeof (box4d));

    // overflow fifo
    if (stackBottom == stackTop) {
        stackBottom = (stackBottom + 1 < STACK_SIZE) ? stackBottom + 1 : 0;
    }
}

int pop(box4d *box1) {
    // stack empty
    if (stackTop == stackBottom) return -1;

    // restore coordinates
    memcpy(box1, &stack[stackTop], sizeof (box4d));

    // decrement stack pointer
    stackTop = (stackTop > 0) ? stackTop - 1 : STACK_SIZE - 1;

    return 0;
}

void window2pixel(box4i *box1, box4i *box2) {
    int tmp;

    // copy box
    memcpy(box2, box1, sizeof (box4i));

    // boundary check
    if (box2->x1 < 0) box2->x1 = 0;
    if (box2->x1 > width) box2->x1 = width;
    if (box2->y1 < 0) box2->y1 = 1;
    if (box2->y1 > height) box2->y1 = height;

    if (box2->x2 < 0) box2->x2 = 1;
    if (box2->x2 > width) box2->x2 = width;
    if (box2->y2 < 0) box2->y2 = 1;
    if (box2->y2 > height) box2->y2 = height;

    // convert window coordinates to pixel coordinates
    box2->y1 = height - box2->y1;
    box2->y2 = height - box2->y2;

    // swap if nessesary
    if (box2->x1 > box2->x2) {
        tmp = box2->x1;
        box2->x1 = box2->x2;
        box2->x2 = tmp;
    }

    if (box2->y1 > box2->y2) {
        tmp = box2->y1;
        box2->y1 = box2->y2;
        box2->y2 = tmp;
    }
}

void pixel2mandel(int px, int py, double *mx, double *my) {
    // convert pixel coordinates to mandelbrot coordinates
    *mx = ((GLdouble) px / width * (actual.x2 - actual.x1)) + actual.x1;
    *my = ((GLdouble) py / height * (actual.y2 - actual.y1)) + actual.y1;
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            free(pixels);
            exit(0);
            break;
        case 'R':
        case 'r':
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
    GLdouble x_center, y_center, x_half, y_half;

    if (button == GLUT_LEFT_BUTTON) {
        switch (state) {
            case GLUT_DOWN:
                // first corner
                box1.x1 = x;
                box1.y1 = y;

                // second corner
                box1.x2 = x;
                box1.y2 = y;

                drawBox = GL_TRUE;
                break;
            case GLUT_UP:
                // save
                push(&master);

                if (box1.x1 == box1.x2 || box1.y1 == box1.y2) {
                    window2pixel(&box1, &box2);
                    pixel2mandel(box2.x1, box2.y1, &x_center, &y_center);

                    x_half = (actual.x2 - actual.x1) / 2.0;
                    y_half = (actual.y2 - actual.y1) / 2.0;

                    // center around mouse click
                    master.x1 = x_center - x_half;
                    master.y1 = y_center - y_half;
                    master.x2 = x_center + x_half;
                    master.y2 = y_center + y_half;
                } else {
                    window2pixel(&box1, &box2);

                    // zoom in on box1
                    pixel2mandel(box2.x1, box2.y1, &master.x1, &master.y1);
                    pixel2mandel(box2.x2, box2.y2, &master.x2, &master.y2);
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
}

void motion(int x, int y) {
    // second corner
    box1.x2 = x;
    box1.y2 = y;

    // draw zoom box
    glutPostRedisplay();
}

void idle(void) {
    GLuint i = 0, imax = 0xfff, p;
    GLdouble x = 0.0, y = 0.0, mx, my, tmp;

    pixel2mandel(px, py, &mx, &my);

    // iterate mandelbrot function
    while ((x*x + y*y <= 2*2) && (i < imax)) {
        tmp = x*x - y*y + mx;
        y = 2 * x * y + my;
        x = tmp;
        i++;
    }

    // imax equals black
    if (i >= imax) i = 0;

    // pixel pointer
    p = (py * width + px) * 4 * sizeof (GLubyte);

    // pixel color
    pixels[p + 0] = (i & 0x00f) << 4; // red
    pixels[p + 1] = (i & 0x0f0);      // green
    pixels[p + 2] = (i & 0xf00) >> 4; // blue

    // next pixel
    if (++px >= width) {
        px = 0;

        // draw new line
        glRasterPos2i(0, py);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, py);
        glDrawPixels(width, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glutPostRedisplay();

        if (++py >= height) {
            py = 0;

            // stop calculations
            glutIdleFunc(NULL);
        }
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

