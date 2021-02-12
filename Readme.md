# Mandelbrot

## Description

This is an old, classic, simple [Mandelbrot](https://en.wikipedia.org/wiki/Mandelbrot_set) Fractal generator using [OpenGL](https://www.opengl.org/) and GLUT. I wrote this program to learn how to modify pixels using OpenGL.

![Mandelbrot](images/Mandelbrot.jpeg)

## Build

Clone or download the software. Make sure the required OpenGL and GLUT headers/libraries are installed. Edit the Makefile to enable the OS specific variables of your platform and then type `make` to build. The build is known to work on MacOS (10.12), Ubuntu (18.04.5) and HP-UX (11.11).

## Usage

You can drag the left mouse button to zoom in on details. The zoom box is also drawn using OpenGL. A single left mouse button click centers the Mandelbrot around the mouse click. The right mouse button can undo  up to eight of the last zooms/centers. Pressing 'R' resets the generator to its default coordinates (in case they are no longer in the undo history). And the Escape key exits the application in classic GLUT style.

Please note: Once the pixels start getting bigger you have reached the resolution limit of the double precision floating point numbers. There's nothing much you can do about that right now except zoom out.

## Screen Shots

<table>
  <tr>
    <td><img src="images/Mandelbrot2.jpeg" width="320" /></td>
    <td><img src="images/Mandelbrot3.jpeg" width="320" /></td>
  </tr> 
  <tr>
    <td><img src="images/Mandelbrot4.jpeg" width="320" /></td>
    <td><img src="images/Mandelbrot5.jpeg" width="320" /></td>
  </tr> 
  <tr>
    <td><img src="images/Mandelbrot6.jpeg" width="320" /></td>
    <td><img src="images/Mandelbrot7.jpeg" width="320" /></td>
  </tr>
  <tr>
    <td><img src="images/Mandelbrot8.jpeg" width="320" /></td>
    <td><img src="images/Mandelbrot9.jpeg" width="320" /></td>
  </tr>
</table>

Date: 2021-01-19
