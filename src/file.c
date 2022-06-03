#include <stdio.h>
#include <string.h>
#include "include/punto.h"
#include "include/file.h"
#include "include/system.h"
#include "include/buffer.h"

void GetFileName(int argc, char *argv[], char *fname) {
  /* Gets the name of the file data.
   *  Must be the last argument at the line comand arguments
   */

  if (strlen(argv[argc - 1]) >= MAX_WORD_LEN) {
    fprintf(stderr, "File name too long. Max. length: %d characters\n",
            MAX_WORD_LEN);
    exit(EXIT_FAILURE);
  }

  strncpy(fname, argv[argc - 1], MAX_WORD_LEN);
  strncpy(&fname[MAX_WORD_LEN - 1], "\0", 1);

} /* --funtion GetFileName  */

int NumRet(FILE *fp) {
  /* end of block ?
     returns the number of ret's between two data lines.
     ignore all the lines that begins with # and % or a letter.
     point the file to the next valid data.
     returns:
     if the file reachs EOF returns -1 else
     the number of \n
   */

  int ret = 0;
  char s;
  long file_pos;
  int eof = FALSE;
  size_t br;

  file_pos = ftell(fp);

  for (;;) {
    br = fread(&s, sizeof(char), 1, fp);
    if (br < 1) {
      if (feof(fp) != 0) {
        eof = TRUE;
        break;
      } else {
        fprintf(stderr, "ERROR in fread()\n");
        exit(EXIT_FAILURE);
      }
    } else {
      //    if(fscanf(fp,"%c",&s) == EOF){eof=TRUE; break;}//return(-1);
      if (s == '\n') {
        ret++;
        file_pos = ftell(fp);
      } else if ((s != ' ') && (s != '\t') && (s != '#') && (s != '%') &&
                 (s < 'a' || s > 'z') && (s < 'A' || s > 'Z')) {
        file_pos = ftell(fp) - 1;
        break;
      }
      /* if a comment line exits goto the next one */
      if (s == '#' || s == '%' || (s > 'a' && s < 'z') ||
          (s > 'A' && s < 'Z')) {
        for (;;) {
          if (fread(&s, sizeof(char), 1, fp) == 0) {
            if (feof(fp) != 0) {
              eof = TRUE;
              break;
            } else {
              perror("fread");
              exit(EXIT_FAILURE);
            }
          }
          //    if(fscanf(fp,"%c",&s) == EOF){eof=TRUE;break;}
          if (s == '\n') {
            ret++;
            file_pos = ftell(fp); /* new position of the file */
            break;
          }
        }
      }
    }
  } /* for */

  fseek(fp, file_pos, SEEK_SET);
  if (eof)
    return (-1);
  return (ret);
} /* --function NumRet */

long CountLines(char *fname) {
  /*
     v 0.2
     returns the number of data lines of the first data block.
     doesnt count comment or empty lines.
     return the number of puntos (particles).
   */
  FILE *fp;
  char s;
  int nlines, nchar, comment;
  int endblock;

  if ((fp = fopen(fname, "rt")) == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  /* go to the first data line */
  (void)NumRet(fp);
  // retorno=NumRet(fp;

  nlines = 0;
  endblock = 0;
  nchar = 0;
  comment = 0;
  do {
    if (fscanf(fp, "%c", &s) != EOF) {
      switch (s) {
      case '\n': /* end of line */
        if (nchar == 0)
          endblock++; /* end of block? */
        else {
          nlines++;
        }
        comment = 0;
        nchar = 0;
        break;
      case '#': /* comment line */
        comment++;
        nchar = 0;
        break;
      case ' ':
      case '\t':
        break;
      default:
        if (comment == 0)
          nchar++;
        break;
      }
    } else {
      endblock++;
    }

  } while (endblock == 0);

  fclose(fp);

  return (nlines);
}

int NColumns(char *fname)
/* function NColums
   returns:
   the number of rows of the data file.
 */
{
  FILE *fp;
  struct stat info;
  int fd;
  char s;
  int c, dentropal;
  int np;
  int cont = 0;
  int sw = 0;

  /* exit if the file doesn't exist */

  if ((fd = open(fname, O_RDONLY)) == -1) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  /* wait until the file size is nonzero    */
  do {
    info.st_size = 0;
    if (fstat(fd, &info) == -1) {
      perror(fname);
      exit(EXIT_FAILURE);
    }
    if ((int)info.st_size != 0)
      sw++;
    if (cont == 0 && sw == 0) {
      printf("file: %s has zero size. Waiting ...\n", fname);
      cont++;
    }
    usleep(100000);
  } while (sw == 0);
  sw = 0;
  close(fd);

  np = 0;
  dentropal = FALSE;
  //  printf("data file : (%s)\n",fname);
  if ((fp = fopen(fname, "rt")) == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }

  /* goto the first data line */
  NumRet(fp);
  do {
    c = fscanf(fp, "%c", &s);
    if (c == EOF)
      sw++;
    if (s == '\n')
      sw++;
    if (s == ' ' || s == '\t')
      dentropal = FALSE;
    else if (dentropal == FALSE) {
      dentropal = TRUE;
      ++np;
    }
  } while (sw == 0);
  fclose(fp);
  return (np);
}

int FindChain(char *target, char *cad[], int n) {
  /* find the word target in the string array cad
     return:
     the position of tarjet in cad
     -1 if is not found.
   */
  int i;

  for (i = 0; i < n; i++) {
    if (strncmp(target, cad[i], MAX_WORD_LEN) == 0) {
      return (i);
    }
  }
  return (-1);
}

int AnalizeLine(char *cad) {
  /* return the type of the line:
     0 empty, return
     -1, end-of-file
     1 comment line
     2 dataline
   */
  int i;
  size_t len;
  int status;

  len = strlen(cad);
  /* look for the first valid character */
  status = 0;
  for (i = 0; i < (int)len && i < MAX_LINE_LEN; i++) {
    /* look for a letter */
    if (strncmp(&cad[i], "a", 1) >= 0 && strncmp(&cad[i], "z", 1) <= 0)
      return (COMMENT);
    if (strncmp(&cad[i], "A", 1) >= 0 && strncmp(&cad[i], "Z", 1) <= 0)
      return (COMMENT);
    /* --look for a letter */
    if (status == 0) {
      switch (cad[i]) {
      case ' ':
      case '\t':
        break;
      case '\n':
        status = RETURN;
        break;
      case '#':
      case '%':
        status = COMMENT;
        break;
      default:
        status = DATA;
        break;
      }
    }
    if (status)
      break;
  }
  return (status);
}

long SizeFile(char *filename) {
  /*
     the file changes ?
     return the size of the file filename
  */
  int fd;
  struct stat info;

  if ((fd = open(filename, O_RDONLY)) == -1) {
    perror(filename);
    exit(EXIT_FAILURE);
  }
  info.st_size = 0;
  if (fstat(fd, &info) == -1) {
    perror(filename);
    exit(EXIT_FAILURE);
  }
  close(fd);
  return (info.st_size);
}

int ReadNBlocks(char *fname, struct Block *br, long fpos) {
  /*
     version 0.1
     Read the file fname completely
     looking for blocks of data
     Create a linked list of struct Block starting in *br.
     return:
     the number of blocks of the file.
   */

  FILE *fp;

  //  struct Buffer buf;
  char line[MAX_LINE_LEN];
  int type;
  int i;
  int sw;

  //  int eoffile;
  long file_pos;
  long file_pos0;
  long file_size;
  long incfile_size;
  struct Block *lastb;
  long cont;
  long fcont = 0;
  int inblock = 0;
  int incomment = 0;
  int indata = 0;
  int infile = 0;
  int numblock = 0;
  long nrbytes = 0;

  //  long ntbytes=0;

  if ((fp = fopen(fname, "rt")) == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }

  if (fseek(fp, fpos, SEEK_SET) == -1)
    printf("ERROR Seeking file\n");

  br->pos = ftell(fp);
  file_size = SizeFile(fname);
  incfile_size = (file_size - fpos) / 20;
  file_pos = file_pos0 = fpos;
  lastb = br;

  cont = 0;
  incomment = 0;
  indata = 0;
  nrbytes = 0;

  do {
    file_pos += nrbytes; // HERE ftell(fp);//-incpos;

    strncpy(line, "", (size_t)1);
    nrbytes = RLine(line, MAX_LINE_LEN, fp);
    //    ntbytes+=nrbytes;
    //    incpos=nrbytes;

    type = AnalizeLine(line);

    if ((file_pos - nrbytes - file_pos0 > incfile_size)) {
      fflush(stdout);
      file_pos0 = file_pos - nrbytes;
      fcont++;
    }
    if (nrbytes < 0) { /* fin de archivo */
      infile = -1;
    }

    if (type == DATA) {
      indata = 1;
      cont++;
    }
    if (type == COMMENT)
      incomment = 1;

    if (inblock == 0) {
      if (type == COMMENT || type == DATA) {
        /* comienza un bloque */
        inblock = 1;
        if (infile == 0) { /* first block */
          infile = 1;
          lastb->pos = file_pos;
        } else { /* siguientes bloques */
          numblock++;
          lastb->next = CreateBlock();
          lastb->next->prev = lastb;
          lastb = lastb->next;
          lastb->next = 0;
          lastb->pos = file_pos;
          lastb->num = 0;
          lastb->len = 0;
          incomment = 0;
        }
      }
    }
    if (inblock) {
      sw = 0;
      if (nrbytes < 0)
        sw = 1;
      if (indata && type != DATA)
        sw = 1;
      if (sw) {
        /* fin de bloque */
        inblock = 0;
        indata = 0;
        incomment = 0;
        lastb->num = cont;
        cont = 0;
      }
    }
  } while (infile > 0);
  fclose(fp);
  return (numblock);
}

struct Block *CreateBlock() {
  /* Create a new Block

   */
  struct Block *bloque;

  if ((bloque = (struct Block *)malloc(sizeof(struct Block))) == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  bloque->read = 0;
  bloque->pos = 0;
  bloque->len = 0;
  bloque->num = 0;
  bloque->comment = NULL;
  bloque->next = NULL;
  bloque->prev = NULL;

  return (bloque);
}

int TakeWordFromCad(char *word, char *cad) {
  /* copy the first word of cad copying it in word,
     word is removed from cad
     returns:
     the lenght of the new cad or
     -1 if the original cad had zero lenght
   */

  char tmp[MAX_LINE_LEN];

  strcpy(tmp, "");
  if (ReadWordFromCad(word, cad) == 0)
    return (-1);
  strcpy(tmp, &cad[strlen(word)]);
  strcpy(cad, tmp);
  return ((int)strlen(cad));
}

size_t ReadWordFromCad(char *word, char *cad) {
  /* Read the first word of the chain cad and put it in word
     Don't change cad
     return values:
     if cad is an empty chain returns 0 else
     return the lenght of word
   */
  size_t i = 0;
  size_t len = 0;
  int status = 0;

  len = strlen(cad);
  //  if(len==0)return(0);

  //  printf("WW: %s\n",cad);
  for (i = 0; i < len && i < MAX_LINE_LEN && status == 0; i++) {

    switch (cad[i]) {
    case ' ':
    case '\t':
    case '#':
    case '%':
    case '\n':
      status = 1;
      break;
    default:
      break;
    }
  }
  strncpy(word, &cad[0], i);
  strncpy(&word[i], "\0", (size_t)1);
  return (i);
}

int Trim(char *cad) {
  /* Remove the blank characteres an the beginning and
     at the end of the chain cad
   */

  size_t i;
  int cl, cr;
  int swcl, swcr;
  size_t len;
  char word[MAX_LINE_LEN];

  cl = cr = 0;
  swcl = swcr = 0;
  len = strlen(cad);

  if (len) {
    for (i = 0; i < len && i < MAX_LINE_LEN; i++) {
      if (swcl == 0) {
        if (cad[i] == ' ' || cad[i] == '\t')
          cl++;
        else
          swcl = 1;
      }
      if (swcr == 0) {
        if (cad[len - i - 1] == ' ' || cad[len - i - 1] == '\t')
          cr++;
        else
          swcr = 1;
      }
      if (swcl == 1 && swcr == 1)
        break;
      if (len - cl - cr <= 0)
        break;
    }
    strncpy(word, &cad[cl], (size_t)(len - cl - cr));
  }
  word[len - cl - cr] = '\0';
  strcpy(cad, word);
  return (0);
}

int ReadLine(FILE *fp, char *cad) {
  /* Read a line from the file fp and copy it in the string pointer by cad.
     returns:
     the number of elements readed.
     if EOF is reached returns EOF
   */
  char c;
  int i;
  size_t status = 0;

  //  fscanf(fp,"%s",&cad);

  i = 0;
  status = fread(&c, sizeof(char), 1, fp);
  while (status > 0 && c != '\n' && i < MAX_LINE_LEN - 1) {
    memcpy(&cad[i], &c, 1);
    status = fread(&c, sizeof(char), 1, fp);
    i++;
  }
  if (c == '\n') { //  endofline
    cad[i] = '\0';
    return (i);
  } else {
    if (status <= 0) { // endoffile
      if (feof(fp) != 0)
        return (EOF);
      else {
        printf("ERROR: error reading file.\n");
        exit(EXIT_FAILURE);
      }
    } else { // endofbuffer
      printf("ERROR: line too long. Exceeds max size: %d Bytes\n",
             MAX_LINE_LEN);
      exit(EXIT_FAILURE);
    }
  }
}
