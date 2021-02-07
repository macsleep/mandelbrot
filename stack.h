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

#ifndef _STACK_H_
#define _STACK_H_

/* Includes */

#include <stdio.h>
#include <stdlib.h>

/* Macros */

#define STACK_SIZE 8

/* Type Definitions */

typedef struct {
    double x1, y1, x2, y2;
} box4d;

/* Function Prototypes */

void stackReset(void);
void stackPush(box4d *box);
void stackPop(box4d *box);

#endif

