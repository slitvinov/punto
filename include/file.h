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
#ifndef _FILE_H
#define _FILE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void GetFileName(int, char **, char *);
int NumRet(FILE *);
long CountLines(char *);
int NColumns(char *);
int FindChain(char *target, char *cad[], int n);

int ReadLine(FILE * fp, char *cad);
int AnalizeLine(char *cad);
long SizeFile(char *);

int Trim(char *cad);
size_t ReadWordFromCad(char *word, char *cad);
int TakeWordFromCad(char *word, char *cad);

struct Block *CreateBlock();
int ReadNBlocks(char *fname, struct Block *br, long fpos);
int ReadRcFile(SDL_Surface * screen, char **, Uint32 *, char *);

#endif
