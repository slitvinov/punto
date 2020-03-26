#include <string.h>
#include <stdlib.h>
#include "buffer.h"

long
RChar(char *cad, FILE * fp)
{
    /* 
       Read a char from file fp
     */
    long rbytes;

    rbytes = _RBuffer(cad, 1, CHAR, fp);
    return (rbytes);
}


long
RWord(char *cad, int len, FILE * fp)
{
    /* 
       Read a char from file fp
     */
    long rbytes;

    rbytes = _RBuffer(cad, len, WORD, fp);
    return (rbytes);
}

long
RLine(char *cad, int len, FILE * fp)
{
    /* 
       Read a line from file fp
     */
    long rbytes;

    rbytes = _RBuffer(cad, len, LINE, fp);
    return (rbytes);
}

long
_RBuffer(char *cad, int len, int type, FILE * fp)
{
    /* 
       Keep a buffer for a file 
       copy in cad the next char, word or line from the buffer.
       returns:
       the number of readed bytes from the file.
       if EOF is reached returns EOF.
     */
    static int init = 0;
    static struct _Buffer buffer;

    size_t n;
    int inword;
    size_t iword, lword;        /* position of the first letter and word length */
    int eow;
    int eof;
    size_t nbytes, ntbytes;
    size_t nbytesc;
    char c;

    strcpy(cad, "\0");          /* delete the chain */

    if (len + 1 > BUFFER_SIZE) {
        printf("ERROR: word size too long.\n");
        printf("\t word size: %d max. size: %d\n", len, BUFFER_SIZE);
        exit(EXIT_FAILURE);
    }

    if (init == 0) {            /* initialize the buffer  */

        buffer.i = 0;
        buffer.f = 0;
        buffer.eof = 0;
        buffer.fp = fp;
        buffer.fpos = ftell(fp);
        n = _FillBuffer(&buffer);

        if (n == 0 && buffer.eof) { /* EOF */
            return (EOF);
        }
        init++;
    }

    /* check if the buffer is valid */
    if (buffer.fp != fp || ftell(fp) != buffer.fpos) {  /* buffer not valid */
        buffer.i = 0;
        buffer.f = 0;
        buffer.fp = fp;
        buffer.fpos = ftell(fp);
        n = _FillBuffer(&buffer);
        if (n == 0 && buffer.eof) { /* EOF */
            return (EOF);
        }
    }

    /* al least one element */
    if (buffer.f == buffer.i) {

        n = _FillBuffer(&buffer);
        if (n == 0 && buffer.eof) { /* EOF *//* may be an error, HERE */
            return (EOF);
        }
        if (n == 0 && !buffer.eof) {
            fprintf(stderr, "ERROR 1 in ReadBuffer()\n");
            exit(EXIT_FAILURE);
        }
    }

    nbytes = 0;
    ntbytes = 0;
    nbytesc = 0;

    switch (type) {
    case CHAR:
        if (buffer.f - buffer.i > 0) {
            memcpy(cad, &buffer.buf[buffer.i], 1);
            buffer.i++;
            nbytes++;
            ntbytes++;
        } else {
            fprintf(stderr, "this line musnt be reached. ReadBuffer()\n");
            exit(EXIT_FAILURE);
        }
        break;

    case WORD:
        inword = 0;
        iword = lword = 0;
        eow = 0;
        eof = 0;
        do {
            c = buffer.buf[buffer.i + lword + iword];   /* read a char */

            switch (c) {
            case ' ':
            case '\t':
                if (inword == 0) {
                    iword++;
                }
                if (inword == 1) {
                    eow = 1;
                }

                break;
            case '\n':
                eow = 1;
                break;
            default:
                inword = 1;
                lword++;

                break;
            }
            nbytes++;
            ntbytes++;

            /* if buffer is empty refill */
            if (buffer.i + nbytes == buffer.f) {
                n = _FillBuffer(&buffer);
                if (n == 0 && buffer.eof) { /* EOF */
                    eow = 1;
                    eof = 1;
                }
            }
        }
        while (eow == 0);

        if (lword > 0) {        //buffer overrun
            if (lword < (size_t) len) {
                memcpy(cad, &buffer.buf[buffer.i + iword], lword);
            } else {
                fprintf(stderr,
                        "Warning. Line too long, truncated to %d bytes\n",
                        len);
                memcpy(cad, &buffer.buf[buffer.i + iword],
                       (size_t) len - 1);
                lword = (size_t) len - 1;
            }
        }
        buffer.i += nbytes;

        /* end of string */
        if (lword < (size_t) len - 1) {
            strcpy(&cad[lword], "\0");
        } else {
            fprintf(stderr, "this line musnt be reached. ReadBuffer()\n");
            fprintf(stderr, "Line too long. ReadBuffer()\n");
            exit(EXIT_FAILURE);

        }
        break;

    case LINE:
        //    printf("reading a line\n");

        lword = 0;
        eow = 0;
        eof = 0;
        do {
            c = buffer.buf[buffer.i + lword];   /* read a char */

            switch (c) {
            case '\n':
                eow = 1;
                break;
            default:
                lword++;
                break;
            }
            nbytes++;
            ntbytes++;
            /* if buffer is empty refill */
            if (buffer.i + nbytes == buffer.f) {
                if (lword > 0) {
                    // buffer overrun
                    if (nbytesc + lword < (size_t) len) {
                        memcpy(&cad[nbytesc], &buffer.buf[buffer.i],
                               lword);
                    } else {
                        fprintf(stderr,
                                "Warning. Line too long, truncated to %d bytes\n",
                                len);
                        memcpy(&cad[nbytesc], &buffer.buf[buffer.i],
                               len - nbytesc - 1);
                        lword = len - nbytesc - 1;
                    }

                    buffer.i += nbytes;
                    nbytesc += lword;
                    buffer.i = buffer.f = 0;
                    lword = 0;
                    nbytes = 0;
                }
                n = _FillBuffer(&buffer);
                if (n == 0 && buffer.eof) { /* EOF */
                    eow = 1;
                    eof = 1;
                }
            }
        }
        while (eow == 0);

        if (lword > 0) {
            // buffer overrun
            if (nbytesc + lword < (size_t) len) {
                memcpy(&cad[nbytesc], &buffer.buf[buffer.i], lword);
            } else {
                fprintf(stderr,
                        "Warning. Line too long, truncated to %d bytes\n",
                        len);
                memcpy(&cad[nbytesc], &buffer.buf[buffer.i],
                       len - nbytesc - 1);
                lword = len - nbytesc - 1;

            }
            nbytesc += lword;
        }
        buffer.i += nbytes;

        /* end of string */
        if (lword < (size_t) len - 1) {
            strcpy(&cad[nbytesc], "\0");
        } else {
            fprintf(stderr, "this line musnt be reached. ReadBuffer()\n");
            fprintf(stderr, "Line too long. ReadBuffer()\n");
            exit(EXIT_FAILURE);

        }
        break;                  //LINE
    default:
        break;
    }

    /* return values */
    if (nbytes == 0) {
        if (buffer.eof && buffer.i == buffer.f && nbytesc == 0)
            return (EOF);
    }
    return (ntbytes);
}

size_t
_FillBuffer(struct _Buffer *buffer)
{
    /* 
       fill the buffer with file data
     */
    size_t nbytes;              /* bytes to read */
    size_t n;

    if (feof(buffer->fp) != 0) {
        buffer->eof = 1;
        return (0);
    }
    if (buffer->i != 0) {       /* moving data */
        memmove(&buffer->buf[0], &buffer->buf[buffer->i],
                buffer->f - buffer->i);
        buffer->f = buffer->f - buffer->i;
        buffer->i = 0;
    }

    nbytes = BUFFER_SIZE - buffer->f;

    n = fread(&buffer->buf[buffer->f], 1, nbytes, buffer->fp);

    if (feof(buffer->fp) != 0)
        buffer->eof = 1;

    if (n > 0) {
        buffer->f += n;
        buffer->fpos = ftell(buffer->fp);
    }
    return (n);
}
