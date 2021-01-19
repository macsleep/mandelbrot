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

#ifndef _MANDELBROT_H_
#define _MANDELBROT_H_

/* Includes */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>
// #include <GL/glut.h>

/* Macros */

#define MX_MIN -2.2
#define MX_MAX 0.8
#define MY_MIN -1.25
#define MY_MAX 1.25

/* Global Variables */

static GLuint px = 0, py = 0;
static GLubyte *pixels;
static GLdouble mx_min, mx_max, my_min, my_max;
static GLdouble mx_min_orig = MX_MIN, mx_max_orig = MX_MAX, my_min_orig = MY_MIN, my_max_orig = MY_MAX;
static GLboolean drawBox = GL_FALSE;
static GLuint box[] = {0, 0, 0, 0};

/* Function Prototypes */

void init(void);
void display(void);
void reset(void);
void reshape(int w, int h);
void pixel2mandel(int px, int py, double *mx, double *my);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle(void);

#endif

