# Mandelbrot

## Description

This is an old, classic, simple [Mandelbrot](https://en.wikipedia.org/wiki/Mandelbrot_set) Fractal generator using OpenGL and GLUT. I wrote this program to learn how to modify pixels using OpenGL.

![Mandelbrot](images/Mandelbrot.jpeg)

## Build

Clone or download the software. Make sure the required OpenGL and GLUT libraries are installed. Edit the Makefile to enable the OS specific variables of your platform and then type `make` to build. The build is known to work on MacOS (10.12) and Ubuntu (18.04.5).

## Usage

You can drag the left mouse button to zoom in on details. The zoom box is also drawn using OpenGL. A single left mouse button click centers the Mandelbrot around the mouse click. The right mouse button resets the generator to its default coordinates. And the Escape key exits the application in classic GLUT style.

Date: 2021-01-19
