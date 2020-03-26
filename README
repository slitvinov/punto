
                               punto v 1.0

                      An X Window particle plotting tool.


     All files in this distribution Copyright (C) 1998-2009
     by Mariano Revenga
     mrevenga@fisfun.uned.es see disclaimer at end of this file.


What is punto?
==============

Punto is a tool to plotting particles written for X. It reads data
from a file and display them in a X window as spheres.  It can plot
also vectorial fields. The output can be in one, two or three
dimensions.

What is the Current Version of Punto?
=====================================

Punto 1.0.04 is the latest release, and is dated 12/06/2009

Is There An Punto Home Page?
============================

Yes, it's located at http://punto.sourceforge.net/

What Are Some of punto' Features?
=================================

type punto without options for a brief description.

o   One, two or three dimensions.
o   Zooming, rotation 
o   Gradients of colors depending of the values of a column.
o   You can indicated the columns to plot.
o   The form of the output can be spheres, circles or squares.
o   Periodic images can be ploted.
o   Trace the trayectories of particles.


Format of the files that punto can read.
=======================================

The files must be written in ascii and must have one column as minimum
for x coordinate. Every column must be separated by an space or a tab
character. Additional columns can indicate to punto the y and/or z
coordinates, a color, a size or the values of a vector
field. Additional columns are discarded.

The file can be divided in blocks. A block is a set of data lines,
each line containing the coordinates of every point. The blocks must
be separated at least by a return, blank or a comment line. A comment
line must begin by the character #.

Punto continues reading the file until the end of the file is reached
or a data-block is completed. Then the data are plot in a X window as
particles (spheres as default). If several blocks exits an animation
can be displayed.


Compiling and Installing Punto
==============================

Please see the file 'INSTALL'

Examples (tutorial)
===================

A serie of data files are generated with the gen tool. Type make gen
and execute the file ./bin/gen. In the directory ./dat several data
files are generated.

type ./TUTORIAL or

Try with these files:

$punto dat/aleatori03d.dat

The file aleatorio3d.dat has five columns. With no options punto reads the
first three and assign them to the x, y and z coordinates.

In the upper left corner three numbers are displayed, the zoom of the
image, the number of particles(Num) and the frame number(Block). In
this file there is only one frame (number 1).


$punto -c -r dat/aleatori03d.dat

Two columns are added: color and size.

A border box is displayed on and off typing 'b'.

Try zooming in and out typing 'z' and 'Z'. 

With the option -r you can change the size typing  'i' and 'I'. 


$punto -r dat/red3d.dat 

The file red3d.dat has four columns, the 3-D coordinates and the size.
Without options punto plot a 3D system with the default size.


$punto dat/deathstar.dat

A 3D sphere formed by particles its plotted. Typing the key 'c' you
can cut by planes parallels to the screen point of view and look in the
inside (Not implemented yet). A fourth column is for the color.

$punto dat/onda.dat 

A 3D wave is plotted. You can start-stop an animation typing
's'. Reverse it typing 'r'. 

You can change the default size of the particles with the option -s
 
$punto -s 4 dat/onda.dat 


The file dat/fuente.dat is a source of colored particles with diferent
sizes flowing from a point.

$punto -c -r dat/fuente.dat

$punto -c -r -s 30 dat/fuente.dat

Type 's' for start and stop the animation. 'space' for one by one
slide. + (plus) and - (minus) for change the delay between frames
(this delay is initially set to zero). The . (dot) sets the delay
to zero.

With the border activated (command option -B or keyboard 'b') you can
go to any point of the file clicking with the mouse in the file status
bar.
 
'4-9' for rotating the axis. '0' for return for the initial point of
view.


Check the performance of your computer with,

$punto -c -r dat/infinito.dat

100000 particles are displayed. Maybe you want to display less particles,

$punto -c -r -n 1500:2000 dat/infinito.dat

plot only 500 particles, between the positions 1500 and 2000.

A minor size also aid,

$punto -c -r -n 1500:2000 -s 4 dat/infinito.dat


You can change the type of the particles to be plotted with the option -t
-t 0 (circles), -t 1 (squares(), -t 2 (spheres, the default) 


colors
------

The values of the color column must be an positive integer. There are
64 colors avalaible. You can see them with,

$punto -c -s 6 dat/colors.dat

colors.dat is a three columns file. Without the -c option punto
interprets it as a three dimensional data file.

If your color column has no integers data or you want to display an
scalar field its possible to plot it using the option -w, in this
case the values are rescaled between the minimum and maximum of the
data values.

$punto -c -s 6 -w dat/colors.dat

But its also possible to plot a gradient of colors from red to blue
with the option -G

$punto -s 6 -G dat/colors.dat

Change the order of the columns with the -z option,

$punto -s 6 -G -z 1:2:1 dat/colors.dat

This last command is interpreted as: two dimensional file, column 1
for x, column 2 for y coordinate and column 1 for the color.


Its possible to change the palette of colors. There are 5 different
palettes. Change it with the -p option:

    -p num where num is the gradient palette.  0 spectrum (default), 1
         red, 2 green, 3 blue, 4 grey.


plotting vectorial fields
-------------------------

The file field2d.dat has four columns. With the -V option this columns
are interpreted as x and y coordinates and the x and y values of a
vectorial field.

$punto -V dat/field2d.dat 

Change the size of the field,

$punto -V -s 20 dat/field2d.dat 


The file field3d.dat has six columns and is interpreted as 3D vectorial field.

$punto -V dat/field3d.dat 

If the number of columns of your data file is different you always
can choose the columns to be plotted with the option -z.


saving frames, animation
------------------------

During visualitation you can save the current image pressing the "F"
key. A file in xpm format is created starting with "#temp-000.xpm" in
the current directory.

Pressing "S" you can change between modes save on and off. In save
mode on all the images are saved in xpm format. Finally you can
convert all these files in an animation with the convert tool from
imagick:

$convert -delay 5 #temp-* temp.gif

this command converts all the files in an animated gif with 5/100
seconds between frames.


Miscellaneous Notes
===================

That's about all for now.  Naturally, I've got a lot of big plans
for Punto, including ...(see TODO file)

If you enjoy punto, make any modifications to it, if have problems with
it, or just want to talk then feel free to email me at
mrevenga at fisfun.uned.es

Thanks, and enjoy!!

Mariano Revenga.

Disclaimer
==========


/*****************************************************************************
**  This is part of the punto program
**  Copyright (C) 1998-2009 Mariano Revenga
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License (version 2) as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

