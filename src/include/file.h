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

int ReadLine(FILE *fp, char *cad);
int AnalizeLine(char *cad);
long SizeFile(char *);

int Trim(char *cad);
size_t ReadWordFromCad(char *word, char *cad);
int TakeWordFromCad(char *word, char *cad);

struct Block *CreateBlock();
int ReadNBlocks(char *fname, struct Block *br, long fpos);
int ReadRcFile(SDL_Surface *screen, char **, Uint32 *, char *);

#endif
