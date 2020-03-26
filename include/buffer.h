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
