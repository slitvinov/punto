#include "help.h"


void Usage(char *ver,char *l_rev){
     
  /*
   *      Usage() Little help. Show the command line parameters.
   */
  
    
  (void) fprintf( stdout, 
		  "Usage is: punto [-options] file \n" );
  (void) fprintf( stdout, 
		  "possible options: h [s,size] n [g,geom,geometry] d [r,radio] [c,color] w G p B D t z V bg fg lc F T\n" );
  (void) fprintf( stdout, 
		  "Where file is an (x,[r,c]), (x,y,[r,c,fx,fy]) or a (x,y,z,[r,c,fx,fy,fz]) file.\n" );
  (void) fprintf( stdout, 
		  "\tr is the radio of the puntos.\n" );
  (void) fprintf( stdout, 
		  "\tc is the color of the puntos.\n" );
  (void) fprintf( stdout, 
		  "\tfx fy fz are the field values.\n" );
  (void) fprintf( stdout, 
		  "Additional columns are discarded. You can choose the columns to plot with the -z option.\n" );
  (void) fprintf( stdout, 
		  "Each frame must be separated for a blank or a comment line.\n" );
  (void) fprintf( stdout, 
		  "The comment lines must begin with a #.\n" );
  (void) fprintf( stdout, 
		  "\nOptions:\n-h \t this help.\n" );
  (void) fprintf( stdout, 
		  "-s N\t where N is max. size in pixels of puntos (an integer number) (default: 15).\n" );
  (void) fprintf( stdout, 
		  "-n N1:N2\twhere N1 and N2 are integers starting from 0.\n\t\tOnly the puntos with labels between these numbers are plotted.\n" );
  (void) fprintf( stdout, 
		  "-g XRESOxYRESO\twidth and height of the window.\n" );
  (void) fprintf( stdout, 
		  "-d cs\twhere cs is the delay time between frames in centiseconds.\n" );
  (void) fprintf( stdout, 
		  "-r [min:max]\tthere is a column of radius.\n" );
  (void) fprintf( stdout, 
		  "\t rescale between min and max values.\n");
  (void) fprintf( stdout, 
		  "-c \tthere is a column of colors.\n" );
  (void) fprintf( stdout, 
		  "-w Obsolote. as -G.\n" );
  //"-w [min:max]\tas -c but the colors are reescaled between min and max values, \n" );
  //  (void) fprintf( stdout, 
  //		  "\tuseful if the color column has negative values.\n" );
  (void) fprintf( stdout, 
		  "-G [min:max] \tThe colors are a gradient scale. \n" );
  (void) fprintf( stdout, 
		  "-p num \twhere num is the gradient palette. \n" );
  (void) fprintf( stdout, 
		  " \t 0 spectrum (default), 1 red, 2 green, 3 blue, 4 grey.\n");
  (void) fprintf( stdout, 
		  "-B [x0:y0:z0:x1:y1:z1]\tplot a border box.\n" ); 
  (void) fprintf( stdout, 
		  "-D N\tthe dimension of the data is set to N.\n" ); 
  (void) fprintf( stdout, 
		  "-t 0,1,2\ttype of the bitmap displayed.\n" ); 
  (void) fprintf( stdout, 
		  " \t 0 display circles, 1 squares, 2 spheres (default).\n");
  (void) fprintf( stdout, 
		  "-z a:b:c:d:e.\tyou can indicate the columns to be plotted.\n");
  (void) fprintf( stdout, 
		  " \t the x,y,z columns then the radio,color and field ones.\n");
  (void) fprintf( stdout, 
		  "-trace [n]\ttrace mode on. n: number of trace dots.\n");
  (void) fprintf( stdout, 
		  "-V \tA vectorial field is plotted with arrows.\n");
  (void) fprintf( stdout, 
		  "-bg color\tset the background color. (Default black).\n" ); 
  (void) fprintf( stdout, 
		  "-fg color\tset the foreground color. (Default blue).\n" ); 
  (void) fprintf( stdout, 
		  "-lc color\tset the led and box color. (Default green).\n" ); 
  (void) fprintf( stdout,
		  "-F \"fname\" \tset the default root filename for the saved pixmaps.\n \t\t(default #temp_).\n");
  (void) fprintf( stdout, 
		  "-T \"name\" \tset the title of the window, (default: the name of the data file).\n");

 (void) fprintf( stdout, 
		  "\nKeyboard controls:\n0\treset positions.\n4,7 \trotate the X-axis.\n5,8 \trotate the Y-axis.\n6,9 \trotate the Z-axis.\n" );
  
  (void) fprintf( stdout, 
		  "z,Z\tzoom-in zoom-out.\n");
  (void) fprintf( stdout, 
		  "i,I\tincrease-decrease the size of puntos. (option -r must be activated.)\n");
  (void) fprintf( stdout, 
		  "left,right arrow\tmove left-right.\n");
  (void) fprintf( stdout, 
		  "up,down arrow\t\tmove up-down.\n");
  (void) fprintf( stdout, 
		  "Keeping pressed the SHIFT or the < key the movement is faster or slower.\n");
  (void) fprintf( stdout, 
		  "r\treverse animation.\n");
  (void) fprintf( stdout, 
		  "f\tfast mode. Don't show graphics.\n");
  (void) fprintf( stdout, 
		  "a\tin vector mode changes among points, lines and arrows.\n");
  (void) fprintf( stdout, 
		  "u\tfixed radio on off.\n");
  (void) fprintf( stdout, 
		  "\t\tin vector mode normalized to unit.\n");
  (void) fprintf( stdout, 
		  "t\ttrace mode on off.\n");
  (void) fprintf( stdout, 
		  "p\tshow periodic images.\n");
  (void) fprintf( stdout, 
		  "S\tSave mode on off. All the images are saved.\n");
  (void) fprintf( stdout, 
		  "F\tSave only the current image.\n");

  (void) fprintf( stdout, 
		  "+(plus)\t\tincrease delay between frames.\n");
  (void) fprintf( stdout, 
		  "-(minus)\tdecrease delay between frames.\n");
  (void) fprintf( stdout, 
		  ".(period)\tsets delay time to the initial value.\n");
  //  (void) fprintf( stdout, 
  //		  "left mouse bottom   stops,starts animation.\n");
  (void) fprintf( stdout, 
		  "s   \tstops, starts animation.\n");
  (void) fprintf( stdout, 
		  "space   step by step animation.\n");
  (void) fprintf( stdout, 
		  "b\tborder box on off.\n");
  (void) fprintf( stdout, 
		  "c\tcomment line on-off.\n");
  (void) fprintf( stdout, 
		  "q   \tquit punto.\n");
  (void) fprintf( stdout, 
		  "To see all available colors, type punto -h color.\n");

  (void) fprintf( stdout, 
		  "\nCopyleft mrevenga 1998-2009 ");
  //		  "\nCopyleft XaY 1998 ");
  (void) fprintf( stdout, 
		  ver);
  (void) fprintf( stdout, 
		  "\nlast revision %s\n",l_rev);
  (void) fprintf( stdout, "Please, send bugs and suggestions to: mrevenga at users.sourceforge.net\n");
  
}       /* -- funcion Usage */
