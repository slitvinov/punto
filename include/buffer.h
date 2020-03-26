/*****************************************************************************
 **  This is part of the punto program
 **  Copyleft (C) 1998-2009  Mariano Revenga
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
/*
  buffer.c
  file reading using a buffer.
  version 0.02 Jun 2009
  Author: mrevenga.
*/

#include <stdio.h>

#define BUFFER_SIZE 8192

#define CHAR 1
#define WORD 2
#define LINE 3



struct _Buffer {
    size_t i, f;                /* next to read and next to write */
    FILE *fp;                   /* last fiel in use */
    long fpos;                  /* position in the file */
    int eof;                    /* if true EOF reached */
    char buf[BUFFER_SIZE];      /* buffer max. size */
};


long RChar(char *cad, FILE * fp);
long RWord(char *cad, int, FILE * fpq);
long RLine(char *cad, int, FILE * fp);
long _RBuffer(char *cad, int, int type, FILE * fp);
size_t _FillBuffer(struct _Buffer *buffer);
