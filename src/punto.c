#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <SDL.h>

#define TRUE 1
#define FALSE 0

#define CIRCLE 0
#define SQUARE 1
#define SPHERE 2

#define PI 3.14159265358979323846
#define INC_ANG (PI / 200.0)

#define ZOOM 0
#define NAV 1

#define MAX_LINE_LEN 4096
#define MAX_WORD_LEN 128

#define LEDW 50
#define LEDH 50

#define MAXNFILES 9999

#define WIDTH 640
#define HEIGHT 480

#define MAX_NSIZE 64
#define MAX_NCOLORS 752
#define MAX_NSCOLORS 64

#define TRACE_DEFAULT 50
#define TRACE_MAX 500

#define SPECTRUM 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define GREY 4

#define FONTSIZE 3

#define RETURN 1
#define COMMENT 2
#define DATA 3

#define DEFAULTZOOM 0.9
#define DEFAULTPALETTE SPECTRUM
#define DEFAULTRADIO 12
#define DEFAULTTYPE SPHERE

#define COS45 .70710678118654752440

struct Point {
  float x, y, z;
};

struct Rectangle {
  int x, y;
  int l, h;
};

struct Vector {
  double x0, y0, z0;
  double x1, y1, z1;
};

struct Punto {
  float x, y, z;
  float x1, y1, z1;
  float radio;
  float color;
  float vx, vy, vz;
  float vx1, vy1, vz1;
  SDL_Surface *sprite;
};

struct Sprite {
  SDL_Surface *surface;
  Uint32 time;
  int cont;
  int on;
};

struct PosCol {
  int x, y, z;
  int r, c;
  int vx, vy, vz;
};

struct DataFile {
  FILE *fp;
  char name[MAX_WORD_LEN];
  int nblocks;
  int ncol;
  long size;
  int eof;
};

struct RGBColor {
  Uint8 r, g, b;
  char name[MAX_WORD_LEN];
};

struct Block {
  long pos;
  long num;

  char *comment;
  struct Block *next;
  struct Block *prev;
};

struct Values {
  float delay;
  int dimension;
  float max_r, min_r;
  float max_c, min_c;
  float max_f, min_f;
  float max_x, min_x, max_y, min_y, max_z, min_z;
};

struct Keys {
  short z;
  short i;
  short q;
  short b;
  short c;
  short f;
  short s;
  short S;
  short r;
  short F;
  short n;
  short u;
  short t;
  short p;
  short plus;
  short minus;
  short period;
  short arrow;
  short up, down, left, right;
  short pageup, pagedown;
  short shift;
  short less;
  short space;
  short k_4, k_5, k_6, k_7, k_8, k_9, k_0;
  short pressed;
  short mclick;
  short mbdown;
  short mbup;
  short user;
};

struct Window {
  SDL_Window *sdl_window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Surface *screen;
  int w, h;
  Uint32 flags;
  struct Point shift;
};

struct Universe {
  struct Point ang;
  float zoom;
  float zoom_size;
  struct Point dshift;
  struct Point cm, cw, cv; /* centro geometrico datos, centro de la ventana,
                              centro visualizacion */
  struct Point escale;
  int psize;
};

struct Options {
  short color;
  short radio;
  short geometria;
  short number;
  short box;
  short comment;
  short realsize;
  short dimension;
  short size;
  short fast;
  short cut;
  short type;
  short column;
  short real_color;
  short gradient;
  short field;
  short fieldcom;
  short varfield;
  short palette;
  short verbose;
  short back;
  short fore;
  short led;
  short file;
  short signal;
  short sizebox;
  short pausa;
  short rotate;
  short mode;
  short anim;
  short reverse;
  short save;
  short title;
  short trace;
  short ntrace;
  short tracelines;
  short periodic;
  short exit;
};

struct Parametres {
  char colstr[MAX_WORD_LEN];
  char boxstr[MAX_WORD_LEN];
  char backname[MAX_WORD_LEN];
  char forename[MAX_WORD_LEN];
  char ledname[MAX_WORD_LEN];
  char rootfilename[MAX_WORD_LEN];
  char titlewindow[MAX_WORD_LEN];
  char path[MAX_WORD_LEN];

  int size;
  int pausa;
  long N, N1, N2;
  int ntrace;
  int dimension;
  int width, height;
  float max_r, min_r;
  float max_c, min_c;
  float max_f, min_f;
  float zoomz;
  struct Vector box;
};

#include "fonts-1.xpm"
#include "rgb.h"

long RLine(char *cad, int, FILE *fp);
void GetFileName(int, char **, char *);
int NumRet(FILE *);
long CountLines(char *);
int NColumns(char *);
int FindChain(char *target, char *cad[], int n);
int AnalizeLine(char *cad);
long SizeFile(char *);
struct Block *CreateBlock(void);
int ReadNBlocks(char *fname, struct Block *br, long fpos);
void Rapida(long *a, int izq, int drch, struct Punto *p);
void TCR(float a, float b, float c, long, long, struct Point r, float comp,
         short field, int *reset_box, struct Punto *p);
void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void DrawLine(SDL_Surface *surface, int x0, int y0, int x1, int y1,
              Uint32 color);
void PutPixelCircle(SDL_Surface *screen, int x, int y, int x0, int y0,
                    double razon, Uint32 color);
void DrawCircle(SDL_Surface *screen, int x0, int y0, int r, Uint32 color);
void FillRing(SDL_Surface *surface, int x0, int y0, int r, int h,
              Uint32 color);
void FillCircle(SDL_Surface *surface, int x0, int y0, int r, Uint32 color);
SDL_Surface *CreateBall(SDL_Surface *screen, Uint32 color, Uint32 bcolor,
                        int radio);
SDL_Surface *CreateBall3D(SDL_Surface *screen, Uint32 color, int radio);
void DrawRectangle(SDL_Surface *surface, int x0, int y0, int x1, int y1,
                   Uint32 color);
void FillRectangle(SDL_Surface *surface, int x0, int y0, int x1, int y1,
                   Uint32 color);
SDL_Surface *CreateRectangle(SDL_Surface *screen, Uint32 color, Uint32 bcolor,
                             int radio);
int DrawText(SDL_Surface *screen, char *text, int x, int y, Uint32 color);
int DrawChar(SDL_Surface *screen, char **text, int x, int y, Uint32 color);
int Sqrt(int x);
void ReadRGBColors(char *fname, struct RGBColor *color, int n);
int LookUpColor(char *name, struct RGBColor *table, SDL_Color *color);
void DrawArrow(SDL_Surface *screen, SDL_Rect pos, struct Point f, float factor,
               Uint32 color, int type);
void DrawScale(SDL_Surface *screen, Uint32 *color, Uint32 border, struct Values,
               int x, int y);
void DrawFile(SDL_Surface *screen, Uint32 color, Uint32 border,
              struct Rectangle rec, struct DataFile);
void GenColorTable(SDL_Surface *screen, Uint32 *colortable, int type);
void GenMiniColorTable(SDL_Surface *screen, Uint32 *ct, int n, SDL_Color color1,
                       SDL_Color color2);
void WriteBitmap(SDL_Surface *bitmap, char *);
void DrawBox(struct Window w, struct Punto *box, int dim, struct Point c_v,
             float z, Uint32 color);
void InfoColors(struct RGBColor *rgbcolortable);
int ParseEvent(SDL_Event *event);
int CheckEvent(SDL_Event *event);
int NextEvent(SDL_Event *event);
int EventLoop(SDL_Event event, struct Window *w, struct Keys *k);
void Usage(char *ver, char *l_rev);
void DrawInfo(SDL_Surface *screen, struct Window win, struct Universe,
              struct DataFile df);
void MovePuntos(struct Universe, struct Parametres par, struct Punto *);
void DrawAll(SDL_Surface *screen, struct Window win, struct Universe u,
             struct Parametres par, Uint32 background, struct DataFile df);
void DrawPuntos(SDL_Surface *screen, struct Window w, struct Universe,
                struct Punto *, long *, struct Point, float,
                struct Parametres val, struct Values data);
void DrawTracePuntos(SDL_Surface *screen, struct Window w, struct Universe u,
                     struct Parametres par, float z, int order);
int DrawPeriodicPuntos(SDL_Surface *screen, struct Window w, struct Universe,
                       struct Punto *, long *, struct Point, float,
                       struct Parametres val, struct Values data);
void RangeOfValues(struct Punto *p, struct Options, struct Values *data);
int CountSprites(struct Sprite *b, int nmax);
void InfoSprites(struct Sprite *b, struct Options opt);
void PurgeSprites(struct Sprite *, int);
struct Punto *ReadBlock(int *n, struct DataFile, struct PosCol pos, long *,
                        struct Punto *p, int *);
int NextBlock(float x, char *fname);
int Arguments(int argc, char *argv[], struct Options *option,
              struct Parametres *val);
void SetValues(struct Values val_d, struct Parametres par, struct Options opt,
               struct Values *val);
int SetDimension(struct Parametres par, struct Options opt, int ncol,
                 int noptcol);
int GetColumns(char *str, int ncolf, int *p_col);
void OrderOfColumns(struct Options opt, int d, int ncol, int noptc, int *posc,
                    struct PosCol *p);
int GetBox(int d, struct Parametres *param);
void BoxProperties(struct Values val_d, struct Parametres param,
                   struct Options opt, struct Punto *b);
void SetInitialValues(struct Options *opt, struct Keys *k);

char version[] = {"1.0.04"};
char last_revision[] = {"Jun 2009"};

struct Sprite *bola;
int numsprites;

int actualizar = TRUE;
int ntrace = TRACE_DEFAULT;

struct Options option;

struct Punto box[8];

int max_nsize;

long numactivados;
long N;
long max_N;

int nextblock = 0;

struct Block *block;
struct Punto *punto;
long *tabla;
int reset_box = TRUE;

int Cont = 0;
int kEOF = FALSE;

Uint32 colortable[MAX_NSCOLORS];
SDL_Surface *screen;
struct Keys kp;
Uint32 defaultcolor, ledcolor;
Uint32 bgcolor, fgcolor;

struct Values val_data, val_run;
float color_factor;

struct Punto *traceball[TRACE_MAX];

char *comment;
char comentario[MAX_WORD_LEN];
char nocomment[MAX_WORD_LEN] = "No Comment";

struct Block *broot;
struct Rectangle filerect;

long RLine(char *cad, int len, FILE *fp) {
  if (fgets(cad, len, fp) == NULL)
    return EOF;
  size_t n = strlen(cad);
  if (n > 0 && cad[n - 1] == '\n')
    cad[--n] = '\0';
  return (long)n;
}

void GetFileName(int argc, char *argv[], char *fname) {
  snprintf(fname, MAX_WORD_LEN, "%s", argv[argc - 1]);
}

int NumRet(FILE *fp) {
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
      if (s == '\n') {
        ret++;
        file_pos = ftell(fp);
      } else if ((s != ' ') && (s != '\t') && (s != '#') && (s != '%') &&
                 (s < 'a' || s > 'z') && (s < 'A' || s > 'Z')) {
        file_pos = ftell(fp) - 1;
        break;
      }
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
          if (s == '\n') {
            ret++;
            file_pos = ftell(fp);
            break;
          }
        }
      }
    }
  }

  fseek(fp, file_pos, SEEK_SET);
  if (eof)
    return (-1);
  return (ret);
}

long CountLines(char *fname) {
  FILE *fp;
  char s;
  int nlines, nchar, comment;
  int endblock;

  if ((fp = fopen(fname, "r")) == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  (void)NumRet(fp);

  nlines = 0;
  endblock = 0;
  nchar = 0;
  comment = 0;
  do {
    if (fscanf(fp, "%c", &s) != EOF) {
      switch (s) {
      case '\n':
        if (nchar == 0)
          endblock++;
        else {
          nlines++;
        }
        comment = 0;
        nchar = 0;
        break;
      case '#':
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
{
  FILE *fp;
  struct stat info;
  int fd;
  char s;
  int c, dentropal;
  int np;
  int cont = 0;
  int sw = 0;


  if ((fd = open(fname, O_RDONLY)) == -1) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
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
  if ((fp = fopen(fname, "r")) == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }

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
  int i;

  for (i = 0; i < n; i++) {
    if (strncmp(target, cad[i], MAX_WORD_LEN) == 0) {
      return (i);
    }
  }
  return (-1);
}

int AnalizeLine(char *cad) {
  int i;
  for (i = 0; cad[i]; i++) {
    if (isalpha(cad[i]) || cad[i] == '#' || cad[i] == '%')
      return COMMENT;
    if (cad[i] == '\n')
      return RETURN;
    if (cad[i] != ' ' && cad[i] != '\t')
      return DATA;
  }
  return 0;
}

long SizeFile(char *filename) {
  struct stat info;
  if (stat(filename, &info) == -1) {
    perror(filename);
    exit(EXIT_FAILURE);
  }
  return info.st_size;
}

int ReadNBlocks(char *fname, struct Block *br, long fpos) {
  FILE *fp;

  char line[MAX_LINE_LEN];
  int type;
  int sw;

  long file_pos;
  long file_pos0;
  long file_size;
  long incfile_size;
  struct Block *lastb;
  long cont;
  long fcont = 0;
  int inblock = 0;
  int indata = 0;
  int infile = 0;
  int numblock = 0;
  long nrbytes = 0;

  if ((fp = fopen(fname, "r")) == NULL) {
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
  indata = 0;
  nrbytes = 0;

  do {
    file_pos += nrbytes; // HERE ftell(fp);//-incpos;

    strncpy(line, "", (size_t)1);
    nrbytes = RLine(line, MAX_LINE_LEN, fp);

    type = AnalizeLine(line);

    if ((file_pos - nrbytes - file_pos0 > incfile_size)) {
      fflush(stdout);
      file_pos0 = file_pos - nrbytes;
      fcont++;
    }
    if (nrbytes < 0) {
      infile = -1;
    }

    if (type == DATA) {
      indata = 1;
      cont++;
    }
    if (type == COMMENT) {
    }

    if (inblock == 0) {
      if (type == COMMENT || type == DATA) {
        inblock = 1;
        if (infile == 0) {
          infile = 1;
          lastb->pos = file_pos;
        } else {
          numblock++;
          lastb->next = CreateBlock();
          lastb->next->prev = lastb;
          lastb = lastb->next;
          lastb->next = 0;
          lastb->pos = file_pos;
          lastb->num = 0;
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
        inblock = 0;
        indata = 0;
        lastb->num = cont;
        cont = 0;
      }
    }
  } while (infile > 0);
  fclose(fp);
  return (numblock);
}

struct Block *CreateBlock(void) {
  struct Block *bloque;

  if ((bloque = malloc(sizeof(struct Block))) == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  bloque->pos = 0;
  bloque->num = 0;
  bloque->comment = NULL;
  bloque->next = NULL;
  bloque->prev = NULL;

  return (bloque);
}

void TCR(float a, float b, float c, long n1, long n2, struct Point r,
         float comp, short field, int *reset_box, struct Punto *p) {
  long i, j, k;
  static float A0[3][3];
  float A1[3][3];
  float temp[3];
  float cosa, sina, cosb, sinb, cosc, sinc;


  if (*reset_box == TRUE) {
    *reset_box = FALSE;

    A0[0][0] = 1;
    A0[0][1] = 0;
    A0[0][2] = 0;

    A0[1][0] = 0;
    A0[1][1] = 1;
    A0[1][2] = 0;

    A0[2][0] = 0;
    A0[2][1] = 0;
    A0[2][2] = 1;
  }

  cosa = cosb = cosc = 1.;
  sina = sinb = sinc = 0.;

  if (a != 0) {
    cosa = (float)cos(a);
    sina = (float)sin(a);
  }
  if (b != 0) {
    cosb = (float)cos(b);
    sinb = (float)sin(b);
  }
  if (c != 0) {
    cosc = (float)cos(c);
    sinc = (float)sin(c);
  }

  A1[0][0] = cosb * cosc;
  A1[0][1] = cosb * sinc;
  A1[0][2] = sinb;

  A1[1][0] = -sina * sinb * cosc - cosa * sinc;
  A1[1][1] = -sina * sinb * sinc + cosa * cosc;
  A1[1][2] = sina * cosb;

  A1[2][0] = -cosa * sinb * cosc + sina * sinc;
  A1[2][1] = -cosa * sinb * sinc - sina * cosc;
  A1[2][2] = cosa * cosb;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      temp[j] = 0.0;
      for (k = 0; k < 3; k++)
        temp[j] += A1[j][k] * A0[k][i];
    }
    for (k = 0; k < 3; k++)
      A0[k][i] = temp[k];
  }

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      A1[i][j] = A0[i][j] * comp;
    }
  }


  for (i = n1; i < n2; i++) {
    p[i].x1 = (A1[0][0] * (p[i].x - r.x) + A1[0][1] * (p[i].y - r.y) +
               A1[0][2] * (p[i].z - r.z));
    p[i].y1 = (A1[1][0] * (p[i].x - r.x) + A1[1][1] * (p[i].y - r.y) +
               A1[1][2] * (p[i].z - r.z));
    p[i].z1 = (A1[2][0] * (p[i].x - r.x) + A1[2][1] * (p[i].y - r.y) +
               A1[2][2] * (p[i].z - r.z));
  }

  if (field == TRUE) {

    for (i = n1; i < n2; i++) {
      p[i].vx1 = ((A1[0][0] * (p[i].vx) + A1[0][1] * (p[i].vy) +
                   A1[0][2] * (p[i].vz))) /
                 comp;
      p[i].vy1 = ((A1[1][0] * (p[i].vx) + A1[1][1] * (p[i].vy) +
                   A1[1][2] * (p[i].vz))) /
                 comp;
      p[i].vz1 = ((A1[2][0] * (p[i].vx) + A1[2][1] * (p[i].vy) +
                   A1[2][2] * (p[i].vz))) /
                 comp;
    }
  }
}

void Rapida(long *a, int izq, int drch, struct Punto *p)
{
  long i, j, k;
  double z;

  if (drch <= izq)
    return;
  i = izq;
  j = drch;

  z = p[a[(izq + drch) / 2]].z1;
  do {
    while (p[a[i]].z1 < z && i < drch)
      i++;
    while (z < p[a[j]].z1 && j > izq)
      j--;
    if (i <= j) {
      k = a[i];
      a[i] = a[j];
      a[j] = k;
      i++;
      j--;
    }
  } while (i <= j);
  if (izq < j)
    Rapida(a, izq, j, p);
  if (i < drch)
    Rapida(a, i, drch, p);
}

void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  Uint8 bpp = surface->format->BytesPerPixel;

  Uint8 *p;

  if (x < 0)
    return;
  if (y < 0)
    return;
  if (x >= surface->w)
    return;
  if (y >= surface->h)
    return;

  p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(Uint16 *)p = pixel;
    break;

  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;

  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
}

void DrawLine(SDL_Surface *surface, int x0, int y0, int x1, int y1,
              Uint32 color) {
  int t, distancia;
  int xerr = 0, yerr = 0, d_x, d_y;
  int incx, incy;

  d_x = x1 - x0;
  d_y = y1 - y0;

  if (d_x > 0)
    incx = 1;
  else if (d_x == 0)
    incx = 0;
  else
    incx = -1;

  if (d_y > 0)
    incy = 1;
  else if (d_y == 0)
    incy = 0;
  else
    incy = -1;

  d_x = abs(d_x);
  d_y = abs(d_y);
  if (d_x > d_y)
    distancia = d_x;
  else
    distancia = d_y;

  for (t = 0; t <= distancia + 1; t++) {
    PutPixel(surface, x0, y0, color);
    xerr += d_x;
    yerr += d_y;
    if (xerr > distancia) {
      xerr -= distancia;
      x0 += incx;
    }
    if (yerr > distancia) {
      yerr -= distancia;
      y0 += incy;
    }
  }
}


void PutPixelCircle(SDL_Surface *screen, int x, int y, int x0, int y0,
                    double razon, Uint32 color) {
  int startx, endx, x1, starty, endy, y1;

  starty = (int)(y * razon);
  endy = (int)((y + 1) * razon);
  startx = (int)(x * razon);
  endx = (int)((x + 1) * razon);

  for (x1 = startx; x1 < endx; ++x1) {
    PutPixel(screen, x1 + x0, y + y0, color);
    PutPixel(screen, x1 + x0, y0 - y, color);
    PutPixel(screen, x0 - x1, y0 - y, color);
    PutPixel(screen, x0 - x1, y + y0, color);
  }

  for (y1 = starty; y1 < endy; ++y1) {
    PutPixel(screen, y1 + x0, x + y0, color);
    PutPixel(screen, y1 + x0, y0 - x, color);
    PutPixel(screen, x0 - y1, y0 - x, color);
    PutPixel(screen, x0 - y1, x + y0, color);
  }
}

void DrawCircle(SDL_Surface *screen, int x0, int y0, int r, Uint32 color) {
  int x, y, delta;
  float razon;

  razon = 1.;
  y = r;
  delta = 3 - 2 * r;

  for (x = 0; x < y;) {
    PutPixelCircle(screen, x, y, x0, y0, razon, color);

    if (delta < 0)
      delta += 4 * x + 6;
    else {
      delta += 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
  x = y;
  if (y)
    PutPixelCircle(screen, x, y, x0, y0, razon, color);
}

void FillRing(SDL_Surface *surface, int x0, int y0, int r, int h,
              Uint32 color) {
  int x, y;
  int r2;
  int y1;

  if (r <= 0)
    return;

  r2 = r * r;
  y1 = (int)(r * COS45 + .5);

  for (y = 0; y <= y1; y++) {

    x = Sqrt(r2 - y * y);

    DrawLine(surface, x0 - x, y0 + y, x0 - x + h, y0 + y, color);
    DrawLine(surface, x0 + x - h, y0 + y, x0 + x, y0 + y, color);

    DrawLine(surface, x0 - x, y0 - y, x0 - x + h, y0 - y, color);
    DrawLine(surface, x0 + x - h, y0 - y, x0 + x, y0 - y, color);

    DrawLine(surface, y0 + y, x0 + x, y0 + y, x0 + x - h, color);
    DrawLine(surface, y0 + y, x0 - x + h, y0 + y, x0 - x, color);

    DrawLine(surface, y0 - y, x0 + x, y0 - y, x0 + x - h, color);
    DrawLine(surface, y0 - y, x0 - x + h, y0 - y, x0 - x, color);
  }
}

void FillCircle(SDL_Surface *surface, int x0, int y0, int r, Uint32 color) {
  int x, y;
  float r2;

  r2 = (float)r * r;

  if (r == 0) {
    PutPixel(surface, x0, y0, color);
  } else {
    for (y = 0; y <= r; y++) {
      x = (int)(sqrt(r2 - y * y) + .5);
      DrawLine(surface, x0 - x, y0 + y, x0 + x, y0 + y, color);
      DrawLine(surface, x0 - x, y0 - y, x0 + x, y0 - y, color);
    }
  }
}

SDL_Surface *CreateBall(SDL_Surface *screen, Uint32 color, Uint32 bcolor,
                        int radio) {
  SDL_Surface *temp;
  Uint32 background;

  temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 2 * radio + 1, 2 * radio + 1,
                              (int)screen->format->BitsPerPixel,
                              screen->format->Rmask, screen->format->Gmask,
                              screen->format->Bmask, screen->format->Amask);

  background = SDL_MapRGB(temp->format, 0x00, 0x00, 0x00);
  SDL_FillRect(temp, NULL, background);
  SDL_SetColorKey(temp, (Uint32)(SDL_TRUE), background);

  FillCircle(temp, radio, radio, radio, color);
  DrawCircle(temp, radio, radio, radio, bcolor);

  return temp;
}

SDL_Surface *CreateBall3D(SDL_Surface *screen, Uint32 color, int radio) {
  SDL_Surface *temp;
  SDL_Surface *sprite;
  Uint32 background;
  float i;
  Uint8 red, green, blue;
  float ir, ig, ib;
  Uint8 r, g, b;
  float inci;

  temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 2 * radio + 1, 2 * radio + 1,
                              (int)screen->format->BitsPerPixel,
                              screen->format->Rmask, screen->format->Gmask,
                              screen->format->Bmask, screen->format->Amask);
  if (temp == NULL) {
    fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  background = SDL_MapRGB(temp->format, 0x00, 0x00, 0x00);
  SDL_FillRect(temp, NULL, background);
  SDL_SetColorKey(temp, (Uint32)(SDL_TRUE), background);

  SDL_GetRGB(color, screen->format, &red, &green, &blue);
  r = red;
  g = green;
  b = blue;
  ir = ((float)red / radio) * .4;
  ig = ((float)green / radio) * .4;
  ib = ((float)blue / radio) * .4;

  if (radio < 2) {
    color = SDL_MapRGB(temp->format, r, g, b);
    FillCircle(temp, radio, radio, radio, color);
  } else {
    inci = 1;
    inci = (float)radio / 20.;
    if (inci < 1)
      inci = 1;
    for (i = (float)radio; i > 0; i -= inci) {
      r = red - ir * i;
      g = green - ig * i;
      b = blue - ib * i;
      color = SDL_MapRGB(temp->format, r, g, b);
      FillRing(temp, radio, radio, (int)(i), (int)(inci * 1.4142 + .5) + 1,
               color);
    }
  }

  sprite = SDL_ConvertSurface(temp, screen->format, 0);
  if (sprite == NULL) {
    return (temp);
  }
  SDL_FreeSurface(temp);
  return (sprite);
}

void DrawRectangle(SDL_Surface *surface, int x0, int y0, int x1, int y1,
                   Uint32 color) {
  DrawLine(surface, x0, y0, x1, y0, color);
  DrawLine(surface, x0, y0, x0, y1, color);
  DrawLine(surface, x0, y1, x1, y1, color);
  DrawLine(surface, x1, y1, x1, y0, color);
}

void FillRectangle(SDL_Surface *surface, int x0, int y0, int x1, int y1,
                   Uint32 color) {
  int y;

  for (y = y0; y <= y1; y++) {
    DrawLine(surface, x0, y, x1, y, color);
  }
}

SDL_Surface *CreateRectangle(SDL_Surface *screen, Uint32 color, Uint32 bcolor,
                             int radio) {
  SDL_Surface *temp;
  Uint32 background;

  temp = SDL_CreateRGBSurface(SDL_SWSURFACE, radio, radio,
                              (int)screen->format->BitsPerPixel,
                              screen->format->Rmask, screen->format->Gmask,
                              screen->format->Bmask, screen->format->Amask);

  background = SDL_MapRGB(temp->format, 0x00, 0x00, 0x00);
  SDL_FillRect(temp, NULL, background);
  SDL_SetColorKey(temp, (Uint32)(SDL_TRUE), background);

  FillRectangle(temp, 0, 0, radio, radio, color);
  DrawRectangle(temp, 0, 0, radio - 1, radio - 1, bcolor);

  return temp;
}

int DrawText(SDL_Surface *screen, char *text, int x0, int y0, Uint32 color) {
  static char **font_table[128];
  static int init = 0;
  size_t i, len;
  int x;
  unsigned char ch;

  if (!init) {
    for (i = 0; i < 128; i++)
      font_table[i] = font_NONE;
    font_table[' '] = font_NULL;
    font_table['!'] = font_EXCLAIM;
    font_table['\\'] = font_BACKSLASH;
    font_table['"'] = font_QUOTEDBL;
    font_table['#'] = font_HASH;
    font_table['$'] = font_DOLLAR;
    font_table['%'] = font_CENT;
    font_table['&'] = font_AMPERSAND;
    font_table['\''] = font_QUOTE;
    font_table['('] = font_LEFTPAREN;
    font_table[')'] = font_RIGHTPAREN;
    font_table['*'] = font_ASTERISK;
    font_table['+'] = font_PLUS;
    font_table[','] = font_COMMA;
    font_table['-'] = font_MINUS;
    font_table['.'] = font_DOT;
    font_table['/'] = font_SLASH;
    font_table['0'] = font_0;
    font_table['1'] = font_1;
    font_table['2'] = font_2;
    font_table['3'] = font_3;
    font_table['4'] = font_4;
    font_table['5'] = font_5;
    font_table['6'] = font_6;
    font_table['7'] = font_7;
    font_table['8'] = font_8;
    font_table['9'] = font_9;
    font_table[':'] = font_COLON;
    font_table[';'] = font_SEMICOLON;
    font_table['<'] = font_LESS;
    font_table['='] = font_EQUALS;
    font_table['>'] = font_GREATER;
    font_table['?'] = font_QUESTION;
    font_table['@'] = font_AT;
    font_table['A'] = font_A;
    font_table['B'] = font_B;
    font_table['C'] = font_C;
    font_table['D'] = font_D;
    font_table['E'] = font_E;
    font_table['F'] = font_F;
    font_table['G'] = font_G;
    font_table['H'] = font_H;
    font_table['I'] = font_I;
    font_table['J'] = font_J;
    font_table['K'] = font_K;
    font_table['L'] = font_L;
    font_table['M'] = font_M;
    font_table['N'] = font_N;
    font_table['O'] = font_O;
    font_table['P'] = font_P;
    font_table['Q'] = font_Q;
    font_table['R'] = font_R;
    font_table['S'] = font_S;
    font_table['T'] = font_T;
    font_table['U'] = font_U;
    font_table['V'] = font_V;
    font_table['W'] = font_W;
    font_table['X'] = font_X;
    font_table['Y'] = font_Y;
    font_table['Z'] = font_Z;
    font_table['['] = font_LEFTBRACKET;
    font_table[']'] = font_RIGHTBRACKET;
    font_table['^'] = font_CARET;
    font_table['_'] = font_UNDERSCORE;
    font_table['`'] = font_BACKQUOTE;
    font_table['a'] = font_a;
    font_table['b'] = font_b;
    font_table['c'] = font_c;
    font_table['d'] = font_d;
    font_table['e'] = font_e;
    font_table['f'] = font_f;
    font_table['g'] = font_g;
    font_table['h'] = font_h;
    font_table['i'] = font_i;
    font_table['j'] = font_j;
    font_table['k'] = font_k;
    font_table['l'] = font_l;
    font_table['m'] = font_m;
    font_table['n'] = font_n;
    font_table['o'] = font_o;
    font_table['p'] = font_p;
    font_table['q'] = font_q;
    font_table['r'] = font_r;
    font_table['s'] = font_s;
    font_table['t'] = font_tt;
    font_table['u'] = font_u;
    font_table['v'] = font_v;
    font_table['w'] = font_w;
    font_table['x'] = font_x;
    font_table['y'] = font_y;
    font_table['z'] = font_z;
    font_table['{'] = font_LEFTKEY;
    font_table['|'] = font_OR;
    font_table['}'] = font_RIGHTKEY;
    font_table['~'] = font_WORM;
    init = 1;
  }

  len = strlen(text);
  for (i = 0; i < len; i++) {
    ch = (unsigned char)text[i];
    x = DrawChar(screen, ch < 128 ? font_table[ch] : font_NONE, x0, y0, color);
    x0 += x;
  }
  return (x0);
}

int DrawChar(SDL_Surface *screen, char **text, int x, int y, Uint32 color) {
  int i, j, k, l;
  static char str[MAX_WORD_LEN] = "";
  int lx, ly, c, s;
  char uno[2] = "+";
  int scale = 1;

  lx = ly = c = s = 0;
  if (strlen(text[0]) > MAX_WORD_LEN) {
    fprintf(stderr, "WARNING: text too long: %s\n", text[0]);
    fprintf(stderr, "text truncate\n");
  }
  strncpy(str, text[0], MAX_WORD_LEN);
  sscanf(str, "%d%d%d%d", &lx, &ly, &c, &s);
  for (j = 0, l = 0; j < ly; j++, l += scale) {
    strncpy(str, text[j + c + 1], MAX_WORD_LEN);
    for (i = 0, k = 0; i < lx; i++, k += scale) {
      if (!strncmp(&str[i], uno, 1)) {
        if (scale == 1) {
          PutPixel(screen, k + x, y + l, color);
        } else {
          int n, m;

          for (m = 0; m < scale; m++)
            for (n = 0; n < scale; n++)
              PutPixel(screen, k + x + n, y + l + m, color);
        }
      }
    }
  }
  return (scale * lx);
}

int Sqrt(int n) {
  static int sq[10000];
  static int cont = 0;

  if (n > 9999) {
    return ((int)(sqrt((double)n) + .5));
  }
  if (n < 0) {
    fprintf(stderr, "sqrt\n");
    exit(EXIT_FAILURE);
  }
  if (cont == 0) {
    int i;

    for (i = 0; i < 10000; i++) {
      sq[i] = (int)(sqrt((double)i) + .5);
    }
    cont = 1;
  }
  return (sq[n]);
}

int LookUpColor(char *name, struct RGBColor *table, SDL_Color *color) {
  int i;
  int status = -1;

  for (i = 0; i < MAX_NCOLORS; i++) {
    if (strncmp(name, table[i].name, MAX_WORD_LEN) == 0) {
      color->r = table[i].r;
      color->g = table[i].g;
      color->b = table[i].b;
      status = 0;
      break;
    }
  }
  return (status);
}

void ReadRGBColors(char *fname, struct RGBColor *color, int n) {
  FILE *fp;
  int i;
  int r, g, b;
  char name[MAX_WORD_LEN] = "";
  int status;
  size_t len;

  if ((fp = fopen(fname, "r")) == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < n; i++) {
    status = fscanf(fp, "%d%d%d%127s", &r, &g, &b, name);

    color[i].r = (Uint8)r;
    color[i].g = (Uint8)g;
    color[i].b = (Uint8)b;
    strncpy(color[i].name, name, MAX_WORD_LEN);
    if (status == 0) {
      i--;
      status = fscanf(fp, "%127s", name);
      strcat(color[i].name, " ");
      len = strlen(name);
      if (len > MAX_WORD_LEN - 1)
        len = MAX_WORD_LEN - 1;
      strcat(color[i].name, name);
    }
  }
  fclose(fp);
}

void DrawArrow(SDL_Surface *screen, SDL_Rect pos, struct Point f, float factor,
               Uint32 color, int type) {
  float sin_O1, cos_O1;
  float vx, vy;
  float r, x, y, x1, y1, x2, y2, x3, y3;
  float ang1;
  static float sin_O2 = .8660254038;  //  sin_O2=sin(PI/3.);
  static float cos_O2 = .5;           //     cos_O2=cos(PI/3.);
  static float sin_O3 = -.8660254038; //  sin_O3=sin(4.*PI/3.);
  static float cos_O3 = -.5;          // cos(4.*PI/3.);

  vx = f.x;
  vy = f.y;

  x = (float)(pos.x) - vx * factor;
  y = (float)(pos.y) + vy * factor;

  x1 = (float)(pos.x) + vx * factor;
  y1 = (float)(pos.y) - vy * factor;

  switch (type) {
  case 0:
    PutPixel(screen, (int)((x + x1) * .5), (int)((y + y1) * .5), color);
    break;
  case 1:
    DrawLine(screen, (int)x, (int)y, (int)x1, (int)y1, color);
    break;
  case 2:
    DrawLine(screen, (int)x, (int)y, (int)x1, (int)y1, color);
    ang1 = atan2(y1 - y, x1 - x);
    r = .5 * sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

    cos_O1 = (float)cos(ang1);
    sin_O1 = (float)sin(ang1);

    x2 = -r * sin_O2;
    y2 = -r * cos_O2;

    x3 = x2 * cos_O1 - y2 * sin_O1;
    y3 = x2 * sin_O1 + y2 * cos_O1;

    DrawLine(screen, (int)x1, (int)y1, (int)(x1 + x3), (int)(y1 + y3), color);

    x2 = r * sin_O3;
    y2 = -r * cos_O3;
    x3 = x2 * cos_O1 - y2 * sin_O1;
    y3 = x2 * sin_O1 + y2 * cos_O1;

    DrawLine(screen, (int)x1, (int)y1, (int)(x1 + x3), (int)(y1 + y3), color);
    break;
  default:
    break;
  }
}

void DrawFile(SDL_Surface *screen, Uint32 color, Uint32 border,
              struct Rectangle rec, struct DataFile df) {
  int i, len;
  long files, filep;

  files = SizeFile(
      df.name); // this function opens again the file. look for another way.
  filep = ftell(df.fp);
  len = (int)((rec.l - 2) * (float)filep / files);

  DrawRectangle(screen, rec.x, rec.y, rec.x + rec.l, rec.y + rec.h, border);
  for (i = 1; i < (rec.h) && len > 0; i++) {
    DrawLine(screen, rec.x + 1, rec.y + i, rec.x + len, rec.y + i, color);
  }

}

void DrawScale(SDL_Surface *screen, Uint32 *color, Uint32 bordercolor,
               struct Values val, int x, int y) {
  int i, j;
  int x0, y0, x1, y1;
  int w, h;
  char num[MAX_WORD_LEN] = "";
  float value;
  float inc;

  w = 10;
  h = 2 * MAX_NSCOLORS;
  x0 = x1 = x - 1;
  y0 = y - 1;
  y1 = y + h;
  DrawLine(screen, x0, y0, x1, y1, bordercolor);
  x0 = x1;
  y0 = y1;
  x1 = x + w + 1;
  y1 = y0;
  DrawLine(screen, x0, y0, x1, y1, bordercolor);
  x0 = x1;
  y0 = y1;
  x1 = x + w + 1;
  y1 = y - 1;
  DrawLine(screen, x0, y0, x1, y1, bordercolor);
  x0 = x1;
  y0 = y1;
  x1 = x - 1;
  y1 = y - 1;
  DrawLine(screen, x0, y0, x1, y1, bordercolor);
  inc = (val.max_c - val.min_c) / 10;
  for (i = 0; i <= 10; i++) {
    x0 = x + w + 2;
    y0 = y + (float)(h * i) / 10;
    x1 = x0 + 3;
    y1 = y0;
    DrawLine(screen, x0, y0, x1, y1, bordercolor);
    value = (10. - i) * inc + val.min_c;
    if (fabs(value) < inc / 1000)
      value = 0;
    snprintf(num, MAX_WORD_LEN, "%g", value);
    DrawText(screen, num, x0 + 5, y0 - 6, bordercolor);
  }

  for (i = MAX_NSCOLORS - 1, j = 0; i >= 0; i--, j += 2) {
    DrawLine(screen, x, y + j, x + w, y + j, color[i]);
    DrawLine(screen, x, y + j + 1, x + w, y + j + 1, color[i]);
  }

}

void GenColorTable(SDL_Surface *screen, Uint32 *colortable, int type) {
  SDL_Color color1, color2;

  color1.a = 0;
  color2.a = 0;

  switch (type) {
  case RED:
    color1.r = 255;
    color1.g = 255;
    color1.b = 0;

    color2.r = 255;
    color2.g = 0;
    color2.b = 0;

    GenMiniColorTable(screen, colortable, MAX_NSCOLORS, color1, color2);
    break;
  case GREEN:

    color1.r = 0;
    color1.g = 30;
    color1.b = 0;

    color2.r = 0;
    color2.g = 255;
    color2.b = 0;

    GenMiniColorTable(screen, colortable, MAX_NSCOLORS, color1, color2);
    break;
  case BLUE:
    color1.r = 0;
    color1.g = 0;
    color1.b = 30;

    color2.r = 0;
    color2.g = 0;
    color2.b = 255;

    GenMiniColorTable(screen, colortable, MAX_NSCOLORS, color1, color2);
    break;
  case GREY:
    color1.r = 30;
    color1.g = 30;
    color1.b = 30;

    color2.r = 255;
    color2.g = 255;
    color2.b = 255;

    GenMiniColorTable(screen, colortable, MAX_NSCOLORS, color1, color2);

    break;
  case SPECTRUM:
  default: {
    float inc_color;
    int cont;
    int fin;

    cont = 0;
    inc_color = (float)MAX_NSCOLORS / 5.;

    color1.r = 255;
    color1.g = 0;
    color1.b = 0;

    color2.r = 255;
    color2.g = 255;
    color2.b = 0;

    fin = (int)inc_color;
    GenMiniColorTable(screen, colortable, fin, color1, color2);

    cont += fin;
    color1.r = 255;
    color1.g = 255;
    color1.b = 0;

    color2.r = 0;
    color2.g = 255;
    color2.b = 0;
    fin = (int)(2. * inc_color - cont);
    GenMiniColorTable(screen, &colortable[cont], fin, color1, color2);

    cont += fin;
    color1.r = 0;
    color1.g = 255;
    color1.b = 0;

    color2.r = 0;
    color2.g = 255;
    color2.b = 255;
    fin = (int)(3. * inc_color - cont);
    GenMiniColorTable(screen, &colortable[cont], fin, color1, color2);

    cont += fin;
    color1.r = 0;
    color1.g = 255;
    color1.b = 255;

    color2.r = 0;
    color2.g = 0;
    color2.b = 255;
    fin = (int)(4. * inc_color - cont);
    GenMiniColorTable(screen, &colortable[cont], fin, color1, color2);

    cont += fin;
    color1.r = 0;
    color1.g = 0;
    color1.b = 255;

    color2.r = 255;
    color2.g = 0;
    color2.b = 255;
    fin = (int)(5. * inc_color - cont);
    GenMiniColorTable(screen, &colortable[cont], fin, color1, color2);
  } break;
  }
}

void GenMiniColorTable(SDL_Surface *screen, Uint32 *ct, int n, SDL_Color color1,
                       SDL_Color color2) {
  int i;
  float ir, ig, ib;
  Uint8 r, g, b;

  ir = (float)(color2.r - color1.r) / (float)n;
  ig = (float)(color2.g - color1.g) / (float)n;
  ib = (float)(color2.b - color1.b) / (float)n;

  for (i = 0; i < n; i++) {
    r = (Uint8)(color1.r + ir * i);
    g = (Uint8)(color1.g + ig * i);
    b = (Uint8)(color1.b + ib * i);
    ct[i] = SDL_MapRGB(screen->format, r, g, b);
  }
}

void WriteBitmap(SDL_Surface *bitmap, char *fpname) {
  static int save_cont = 0;
  char name[MAX_WORD_LEN] = "";

  snprintf(name, MAX_WORD_LEN, "%s%04d.bmp", fpname, save_cont);

  if (SDL_SaveBMP(bitmap, name) == -1) {
    fprintf(stderr, "Error saving bitmap\n");
  }

  if (save_cont < MAXNFILES)
    save_cont++;
  else {
    fprintf(stderr, "Warning: maximun number of files reached.\n");
  }
  printf("saved: %s\n", name);
}

void DrawBox(struct Window w, struct Punto *box, int dim, struct Point c_v,
             float z, Uint32 color) {
  struct Punto p[8];
  int i, j, k;
  int m, n;
  float minz;
  int alto;
  int width, height;

  static int box_d[8][3] = {{2, 5, 7}, {3, 4, 6}, {0, 5, 7}, {1, 4, 6},
                             {1, 3, 6}, {0, 2, 7}, {1, 3, 4}, {0, 2, 5}};
  static int box_l[8][3] = {{1, 3, 4}, {0, 2, 5}, {1, 3, 6}, {0, 2, 7},
                             {0, 5, 7}, {1, 4, 6}, {2, 5, 7}, {3, 4, 6}};

  width = w.w;
  height = w.h;
  alto = height;
  k = 0;
  minz = box[0].z1;

  for (i = 0; i < 8; i++) {
    p[i].x = (box[i].x1) * z + c_v.x;
    p[i].y = (float)(alto - ((box[i].y1) * z + c_v.y));

    if (box[i].z1 < minz) {
      minz = box[i].z1;
      k = i;
    }
  }
  if (dim <= 2) {
    DrawLine(w.screen, (int)p[3].x, (int)p[3].y, (int)p[2].x, (int)p[2].y,
             color);
    DrawLine(w.screen, (int)p[2].x, (int)p[2].y, (int)p[6].x, (int)p[6].y,
             color);
    DrawLine(w.screen, (int)p[6].x, (int)p[6].y, (int)p[7].x, (int)p[7].y,
             color);
    DrawLine(w.screen, (int)p[7].x, (int)p[7].y, (int)p[3].x, (int)p[3].y,
             color);
  }
  if (dim == 3) {
    for (i = 0; i < 3; i++) {
      m = box_d[k][i];
      for (j = 0; j < 3; j++) {
        n = box_l[m][j];
        DrawLine(w.screen, (int)p[m].x, (int)p[m].y, (int)p[n].x, (int)p[n].y,
                 color);
      }
    }
  }
  DrawLine(w.screen, (int)(width * .5 - 5), (int)(height * .5),
           (int)(width * .5 + 5), (int)(height * .5), color);
  DrawLine(w.screen, (int)(width * .5), (int)(height * .5 - 5),
           (int)(width * .5), (int)(height * .5 + 5), color);

}

void InfoColors(struct RGBColor *rgbcolortable) {
  int i;

  printf("Colors that can be used:\n");
  printf("format in RGB mode:\nnumberID: red gren blue name\n");
  for (i = 0; i < MAX_NCOLORS; i++) {
    printf("%d: %d %d %d %s\n", i, (int)rgbcolortable[i].r,
           (int)rgbcolortable[i].g, (int)rgbcolortable[i].b,
           rgbcolortable[i].name);
  }
}

int ParseEvent(SDL_Event *event) {
  int status;

  status = FALSE;

  switch (event->type) {
  case SDL_USEREVENT:
  case SDL_KEYDOWN:
  case SDL_KEYUP:
  case SDL_MOUSEBUTTONUP:
  case SDL_MOUSEBUTTONDOWN:
  case SDL_WINDOWEVENT:
  case SDL_QUIT:
    status = TRUE;
    break;

  default:
    break;
  }

  return (status);
}

int CheckEvent(SDL_Event *event) {
  int status;

  status = FALSE;
  while (SDL_PollEvent(event) > 0 && status == FALSE) {
    status = ParseEvent(event);
  }

  return (status);
}

int NextEvent(SDL_Event *event) {
  int status;
  status = FALSE;

  while (status == FALSE) {
    SDL_WaitEvent(event);
    status = ParseEvent(event);
  }
  return (status);
}

int EventLoop(SDL_Event event, struct Window *w, struct Keys *k) {
  int status;

  static int block_a = FALSE;
  int w0;
  int h0;

  k->pressed = FALSE;
  status = FALSE;
  switch (event.type) {

  case SDL_USEREVENT:

    status = TRUE;

    k->user = TRUE;
    break;
  case SDL_WINDOWEVENT:
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      w0 = event.window.data1;
      h0 = event.window.data2;
      w->shift.x += ((double)w0 - w->w) / 2.;
      w->shift.y += ((double)h0 - w->h) / 2.;
      w->w = w0;
      w->h = h0;

      SDL_FreeSurface(w->screen);
      w->screen = SDL_CreateRGBSurface(0, w->w, w->h, 32, 0x00FF0000,
                                       0x0000FF00, 0x000000FF, 0xFF000000);
      screen = w->screen;
      SDL_DestroyTexture(w->texture);
      w->texture =
          SDL_CreateTexture(w->renderer, SDL_PIXELFORMAT_ARGB8888,
                            SDL_TEXTUREACCESS_STREAMING, w->w, w->h);
      status = TRUE;
    }
    break;
  case SDL_KEYDOWN:
    status = TRUE;

    k->shift = FALSE;
    if (event.key.keysym.mod & KMOD_LSHIFT ||
        event.key.keysym.mod & KMOD_RSHIFT) {
      k->shift = TRUE;
    }

    switch (event.key.keysym.sym) {
    case SDLK_q:
      k->q = TRUE;
      break;
    case SDLK_0:
    case SDLK_KP_0:
      k->k_0 = TRUE;
      break;
    case SDLK_LESS:
      k->less = TRUE;
      break;
    case SDLK_z:
      k->z = TRUE;
      break;
    case SDLK_PLUS:
    case SDLK_KP_PLUS:
      k->plus = TRUE;
      break;
    case SDLK_MINUS:
    case SDLK_KP_MINUS:
      k->minus = TRUE;
      break;
    case SDLK_KP_PERIOD:
    case SDLK_PERIOD:
      k->period = TRUE;
      break;
    case SDLK_i:
      k->i = TRUE;
      break;
    case SDLK_u:
      k->u = TRUE;
      break;
    case SDLK_b:
      k->b = TRUE;
      break;
    case SDLK_c:
      k->c = TRUE;
      break;
    case SDLK_p:
      k->p = TRUE;
      break;

    case SDLK_t:
      k->t = TRUE;
      break;
    case SDLK_a:
      k->pressed = TRUE;
      if (block_a == FALSE) {
        block_a = TRUE;
        k->arrow++;
        if (k->arrow > 2)
          k->arrow = 0;
      }
      break;
    case SDLK_n:
      k->n = TRUE;
      break;
    case SDLK_f:
      if (k->shift == FALSE) {
        if (k->f == FALSE)
          k->f = TRUE;
      } else {
        k->F = TRUE;
      }
      break;
    case SDLK_s:
      if (k->shift == FALSE) {
        if (k->s == FALSE)
          k->s = TRUE;
      } else {
        if (k->S == FALSE)
          k->S = TRUE;
      }
      break;
    case SDLK_r:
      k->r = TRUE;
      break;
    case SDLK_SPACE:
      k->space = TRUE;
      break;
    case SDLK_RIGHT:
      k->right = TRUE;
      break;
    case SDLK_LEFT:
      k->left = TRUE;
      break;
    case SDLK_UP:
      k->up = TRUE;
      break;
    case SDLK_DOWN:
      k->down = TRUE;
      break;
    case SDLK_PAGEUP:
      k->pageup = TRUE;
      break;
    case SDLK_PAGEDOWN:
      k->pagedown = TRUE;
      break;
    case SDLK_KP_7:
    case SDLK_7:
      k->pressed = TRUE;
      k->k_7 = TRUE;
      break;
    case SDLK_8:
    case SDLK_KP_8:
      k->pressed = TRUE;
      k->k_8 = TRUE;
      break;
    case SDLK_9:
    case SDLK_KP_9:
      k->pressed = TRUE;
      k->k_9 = TRUE;
      break;
    case SDLK_4:
    case SDLK_KP_4:
      k->pressed = TRUE;
      k->k_4 = TRUE;
      break;
    case SDLK_5:
    case SDLK_KP_5:
      k->pressed = TRUE;
      k->k_5 = TRUE;
      break;
    case SDLK_6:
    case SDLK_KP_6:
      k->pressed = TRUE;
      k->k_6 = TRUE;
      break;
    default:
      status = FALSE;
      break;
    }
    break;
  case SDL_KEYUP:
    k->pressed = FALSE;
    status = TRUE;
    switch (event.key.keysym.sym) {

    case SDLK_z:
      k->z = FALSE;
      break;
    case SDLK_PLUS:
    case SDLK_KP_PLUS:
      k->plus = FALSE;
      break;
    case SDLK_MINUS:
    case SDLK_KP_MINUS:
      k->minus = FALSE;
      break;
    case SDLK_PERIOD:
      k->period = FALSE;
      break;
    case SDLK_i:
      k->i = FALSE;
      break;
    case SDLK_u:
      k->u = FALSE;
      break;
    case SDLK_LESS:
      k->less = FALSE;
      break;
    case SDLK_RIGHT:
      k->right = FALSE;
      break;
    case SDLK_LEFT:
      k->left = FALSE;
      break;
    case SDLK_UP:
      k->up = FALSE;
      break;
    case SDLK_DOWN:
      k->down = FALSE;
      break;
    case SDLK_4:
    case SDLK_KP_4:
      k->k_4 = FALSE;
      break;
    case SDLK_5:
    case SDLK_KP_5:
      k->k_5 = FALSE;
      break;
    case SDLK_6:
    case SDLK_KP_6:
      k->k_6 = FALSE;
      break;
    case SDLK_7:
    case SDLK_KP_7:
      k->k_7 = FALSE;
      break;
    case SDLK_8:
    case SDLK_KP_8:
      k->k_8 = FALSE;
      break;
    case SDLK_9:
    case SDLK_KP_9:
      k->k_9 = FALSE;
      break;
    case SDLK_b:
      k->b = FALSE;
      break;
    case SDLK_c:
      k->c = FALSE;
      break;
    case SDLK_p:
      k->p = FALSE;
      break;
    case SDLK_t:
      k->t = FALSE;
      break;
    case SDLK_a:
      block_a = FALSE;
      break;
    case SDLK_n:
      k->n = FALSE;
      break;
    case SDLK_s:
      k->s = FALSE;
      k->S = FALSE;
      break;
    case SDLK_r:
      k->r = FALSE;
      break;
    case SDLK_f:
      k->f = FALSE;
      break;
    default:
      status = FALSE;
      break;
    }
    break;
  case SDL_MOUSEBUTTONDOWN:
    k->mclick = TRUE;
    k->mbdown = TRUE;
    k->mbup = FALSE;
    break;

  case SDL_MOUSEBUTTONUP:
    k->mbdown = FALSE;
    k->mbup = TRUE;

    status = TRUE;
    break;
  case SDL_QUIT:
    k->q = TRUE;
    status = TRUE;
    break;
  default:
    break;
  }
  return (status);
}

void Usage(char *ver, char *l_rev) {
  printf("Usage is: punto [-options] file \n");
  printf("possible options: h [s,size] n [g,geom,geometry] d "
                        "[r,radio] [c,color] w G p B D t z V bg fg lc F T\n");
  printf("Where file is an (x,[r,c]), (x,y,[r,c,fx,fy]) or a "
                        "(x,y,z,[r,c,fx,fy,fz]) file.\n");
  printf("\tr is the radio of the puntos.\n");
  printf("\tc is the color of the puntos.\n");
  printf("\tfx fy fz are the field values.\n");
  printf("Additional columns are discarded. You can choose the "
                        "columns to plot with the -z option.\n");
  printf("Each frame must be separated for a blank or a comment line.\n");
  printf("The comment lines must begin with a #.\n");
  printf("\nOptions:\n-h \t this help.\n");
  printf("-s N\t where N is max. size in pixels of puntos (an "
                        "integer number) (default: 15).\n");
  printf("-n N1:N2\twhere N1 and N2 are integers starting from 0.\n\t\tOnly the "
      "puntos with labels between these numbers are plotted.\n");
  printf("-g XRESOxYRESO\twidth and height of the window.\n");
  printf("-d cs\twhere cs is the delay time between frames in centiseconds.\n");
  printf("-r [min:max]\tthere is a column of radius.\n");
  printf("\t rescale between min and max values.\n");
  printf("-c \tthere is a column of colors.\n");
  printf("-e \"fname\" save pixmap and exit.\n");
  printf("-w Obsolote. as -G.\n");
  printf("-G [min:max] \tThe colors are a gradient scale. \n");
  printf("-p num \twhere num is the gradient palette. \n");
  printf(
                " \t 0 spectrum (default), 1 red, 2 green, 3 blue, 4 grey.\n");
  printf("-B [x0:y0:z0:x1:y1:z1]\tplot a border box.\n");
  printf("-D N\tthe dimension of the data is set to N.\n");
  printf("-t 0,1,2\ttype of the bitmap displayed.\n");
  printf(
                " \t 0 display circles, 1 squares, 2 spheres (default).\n");
  printf(
                "-z a:b:c:d:e.\tyou can indicate the columns to be plotted.\n");
  printf(
                " \t the x,y,z columns then the radio,color and field ones.\n");
  printf(
                "-trace [n]\ttrace mode on. n: number of trace dots.\n");
  printf("-V \tA vectorial field is plotted with arrows.\n");
  printf(
                "-bg color\tset the background color. (Default black).\n");
  printf(
                "-fg color\tset the foreground color. (Default blue).\n");
  printf(
                "-lc color\tset the led and box color. (Default green).\n");
  printf("-F \"fname\" \tset the default root filename for the "
                        "saved pixmaps.\n \t\t(default #temp_).\n");
  printf("-T \"name\" \tset the title of the window, (default: "
                        "the name of the data file).\n");

  printf("\nKeyboard controls:\n0\treset positions.\n4,7 \trotate the "
              "X-axis.\n5,8 \trotate the Y-axis.\n6,9 \trotate the Z-axis.\n");

  printf("z,Z\tzoom-in zoom-out.\n");
  printf("i,I\tincrease-decrease the size of puntos. (option -r "
                        "must be activated.)\n");
  printf("left,right arrow\tmove left-right.\n");
  printf("up,down arrow\t\tmove up-down.\n");
  printf("Keeping pressed the SHIFT or the < key the movement "
                        "is faster or slower.\n");
  printf("r\treverse animation.\n");
  printf("f\tfast mode. Don't show graphics.\n");
  printf(
                "a\tin vector mode changes among points, lines and arrows.\n");
  printf("u\tfixed radio on off.\n");
  printf("\t\tin vector mode normalized to unit.\n");
  printf("t\ttrace mode on off.\n");
  printf("p\tshow periodic images.\n");
  printf("S\tSave mode on off. All the images are saved.\n");
  printf("F\tSave only the current image.\n");

  printf("+(plus)\t\tincrease delay between frames.\n");
  printf("-(minus)\tdecrease delay between frames.\n");
  printf(".(period)\tsets delay time to the initial value.\n");
  printf("s   \tstops, starts animation.\n");
  printf("space   step by step animation.\n");
  printf("b\tborder box on off.\n");
  printf("c\tcomment line on-off.\n");
  printf("q   \tquit punto.\n");
  printf("To see all available colors, type punto -h color.\n");

  printf("\nCopyleft mrevenga 1998-2009 ");
  printf("%s", ver);
  printf("\nlast revision %s\n", l_rev);
  printf("Please, send bugs and suggestions to: mrevenga at "
                        "users.sourceforge.net\n");

}

int main(int argc, char *argv[]) {
  Uint32 background;
  long i;
  int done;
  SDL_Event event;
  Uint32 now;
  char title[MAX_WORD_LEN] = "";
  int pos_columns[7];

  int n_option_columns = 0;

  struct Window window;
  int statusblock;
  int inmouse = 0;
  Uint32 time, time0;
  long filesize;

  int sw_trace = 0;

  struct Block *b;
  struct PosCol pos;

  struct Universe universe;
  struct DataFile dfile;
  struct Parametres param;

  universe.ang.x = universe.ang.y = universe.ang.z = 0;
  universe.dshift.x = universe.dshift.y = universe.dshift.z = 0;
  universe.cm.x = universe.cm.y = universe.cm.z = 0;
  universe.cw.x = universe.cw.y = universe.cw.z = 0;
  universe.cv.x = universe.cv.y = universe.cv.z = 0;
  universe.escale.x = universe.escale.y = universe.escale.z = 0;
  universe.zoom = universe.zoom_size = 1;

  SetInitialValues(&option, &kp);

  switch (Arguments(argc, argv, &option, &param)) {
  case 1:
    Usage(version, last_revision);
    exit(EXIT_FAILURE);
    break;
  case 2:
    Usage(version, last_revision);
    if (argc > 2) {
      if (strncasecmp(argv[argc - 1], "color", 5) == 0) {
        InfoColors(rgbcolortable);
      }
    }
    exit(0);
    break;
  default:
    break;
  }

  GetFileName(argc, argv, dfile.name);
  if ((dfile.fp = fopen(dfile.name, "r")) == NULL) {
    perror(dfile.name);
    exit(EXIT_FAILURE);
  }
  dfile.size = SizeFile(dfile.name);

  strcpy(title, "");
  if (option.title == TRUE) {
    strncat(title, param.titlewindow, MAX_WORD_LEN);
  } else {
    strncat(title, dfile.name, MAX_WORD_LEN);
  }

  if (option.file == FALSE) {
    strncat(param.rootfilename, "temp_", 6);
  } else {
    strncat(param.rootfilename, "_", 2);
  }


  dfile.ncol = NColumns(dfile.name);

  N = CountLines(dfile.name);

  max_nsize = MAX_NSIZE;
  if (option.size)
    max_nsize = param.size;
  if (max_nsize > MAX_NSIZE)
    max_nsize = MAX_NSIZE;

  ntrace = TRACE_DEFAULT;
  if (option.ntrace) {
    ntrace = param.ntrace;
    if (ntrace >= TRACE_MAX)
      ntrace = TRACE_MAX;
    if (ntrace < 0)
      ntrace = TRACE_DEFAULT;
  }

  n_option_columns = 0;
  if (option.column == TRUE) {
    n_option_columns = GetColumns(param.colstr, dfile.ncol, pos_columns);
  }

  val_run.dimension = SetDimension(param, option, dfile.ncol, n_option_columns);
  if (val_run.dimension > 3 || val_run.dimension < 1) {
    fprintf(stderr, "ERROR. Wrong Dimension of the data\n");
    fprintf(stderr, "\tDimension calculated: %d\n", val_run.dimension);
    exit(EXIT_FAILURE);
  }

  if (option.type > 2 || option.type < 0) {
    option.type = SPHERE;
    fprintf(stderr, "Warning: incorrect option, setting type to sphere\n");
  }

  OrderOfColumns(option, val_run.dimension, dfile.ncol, n_option_columns,
                 pos_columns, &pos);


  max_N = N + 1000;

  if ((punto = calloc((size_t)max_N, sizeof(struct Punto))) ==
      NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  if ((tabla = calloc((size_t)max_N, sizeof(long))) == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }


  for (i = 0; i < max_N; i++)
    tabla[i] = i;

  dfile.nblocks = 0;
  if ((block = calloc((size_t)1, sizeof(struct Block))) ==
      NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }
  block[0].comment = nocomment;


  broot = CreateBlock();
  broot->pos = 0;
  broot->next = 0;
  broot->prev = 0;

  dfile.nblocks += ReadNBlocks(dfile.name, broot, 0);
  punto = ReadBlock(&nextblock, dfile, pos, &numactivados, punto, &statusblock);

  RangeOfValues(punto, option, &val_data);

  universe.cm.x = (val_data.max_x + val_data.min_x) / 2;
  universe.cm.y = (val_data.max_y + val_data.min_y) / 2;
  universe.cm.z = (val_data.max_z + val_data.min_z) / 2;
  universe.cv = universe.cm;


  SetValues(val_data, param, option, &val_run);
  if (option.real_color == TRUE) {
    float t;

    t = (val_run.max_c - val_run.min_c);
    if (t != 0)
      color_factor = (float)(MAX_NSCOLORS - 1) / t;
    else
      color_factor = 1;
  } else
    color_factor = 1;


  if (option.sizebox == TRUE) {
    if (GetBox(val_run.dimension, &param))
      option.sizebox = FALSE;
  }
  BoxProperties(val_data, param, option, box);
  if (option.verbose > 1) {
    printf("CM: %f %f %f\n", universe.cm.x, universe.cm.y, universe.cm.z);
    printf("Cv: %f %f %f\n", universe.cv.x, universe.cv.y, universe.cv.z);
  }
  numsprites = (max_nsize + 1) * MAX_NSCOLORS;
  if ((bola = malloc((size_t)(numsprites) *
                                      sizeof(struct Sprite))) == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < numsprites; i++) {
    bola[i].time = 0;
    bola[i].cont = 0;
    bola[i].on = FALSE;
    bola[i].surface = NULL;
  }

  if (option.size)
    universe.psize = param.size;
  else
    (universe.psize = DEFAULTRADIO);
  for (i = 0; i < numactivados; ++i) {
  }


  if (SDL_Init((Uint32)(SDL_INIT_VIDEO | SDL_INIT_TIMER)) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  atexit(SDL_Quit);

  window.flags = SDL_WINDOW_RESIZABLE;
  window.w = WIDTH;
  window.h = HEIGHT;
  if (option.geometria == TRUE) {
    window.w = param.width;
    window.h = param.height;
  }

  window.sdl_window =
      SDL_CreateWindow("punto", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, window.w, window.h,
                       window.flags);
  if (!window.sdl_window) {
    fprintf(stderr, "Couldn't create %dx%d window: %s\n", window.w, window.h,
            SDL_GetError());
    exit(EXIT_FAILURE);
  }
  window.renderer =
      SDL_CreateRenderer(window.sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (!window.renderer) {
    fprintf(stderr, "Couldn't create renderer: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  screen = SDL_CreateRGBSurface(0, window.w, window.h, 32, 0x00FF0000,
                                0x0000FF00, 0x000000FF, 0xFF000000);
  if (!screen) {
    fprintf(stderr, "Couldn't create framebuffer: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  window.texture =
      SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, window.w, window.h);
  window.screen = screen;

  defaultcolor = SDL_MapRGB(screen->format, 0, 0, 250);
  ledcolor = SDL_MapRGB(screen->format, 0, 255, 0);
  bgcolor = SDL_MapRGB(screen->format, 0, 0, 0);
  fgcolor = SDL_MapRGB(screen->format, 0, 0, 255);

  if (option.back == TRUE) {
    SDL_Color col;

    if (LookUpColor(param.backname, rgbcolortable, &col) != -1)
      bgcolor = SDL_MapRGB(screen->format, col.r, col.g, col.b);
    else {
      fprintf(stderr, "Warning: Unknown color %s\n", param.backname);
      fprintf(stderr, "\tUsing the default one.\n");
    }
  }

  if (option.fore == TRUE) {
    SDL_Color col;

    if (LookUpColor(param.forename, rgbcolortable, &col) != -1)
      fgcolor = SDL_MapRGB(screen->format, col.r, col.g, col.b);
    else {
      fprintf(stderr, "Warning: Unknown color %s\n", param.forename);
      fprintf(stderr, "\tUsing the default one.\n"); // HERE
    }
  }

  if (option.led == TRUE) {
    SDL_Color col;

    if (LookUpColor(param.ledname, rgbcolortable, &col) != -1)
      ledcolor = SDL_MapRGB(screen->format, col.r, col.g, col.b);
    else {
      fprintf(stderr, "Warning: Unknown color %s\n", param.ledname);
      fprintf(stderr, "\tUsing the default one.\n"); // HERE
    }
  }
  background = bgcolor;
  strncpy(nocomment, title, MAX_WORD_LEN);
  {
    SDL_Surface *icon = CreateBall3D(screen, fgcolor, 15);
    if (icon)
      SDL_SetWindowIcon(window.sdl_window, icon);
  }
  SDL_SetWindowTitle(window.sdl_window, title);

  if (option.verbose > 1) {
    printf("Screen is at %d bits per pixel\n",
           (int)screen->format->BitsPerPixel);
  }

  if (option.gradient || option.real_color) {
    if (option.palette)
      GenColorTable(screen, colortable, option.palette);
    else {
      GenColorTable(screen, colortable, DEFAULTPALETTE);
    }
  } else {
    for (i = 0; i < MAX_NSCOLORS; i++) {
      colortable[i] =
          SDL_MapRGB(screen->format, rgbcolortable[41 + i * 8].r,
                     rgbcolortable[41 + i * 8].g, rgbcolortable[41 + i * 8].b);
    }
  }

  {
    float dx = val_data.max_x - val_data.min_x;
    float dy = val_data.max_y - val_data.min_y;

    universe.escale.x = dx > 0 ? (float)window.w / dx : 1;
    universe.escale.y = dy > 0 ? (float)window.h / dy : 1;
  }
  if (option.sizebox == TRUE) {
    universe.escale.x = (float)window.w / (param.box.x1 - param.box.x0);
    universe.escale.y = (float)window.h / (param.box.y1 - param.box.y0);
  }

  if (universe.escale.x > universe.escale.y)
    universe.escale.x = universe.escale.y;
  universe.escale.y = universe.escale.x;

  if (option.verbose > 1)
    printf("escala_x: %g esc_y:%g zoom:%g \n", universe.escale.x,
           universe.escale.y, universe.zoom);

  universe.dshift.x =
      (val_data.max_x - val_data.min_x) * .5 * universe.escale.x;
  universe.dshift.y =
      (val_data.max_y - val_data.min_y) * .5 * universe.escale.x;
  window.shift.x = (float)window.w / 2 - universe.dshift.x;
  window.shift.y = (float)window.h / 2 - universe.dshift.y;

  universe.zoom = DEFAULTZOOM;

  if (option.radio == FALSE) {
    bola[0].surface =
        CreateBall3D(screen, defaultcolor,
                     (universe.psize + 1) * (universe.psize < MAX_NSIZE) +
                         MAX_NSIZE * (universe.psize > MAX_NSIZE));
    for (i = 1; i < numactivados; ++i) {
      punto[i].sprite = bola[0].surface;
    }
  }

  done = 0;


  for (i = 0; i < ntrace; i++) {
    if ((traceball[i] =
             calloc((size_t)1, sizeof(struct Punto))) == NULL) {
      perror("realloc");
      exit(EXIT_FAILURE);
    }
  }

  kEOF = FALSE;

  time = time0 = SDL_GetTicks();
  while (!done) {
    float ishift;
    float inc_ang2;
    int status;
    static int cont = 0;
    static float delay = 0;
    int md_x, md_y;

    inc_ang2 = INC_ANG;
    status = FALSE;
    if (!delay && option.anim == TRUE)
      delay = val_run.delay * 10;

    if (cont != 0) {

      if (((option.anim == TRUE && kEOF == FALSE) || kp.space == TRUE ||
           kp.pressed)) {

        do {
          status = CheckEvent(
              &event);
          time = SDL_GetTicks();
          if (time - time0 - delay + 10 > 0) {
            delay = 0;
            time0 = time;
          } else {
            if (delay && option.pausa == TRUE && option.anim == TRUE &&
                option.fast == FALSE)
              SDL_Delay(10);
          }
        } while (delay && status == FALSE);
      } else {
        if (kp.pressed == FALSE) {
          status = NextEvent(&event);
        }
      }
    }

    if (status == TRUE || kp.pressed) {
      actualizar = EventLoop(event, &window, &kp);
      if (kp.pressed) {
        actualizar = TRUE;
      }
      if (kp.q == TRUE)
        done = 1;
      if (kp.k_0 == TRUE) {
        universe.zoom = DEFAULTZOOM;
        universe.zoom_size = 1;
        window.shift.x = (float)window.w / 2 - universe.dshift.x;
        window.shift.y = (float)window.h / 2 - universe.dshift.y;
        universe.ang.x = universe.ang.y = universe.ang.z = 0;
        reset_box = TRUE;
        option.rotate = TRUE;
        actualizar = TRUE;
        kp.k_0 = FALSE;
      }
      if (kp.n == TRUE) {
        option.mode = (option.mode == ZOOM) ? NAV : ZOOM;
        kp.n++;
      }

      if (kp.u == TRUE) {
        option.varfield = (option.varfield == TRUE) ? FALSE : TRUE;
        kp.u++;
      }

      if (kp.S == TRUE) {
        option.save = (option.save == TRUE) ? FALSE : TRUE;
      }
      if (kp.s == TRUE) {
        option.anim = (option.anim == TRUE) ? FALSE : TRUE;
        kp.s++;
      }
      if (kp.r == TRUE) {
        option.reverse = (option.reverse == TRUE) ? FALSE : TRUE;
        kp.r++;
      }

      if (kp.period == TRUE) {
        val_run.delay = (float)param.pausa;
        kp.period = FALSE;
      }
      if (kp.plus == TRUE) {
        val_run.delay += 1 + (val_run.delay / 10);
        option.pausa = TRUE;
      }
      if (kp.minus == TRUE) {
        val_run.delay -= 1 + val_run.delay / 10;
        if (val_run.delay < 0)
          val_run.delay = 0;
        PurgeSprites(bola, 1);
      }

      if (kp.c == TRUE) {
        option.comment = (option.comment == TRUE) ? FALSE : TRUE;
        kp.c++;
      }

      if (kp.t == TRUE) {
        if (option.trace) {
          sw_trace = nextblock;
        }
        option.trace++;
        if (option.trace > 2)
          option.trace = 0;
        actualizar = TRUE;
        kp.t = FALSE;
      }
      if (kp.p == TRUE) {
        option.periodic = (option.periodic == TRUE) ? FALSE : TRUE;
        kp.p++;
      }

      if (kp.f == TRUE) {
        option.fast = (option.fast == TRUE) ? FALSE : TRUE;
        kp.f++;
      }
      if (kp.i == TRUE) {
        if (kp.shift)
          universe.zoom_size *= 1.05;
        else
          universe.zoom_size /= 1.05;
      }

      if (kp.user) {
        kp.user = FALSE;
        if (option.verbose > 1)
          printf("Checking file size\n");
        filesize = SizeFile(dfile.name);
        if (filesize != dfile.size) {
          printf("FILE SIZE CHANGED, new size: %ld\n", filesize);
          dfile.size = filesize;
          kEOF = FALSE;
          actualizar = TRUE;
          b = broot;
          while (b->next != 0)
            b = b->next;
          dfile.nblocks += ReadNBlocks(dfile.name, b, b->pos);
          printf("\n");
        }
      }

      if (option.mode == ZOOM) {
        if (kp.z == TRUE) {
          if (kp.shift)
            universe.zoom *= 1.05;
          else
            universe.zoom /= 1.05;
        }
      } else {
        double lx;
        double shift;

        lx = val_data.max_x - val_data.min_x;
        shift = -.01 * lx;
        if (kp.less)
          shift = -.001 * lx;
        if (kp.z == TRUE) {
          if (kp.shift) {
            universe.cv.z += shift;
            universe.zoom /= (1 + shift / universe.cv.z);
          } else {
            universe.cv.z -= shift;
            universe.zoom *= (1 + shift / universe.cv.z);
          }
        }
      }

      ishift = -.01 * window.w;
      if (kp.shift)
        ishift = -.10 * window.w;
      if (kp.less)
        ishift = -.001 * window.w;

      if (kp.right)
        window.shift.x += ishift / universe.zoom;
      if (kp.left)
        window.shift.x -= ishift / universe.zoom;
      if (kp.up)
        window.shift.y += ishift / universe.zoom;
      if (kp.down)
        window.shift.y -= ishift / universe.zoom;

      inc_ang2 = (1. + 5. * (kp.shift == TRUE)) * INC_ANG;
      if (kp.less)
        inc_ang2 /= 10;
      if (universe.zoom > 1)
        inc_ang2 /= universe.zoom;
      if (kp.k_7)
        universe.ang.x += inc_ang2;
      if (kp.k_8)
        universe.ang.y += inc_ang2;
      if (kp.k_9)
        universe.ang.z += inc_ang2;
      if (kp.k_4)
        universe.ang.x -= inc_ang2;
      if (kp.k_5)
        universe.ang.y -= inc_ang2;
      if (kp.k_6)
        universe.ang.z -= inc_ang2;

      if (universe.ang.x || universe.ang.y || universe.ang.z)
        option.rotate = TRUE;

    }

    if (option.trace == FALSE && sw_trace) {
      if (sw_trace - nextblock > 1 || nextblock - sw_trace > 1) {
        DrawTracePuntos(screen, window, universe, param, 0, 0);
        sw_trace = 0;
      }
    }

    {
      inmouse = 0;
      md_x = md_y = 0;
      if (kp.mbdown == TRUE) {
        (void)SDL_GetMouseState(&md_x, &md_y);
        kp.mbdown = FALSE;
      }
      if (kp.mbup == TRUE && kp.mclick == TRUE) {
        (void)SDL_GetMouseState(NULL, NULL);
      }
      if (md_x - filerect.x + 20 > 0 &&
          (filerect.x + filerect.l + 20) - md_x > 0) {
        if (md_y - filerect.y > 0 && (filerect.y + filerect.h) - md_y > 0) {
          if (option.box == TRUE) {
            inmouse = 1;
            kEOF = FALSE;
          }
        }
      }
      if (kp.mbup == TRUE)
        kp.mclick = FALSE;
    }

    if (option.reverse == TRUE)
      kEOF = FALSE;

    if (actualizar == TRUE) {
      if ((option.anim && delay == 0) || kp.space == TRUE) {
        if (option.reverse) {
          nextblock--;
          if (nextblock < 0) {
            nextblock = 0;
            option.anim = FALSE;
          }
        } else {
          if (kEOF == FALSE)
            nextblock++;
        }
      }
      if (kp.pageup == TRUE) {
        nextblock += 10;
        kp.pageup = FALSE;
        kEOF = FALSE;
      }
      if (kp.pagedown == TRUE) {
        nextblock -= 10;
        kp.pagedown = FALSE;
        if (nextblock < 0)
          nextblock = 0;
        kEOF = FALSE;
      }
    }

    if (kEOF == FALSE) {

      statusblock = FALSE;

      if (inmouse)
        nextblock =
            NextBlock((float)(md_x - filerect.x) / filerect.l, dfile.name);
      md_x = 0;
      punto =
          ReadBlock(&nextblock, dfile, pos, &numactivados, punto, &statusblock);
    }

    MovePuntos(universe, param, punto);
    universe.ang.x = universe.ang.y = universe.ang.z = 0;
    DrawAll(screen, window, universe, param, background, dfile);

    if (kp.F == TRUE || option.save == TRUE) {
      WriteBitmap(screen, param.rootfilename);
      kp.F = FALSE;
    }

    if (option.exit == TRUE) {
      done = TRUE;
      if (SDL_SaveBMP(screen, param.path) == -1) {
        fprintf(stderr, "Error saving bitmap\n");
        exit(2);
      }
    }

    if (actualizar == TRUE) {
      PurgeSprites(bola, 0);
      InfoSprites(bola, option);
    }

    actualizar = FALSE;
    option.rotate = FALSE;

    if (kEOF == FALSE) {
      actualizar = TRUE;
      option.rotate = TRUE;
    }

    if (kp.space == TRUE) {
      kp.space = FALSE;
    }

    Cont++;
    cont++;
  }
  for (i = 0; i < numsprites; i++) {
    if (bola[i].on == TRUE) {
      SDL_FreeSurface(bola[i].surface);
    }
  }
  now = SDL_GetTicks();
  SDL_FreeSurface(screen);
  SDL_DestroyTexture(window.texture);
  SDL_DestroyRenderer(window.renderer);
  SDL_DestroyWindow(window.sdl_window);
  SDL_Quit();
  return (0);
}

void DrawInfo(SDL_Surface *screen, struct Window win, struct Universe u,
              struct DataFile df) {
  int x;
  float y;
  int fs = FONTSIZE;

  int fh = (fs + 1) * 2 + 3;
  char num[MAX_WORD_LEN] = "";

  y = u.escale.x * u.zoom;
  if (option.box)
    DrawBox(win, box, val_run.dimension, u.cw, y, ledcolor);

  if (option.box == 2)
    return;

  x = DrawText(screen, "Zoom:", 5, 3, ledcolor);
  snprintf(num, MAX_WORD_LEN, "%.2g", y);

  x = DrawText(screen, num, x, 3, ledcolor);

  filerect.x = x + 20;
  filerect.y = 1;
  filerect.l = 200;
  filerect.h = 12;

  if (df.nblocks > 1)
    DrawFile(screen, ledcolor, ledcolor, filerect, df);

  x = DrawText(screen, "Num:", 5, fh + 6, ledcolor);
  snprintf(num, MAX_WORD_LEN, "%ld", numactivados);
  x = DrawText(screen, num, x, fh + 6, ledcolor);
  x = DrawText(screen, "Block:", 5, fh * 2 + 9, ledcolor);
  snprintf(num, MAX_WORD_LEN, "%d", nextblock + 1);

  x = DrawText(screen, num, x, fh * 2 + 9, ledcolor);

  if (option.fast == FALSE) {
    if (option.real_color)
      DrawScale(screen, colortable, ledcolor, val_run, 10, 60);
  }
}

void MovePuntos(struct Universe u, struct Parametres par, struct Punto *p) {
  long n1, n2;
  static int cont;

  if (cont == 0) {
    long i;

    for (i = 0; i < numactivados; i++)
      tabla[i] = i;
    cont++;
  }

  n1 = 0;
  n2 = numactivados;
  if (option.number == TRUE) {
    n1 = par.N1;
    n2 = par.N2;
    if (n1 < 0)
      n1 = 0;
    if (n2 > numactivados)
      n2 = numactivados;
  }

  if ((actualizar == TRUE || option.rotate == TRUE)) {
    struct Point c;

    c = u.cm;
    if (option.mode == NAV)
      c = u.cv;
    TCR(u.ang.x, u.ang.y, u.ang.z, n1, n2, c, 1, option.field, &reset_box, p);
    TCR(0, 0, 0, 0, 8, c, 1, FALSE, &reset_box, box);
  }

  if (actualizar == TRUE) {
    if (option.fast == FALSE && option.rotate == TRUE) {
      if (par.size || u.psize) {
        int i;

        for (i = 0; i < numactivados; i++)
          tabla[i] = i;
        Rapida(tabla, n1, n2 - 1, p);
      }
    }
  }
}

void DrawAll(SDL_Surface *screen, struct Window win, struct Universe u,
             struct Parametres par, Uint32 background, struct DataFile df) {
  SDL_Rect led;

  led.x = 0;
  led.y = 0;
  led.w = LEDW;
  led.h = LEDH;

  if (actualizar == TRUE) {


    if ((option.fast == FALSE || actualizar == TRUE)) {
      SDL_FillRect(screen, NULL, background);
    } else {
      SDL_FillRect(screen, &led, background);
    }
    {
      double x, y;

      x = u.dshift.x + win.shift.x;
      y = u.dshift.y + win.shift.y;
      if (option.mode == NAV) {
        x += u.cv.x;
        y += u.cv.y;
      }
      u.cw.x = (x)*u.zoom + (double)win.w / 2. * (1. - u.zoom);
      u.cw.y = (y)*u.zoom + (double)win.h / 2. * (1. - u.zoom);
    }
    if (option.fast == FALSE) {
      DrawPuntos(screen, win, u, punto, tabla, u.cw, u.escale.x * u.zoom, par,
                 val_run);

      if (option.periodic == TRUE) {
        DrawPeriodicPuntos(screen, win, u, punto, tabla, u.cw,
                           u.escale.x * u.zoom, par, val_run);
      }

      if (option.trace) {
        DrawTracePuntos(screen, win, u, par, u.escale.x * u.zoom, 1);
      }
    }
    if (kp.b == TRUE) { // HERE this must not be here
      option.box++;
      if (option.box > 2)
        option.box = 0;
      kp.b = FALSE;
    }

    if (option.box) {
      DrawInfo(screen, win, u, df);
    }
    if (option.comment == TRUE)
      DrawText(screen, comment, 5, win.h - 20, ledcolor);

    if (option.verbose > 1)
      printf("sprites active: %d\n", CountSprites(bola, numsprites));

    SDL_UpdateTexture(win.texture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(win.renderer);
    SDL_RenderCopy(win.renderer, win.texture, NULL, NULL);
    SDL_RenderPresent(win.renderer);
  }
}

void DrawPuntos(SDL_Surface *screen, struct Window w, struct Universe u,
                struct Punto *p, long *t, struct Point cwin, float z,
                struct Parametres par, struct Values data) {
  int i;
  SDL_Rect position;
  Uint32 time;
  float x, y;
  int max;
  int coloridx;
  int idx;
  int sizeidx;
  long n1, n2;
  float factor;
  float radio;
  float color;

  time = SDL_GetTicks();

  max = (screen->w > screen->h) ? screen->w : screen->h;
  max += max_nsize;
  factor = .5 * u.psize * u.zoom * u.zoom_size;
  sizeidx = (int)(u.psize * u.zoom_size);
  n1 = 0;
  n2 = numactivados;
  if (option.number == TRUE) {
    n1 = par.N1;
    n2 = par.N2;
    if (n1 < 0)
      n1 = 0;
    if (n2 > numactivados)
      n2 = numactivados;
  }

  if (option.field) {
    if (SDL_MUSTLOCK(screen)) {
      if (SDL_LockSurface(screen) < 0) {
        printf("Error locking surface: %s\n", SDL_GetError());
        abort();
      }
    }
  }
  for (i = 0; i < numactivados; i++) {
    if (t[i] < 0 || t[i] > max_N + 1) {
      printf("FATAL ERROR: seg fault. exiting...\n\t  KK 10: %d t_i %ld max_N: "
             "%ld\n",
             i, t[i], max_N);
      exit(EXIT_FAILURE);
    }

    p[t[i]].sprite = NULL;
    if (t[i] < n1 || t[i] >= n2)
      continue;
    x = (float)(p[t[i]].x1 * z + cwin.x);
    y = (float)(w.h - ((p[t[i]].y1) * z + cwin.y));

    if ((int)x > max || ((int)y > max) || ((int)x < -max_nsize) ||
        ((int)y < -max_nsize)) {
      continue;
    }
    if (option.mode == NAV)
      if (p[t[i]].z1 > 0)
        continue;
    coloridx = 0;

    if (option.color) {
      color = (p[t[i]].color);
      coloridx = (int)color % MAX_NSCOLORS;

      if (option.real_color == TRUE) {
        coloridx = (int)((color - data.min_c) * color_factor);
        if (coloridx > MAX_NSCOLORS - 1)
          coloridx = MAX_NSCOLORS - 1;
      }
      if (coloridx < 0)
        coloridx = 0;
    }

    if (option.radio == TRUE && option.varfield == TRUE) {
      sizeidx = (int)(z * u.zoom_size);
      radio = p[t[i]].radio;
      if (radio > 0) {
        if (val_run.max_r > 0) {
          if (radio > val_run.max_r)
            radio = val_run.max_r;
        }
        sizeidx = (int)(radio * z * u.zoom_size);
      }
    }
    if (sizeidx > max_nsize)
      sizeidx = max_nsize;
    if (sizeidx < 1)
      sizeidx = 0;

    if (option.color) {
      fgcolor = colortable[coloridx];
    }

    idx = coloridx + MAX_NSCOLORS * sizeidx;

    bola[idx].cont = Cont;
    bola[idx].time = time;

    if (option.field == FALSE) {
      if (bola[idx].on == FALSE) {
        if (option.verbose > 1) {
          printf(" (%d) %d+%d=%d ", idx, sizeidx, coloridx, idx);
          fflush(stdout);
        }

        if (bola[idx].surface != NULL)
          SDL_FreeSurface(bola[idx].surface);
        bola[idx].surface = NULL;
        switch (option.type) {
        case CIRCLE:
          bola[idx].surface =
              CreateBall(screen, fgcolor, defaultcolor, sizeidx);
          break;
        case SQUARE:
          bola[idx].surface =
              CreateRectangle(screen, fgcolor, fgcolor, 2 * sizeidx + 1);
          break;
        case SPHERE:
          bola[idx].surface = CreateBall3D(screen, fgcolor, sizeidx);
          break;
        default:
          bola[idx].surface =
              CreateBall(screen, defaultcolor, fgcolor, sizeidx);

          break;
        }
        bola[idx].on = TRUE;
      }

      p[t[i]].sprite = bola[idx].surface;

      position.x = (Sint16)((int)x - sizeidx);
      position.y = (Sint16)((int)y - sizeidx);

      SDL_BlitSurface(p[t[i]].sprite, NULL, screen, &position);
    } else {
      struct Point f;
      float norm;

      position.x = (Sint16)x;
      position.y = (Sint16)y;
      f.x = p[t[i]].vx1;
      f.y = p[t[i]].vy1;
      if (option.varfield == TRUE) {
        norm = val_run.max_f;
      } else {
        norm = sqrt(f.x * f.x + f.y * f.y);
      }
      if (norm == 0)
        norm = 1;
      DrawArrow(screen, position, f, factor / norm, fgcolor, kp.arrow);
    }
  }

  if (option.verbose > 1)

  if (option.field) {
    SDL_UnlockSurface(screen);
  }
}

void DrawTracePuntos(SDL_Surface *screen, struct Window w, struct Universe u,
                     struct Parametres par, float z, int order) {
  long int i, n1, n2;
  int j, k, l;
  float lenx, leny, lenz;
  static int block;
  static int trace_num = 0;
  static int trace_pos = 0;
  static int trace_cont = 1;

  if (order == 0) {
    trace_pos = 0;
    trace_cont = 1;
    trace_num = 1;
    if (option.verbose > 0)
      printf("FREEING MEM (trace points)\n");
    for (i = 0; i < ntrace; i++) {
      if ((traceball[i] = realloc(
               traceball[i], (size_t)trace_num * sizeof(struct Punto))) ==
          NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
      }
    }
    return;
  }

  lenx = (val_data.max_x - val_data.min_x) / 2;
  leny = (val_data.max_y - val_data.min_y) / 2;
  lenz = (val_data.max_z - val_data.min_z) / 2;

  n1 = 0;
  n2 = numactivados;
  if (option.number == TRUE) {
    n1 = par.N1;
    n2 = par.N2;
    if (n1 < 0)
      n1 = 0;
    if (n2 > numactivados)
      n2 = numactivados;
  }

  if (nextblock != block) {
    if (nextblock - block == 1 || block - nextblock == 1) {
      trace_pos++;
      if (trace_pos >= ntrace)
        trace_pos = 0;
      trace_cont++;
      if (trace_cont >= ntrace)
        trace_cont = ntrace;
    } else {
      DrawTracePuntos(screen, w, u, par, z, 0);
    }

    block = nextblock;
  }


  if (numactivados > trace_num) {

    if (option.verbose > 1)
      printf("REALLOCATING\n");
    for (i = 0; i < ntrace; i++) {
      if ((traceball[i] = realloc(
               traceball[i], (size_t)numactivados * sizeof(struct Punto))) ==
          NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
      }
      for (j = trace_num; j < numactivados; j++) {
        traceball[i][j].z = 0;
        traceball[i][j].y = 0;
      }
    }
    trace_num = numactivados;
  }

  for (i = n1; i < n2; i++) {
    traceball[trace_pos][i].x = punto[i].x;
  }
  if (val_run.dimension > 1) {
    for (i = n1; i < n2; i++) {
      traceball[trace_pos][i].y = punto[i].y;
    }
  }
  if (val_run.dimension == 3) {
    for (i = n1; i < n2; i++) {
      traceball[trace_pos][i].z = punto[i].z;
    }
  }

  if (actualizar == TRUE || option.rotate == TRUE) {
    for (i = 0; i < trace_cont; i++) {
      TCR(0, 0, 0, n1, n2, u.cm, 1, option.field, &reset_box, traceball[i]);
    }
  }
  for (i = n1; i < n2; i++) {
    for (j = 0; j < trace_cont; j++) {
      k = trace_pos - j;
      if (k < 0)
        k += ntrace;

      if (option.trace == 1)
        PutPixel(screen, (int)(traceball[k][i].x1 * z + u.cw.x),
                 (int)(w.h - (traceball[k][i].y1 * z + u.cw.y)), ledcolor);
      else {
        if (j < trace_cont - 1) {
          l = trace_pos - j - 1;
          if (l < 0)
            l += ntrace;
          if (fabs(traceball[l][i].x - traceball[k][i].x) < lenx) {
            if (fabs(traceball[l][i].y - traceball[k][i].y) < leny) {
              DrawLine(screen, (int)(traceball[l][i].x1 * z + u.cw.x),
                       (int)(w.h - (traceball[l][i].y1 * z + u.cw.y)),
                       (int)(traceball[k][i].x1 * z + u.cw.x),
                       (int)(w.h - (traceball[k][i].y1 * z + u.cw.y)),
                       ledcolor);
            }
          }
        }
      }
    }
  }

  return;
}

int DrawPeriodicPuntos(SDL_Surface *screen, struct Window w, struct Universe u,
                       struct Punto *p, long *t, struct Point cw, float z,
                       struct Parametres par, struct Values data) {
  int i, j, k, l;
  static struct Punto *p0 = NULL;
  static long p0_size = 0;
  float ix, iy, iz;
  long n1, n2;
  struct Point c;

  c = u.cm;
  if (numactivados > p0_size) {
    free(p0);
    if ((p0 = malloc((size_t)numactivados *
                                     sizeof(struct Punto))) == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }
    p0_size = numactivados;
  }

  ix = val_data.max_x - val_data.min_x;
  iy = val_data.max_y - val_data.min_y;
  iz = val_data.max_z - val_data.min_z;
  n1 = 0;
  n2 = numactivados;

  if (option.color == TRUE) {
    for (i = 0; i < numactivados; i++) {
      p0[i].color = punto[i].color;
    }
  }
  if (option.radio == TRUE) {
    for (i = 0; i < numactivados; i++) {
      p0[i].radio = punto[i].radio;
    }
  }
  if (option.field == TRUE) {
    for (i = 0; i < numactivados; i++) {
      p0[i].vx = punto[i].vx;
      p0[i].vy = punto[i].vy;
      p0[i].vz = punto[i].vz;
    }
  }

  for (l = -1; l < 2; l++) {
    if (val_run.dimension == 2)
      l = 0;
    for (j = -1; j < 2; j++) {
      for (k = -1; k < 2; k++) {
        if (k == 0 && j == 0 && l == 0)
          continue;
        for (i = 0; i < numactivados; i++) {
          p0[i].x = punto[i].x + ix * j;
          p0[i].y = punto[i].y + iy * k;
          p0[i].z = punto[i].z + iz * l;
        }

        TCR(0, 0, 0, n1, n2, c, 1, option.field, &reset_box, p0);
        DrawPuntos(screen, w, u, p0, t, cw, z, par, data);
      }
    }
    if (val_run.dimension == 2)
      break;
  }

  return (0);
}

void RangeOfValues(struct Punto *p, struct Options opt, struct Values *data) {
  int i;

  data->max_x = data->min_x = p[0].x;
  data->max_y = data->min_y = p[0].y;
  data->max_z = data->min_z = p[0].z;
  if (opt.field) {
    data->max_f = data->min_f = p[0].vx;
  }
  if (opt.radio) {
    data->min_r = data->max_r = p[0].radio;
  }
  if (opt.color) {
    data->max_c = data->min_c = p[0].color;
  }

  for (i = 0; i < numactivados; i++) {
    if (opt.radio) {
      if (p[i].radio > data->max_r)
        data->max_r = p[i].radio;
      if (p[i].radio < data->min_r)
        data->min_r = p[i].radio;
    }
    if (opt.color) {
      if (p[i].color > data->max_c)
        data->max_c = p[i].color;
      if (p[i].color < data->min_c)
        data->min_c = p[i].color;
    }
    if (opt.field == TRUE) {
      if (p[i].vx > data->max_f)
        data->max_f = p[i].vx;
      if (p[i].vy > data->max_f)
        data->max_f = p[i].vy;
      if (p[i].vz > data->max_f)
        data->max_f = p[i].vz;

      if (p[i].vx < data->min_f)
        data->min_f = p[i].vx;
      if (p[i].vy < data->min_f)
        data->min_f = p[i].vy;
      if (p[i].vz < data->min_f)
        data->min_f = p[i].vz;
    }
    if (p[i].x > data->max_x)
      data->max_x = p[i].x;
    if (p[i].x < data->min_x)
      data->min_x = p[i].x;
    if (p[i].y > data->max_y)
      data->max_y = p[i].y;
    if (p[i].y < data->min_y)
      data->min_y = p[i].y;
    if (p[i].z > data->max_z)
      data->max_z = p[i].z;
    if (p[i].z < data->min_z)
      data->min_z = p[i].z;
  }
}

int CountSprites(struct Sprite *b, int nmax) {
  int i;
  int cont = 0;

  for (i = 0; i < nmax; i++)
    if (b[i].on == TRUE)
      cont++;
  return cont;
}

void InfoSprites(struct Sprite *b, struct Options opt) {
  int i;
  long mem;
  long cont;
  uintptr_t maxpos, minpos;
  static int lastpurge;
  int sw;

  mem = 0;
  cont = 0;
  sw = 0;
  maxpos = 0;
  minpos = 0;
  for (i = 0; i < numsprites; i++) {
    if (b[i].on == TRUE) {
      cont++;
      mem += 100 + 3 * b[i].surface->w * b[i].surface->h;

      if (sw == 0) {
        minpos = (uintptr_t)b[i].surface;
        sw = 1;
      }
      if ((uintptr_t)b[i].surface < minpos)
        minpos = (uintptr_t)b[i].surface;
      if ((uintptr_t)b[i].surface > maxpos) {
        maxpos = (uintptr_t)b[i].surface;
      }
    }
  }
  if (mem) {
    if (option.verbose) {
      printf("num spr: %ld (%ld) mem: %ld\n", cont, numactivados, mem);
      printf("MAXPOS: %lu MINPOS: %lu dif: %lu\n", (unsigned long)maxpos,
             (unsigned long)minpos, (unsigned long)(maxpos - minpos));
      printf("factor: %f\n", ((float)(maxpos - minpos)) / (float)mem);
    }
    if (option.trace)
      mem += numactivados * ntrace * sizeof(struct Punto);
    if ((((float)(maxpos - minpos) / mem > 2 && maxpos - minpos > 16000000) ||
         (cont > numactivados * 2 && cont > 256)) &&
        lastpurge > 16) {
      PurgeSprites(bola, 1);
      if (option.verbose) {
        printf("PURGANDO %ld %d\n", cont, lastpurge);
      }
      lastpurge = 0;
    } else
      lastpurge++;
  }
}

void PurgeSprites(struct Sprite *b, int type) {
  int i;
  int cont1, cont2, cont3;
  static int init;
  int continit;
  Uint32 time, mintime;

  time = SDL_GetTicks();
  mintime = time - 2000;
  if (time < 2000)
    mintime = 0;
  cont1 = cont2 = cont3 = 0;
  switch (type) {
  case 0:
    for (i = 0; i < numsprites; i++) {
      if (b[i].on)
        cont3++;

      if (b[i].on == TRUE &&
          (b[i].cont < (Cont - 20)) /*        && b[i].time < mintime */) {
        b[i].on = FALSE;
        b[i].cont = 0;
        b[i].time = 0;
        SDL_FreeSurface(b[i].surface);
        b[i].surface = NULL;
        if (option.verbose > 1)
          printf(" (%d)<%d+%d=%d ", i, i % max_nsize, (int)(i / max_nsize), i);
        cont1++;
      } else {
        cont2++;
      }
    }
    break;

  case 1:
    for (i = 0; i < numsprites; i++) {
      if (b[i].on == TRUE) {
        b[i].on = FALSE;
        b[i].cont = 0;
        b[i].time = 0;
        SDL_FreeSurface(b[i].surface);
        b[i].surface = NULL;
        if (option.verbose > 1)
          printf(" <(%d) %d+%d ", i, i % max_nsize, (int)(i / max_nsize));
      }
    }
    break;

  case 2:
    for (i = 0; i < numsprites; i++) {
      if (b[i].on == TRUE) {
        if (!(rand() % 40)) {
          b[i].on = FALSE;
          b[i].cont = 0;
          b[i].time = 0;
          SDL_FreeSurface(b[i].surface);
          b[i].surface = NULL;
          if (option.verbose > 1)
            printf(" (%d)<%d+%d=%d ", i, i % max_nsize, (int)(i / max_nsize),
                   i);
        }
      }
    }
    break;

  case 3:
    continit = 0;
    for (i = init; i < numsprites && continit < 10; i++) {
      if (b[i].on == TRUE) {

        b[i].on = FALSE;
        b[i].cont = 0;
        b[i].time = 0;
        SDL_FreeSurface(b[i].surface);
        b[i].surface = NULL;
        if (option.verbose > 1)
          printf(" (%d)<%d+%d=%d ", i, i % max_nsize, (int)(i / max_nsize), i);
        continit++;
      }
    }
    init = i;
    if (i > numsprites - 1)
      init = 0;
    break;

  default:
    for (i = 0; i < max_nsize * MAX_NSCOLORS; i++) {
      if (b[i].on && (b[i].cont < Cont - 200 || b[i].time < mintime)) {
        b[i].on = FALSE;
        b[i].cont = 0;

        SDL_FreeSurface(b[i].surface);
        b[i].surface = NULL;
        if (option.verbose > 1)
          printf(" <%d+%d ", i % max_nsize, (int)(i / max_nsize));
      }
    }
    break;
  }
}

struct Punto *ReadBlock(int *next, struct DataFile df, struct PosCol pos,
                        long *numact, struct Punto *p, int *status) {
  size_t i, j, k;
  double *temp;
  long cont = 0;
  int reallocpunto = FALSE;
  static int lastblock = -1;
  static int maxblock = 0;

  struct Block *b;
  char line[MAX_LINE_LEN];
  long rbytes;
  int typeline;
  size_t lenline;
  int sw;
  int rvalue;
  int linecont = 0;


  if (*next == lastblock) {
    *status = FALSE;
    return (p);
  }

  *status = TRUE;

  b = broot;
  cont = 0;

  while (cont < *next) {
    if (b->next != 0) {
      b = b->next;
      cont++;
    } else {
      kEOF = TRUE;
      *next = cont;
      maxblock = lastblock;
      return (p);
    }
  }

  if ((temp = malloc((size_t)(df.ncol + 1) * sizeof(double))) ==
      NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for (i = 0; (int)i < df.ncol + 1; i++)
    temp[i] = 0;

  comment = nocomment;


  if ((*next - lastblock != 1 || maxblock <= lastblock)) {
    if (fseek(df.fp, b->pos, SEEK_SET) == -1)
      printf("ERROR Seeking file\n");
  }

  sw = 0;
  do {
    rbytes = RLine(line, MAX_LINE_LEN, df.fp);
    linecont++;

    typeline = AnalizeLine(line);
    if (typeline == COMMENT && sw == 0) {
      strncpy(comentario, line, MAX_WORD_LEN);
      comment = comentario;
      sw = 1;
    }
  } while ((typeline == COMMENT || typeline == RETURN) && rbytes != EOF);
  i = 0;
  sw = 0;

  do {
    if (sw) {
      rbytes = RLine(line, MAX_LINE_LEN, df.fp);
      linecont++;

      typeline = AnalizeLine(line);
    }
    sw = 1;
    if (rbytes == EOF)
      kEOF = TRUE;
    if (typeline == DATA && kEOF == FALSE) {
      k = 0;
      lenline = strlen(line);
      for (j = 0; j < (size_t)df.ncol; j++) {
        rvalue = sscanf(&line[k], "%lf", &temp[j]);
        if (rvalue == EOF) {
          kEOF = TRUE;
          printf("ERROR-readblock-fscanf\n");
          exit(EXIT_FAILURE);
        }
        while (line[k] != ' ' && line[k] != '\t' && k < lenline)
          k++;
        k++;
      }
      p[i].x = temp[pos.x];
      p[i].y = temp[pos.y];
      p[i].z = temp[pos.z];
      p[i].radio = temp[pos.r];
      p[i].color = temp[pos.c];
      if (option.field == TRUE) {
        p[i].vx = temp[pos.vx];
        p[i].vy = temp[pos.vy];
        p[i].vz = temp[pos.vz];
      }
      i++;
      if (i >= (size_t)max_N) {
        if ((p = realloc(
                 p, (size_t)(i + 1024) * sizeof(struct Punto))) == NULL) {
          perror("realloc");
          exit(EXIT_FAILURE);
        }
        max_N = i + 1023;
        reallocpunto = TRUE;
      }
    }
  } while (typeline == DATA && kEOF == FALSE);

  if (reallocpunto) {
    if ((tabla = realloc(tabla, (size_t)(max_N + 1) * sizeof(long))) ==
        NULL) {
      perror("realloc");
      exit(EXIT_FAILURE);
    }
    for (i = *numact; i < (size_t)max_N + 1; i++)
      tabla[i] = i;
  }

  free(temp);
  lastblock = *next;
  if (kEOF == TRUE) {
    if (!maxblock)
      maxblock = lastblock;
  }

  *numact = b->num;
  return (p);
}

int NextBlock(float x, char *fn) {

  long fsize;
  int cont = 0;
  struct Block *b;

  if (x < 0)
    return (0);
  b = broot;
  fsize = SizeFile(fn) * x;

  while (b->pos < fsize) {
    if (b->next != 0) {
      cont++;
      b = b->next;
    } else {
      if (cont < 0)
        return (0);
      return (cont);
    }
  }
  if (cont - 1 < 0)
    return (0);
  return (cont - 1);
}


int Arguments(int argc, char *argv[], struct Options *opt,
              struct Parametres *par)
{
  int i;
  char c;
  int sw = 0;
  char *pointer;
  char *endptr = NULL;
  static char str[MAX_WORD_LEN] = "",
              str0[MAX_WORD_LEN] = "";
  int flag = 0;
  int narg;
  char arg[MAX_WORD_LEN] = "";
  char pal[MAX_WORD_LEN] = "";

  char *validargum[] = {"h",        "s",       "size", "n",     "g",     "geom",
                        "geometry", "d",       "e",    "r",     "radio", "c",
                        "color",    "w",       "G",    "p",     "B",     "D",
                        "t",        "z",       "V",    "bg",    "fg",    "F",
                        "lc",       "verbose", "T",    "trace", "P"};
  int NUMARGS = 28;

  if (argc < 2) {
    return (1);
  }

  strncpy(par->colstr, "\0", (size_t)1);
  strncpy(par->boxstr, "\0", (size_t)1);

  strncpy(par->backname, "\0", (size_t)1);
  strncpy(par->forename, "\0", (size_t)1);
  strncpy(par->ledname, "\0", (size_t)1);
  strncpy(par->rootfilename, "\0", (size_t)1);
  strncpy(par->titlewindow, "\0", (size_t)1);

  par->size = 0;
  par->pausa = 0;
  par->N = 0;
  par->N1 = 0;
  par->N2 = 0;
  par->ntrace = 0;
  par->dimension = 0;
  par->width = 0;
  par->height = 0;
  par->max_r = 0;
  par->min_r = 0;
  par->max_c = 0;
  par->min_c = 0;
  par->max_f = 0;
  par->min_f = 0;
  par->zoomz = 0;

  for (i = 0; i < argc; i++) {
    if (*argv[i] == '-') {

      strncpy(arg, &argv[i][1], MAX_WORD_LEN);
      strncpy(&arg[MAX_WORD_LEN - 1], "\0", (size_t)1);
      flag = 0;
      c = '0';
      if (i + 1 < argc) {
        c = argv[i + 1][0];
        if (c != '-') {
          strncpy(pal, argv[i + 1], MAX_WORD_LEN);
          strncpy(&pal[MAX_WORD_LEN - 1], "\0", (size_t)1);
        } else
          strncpy(pal, "\0", (size_t)1);
      }
      if (c == ' ' || c == '\0')
        break;
      if (flag)
        break;

      if ((narg = FindChain(arg, validargum, NUMARGS)) < 0) {
        printf("\ninvalid opt -- %s\n", arg);
        printf("\ntry 'punto -h' for help\n");
        exit(EXIT_FAILURE);
      }
      c = arg[0];
      switch (c) {
      case 'z':
        if (strncmp("z", arg, MAX_WORD_LEN) == 0) { // choose columns
          opt->column = TRUE;
          strncpy(par->colstr, argv[i + 1], MAX_WORD_LEN);
          strncpy(&par->colstr[MAX_WORD_LEN - 1], "\0", (size_t)1);
        }
        break;
      case 't':
        if (strncmp("t", arg, MAX_WORD_LEN) == 0) { // type of punto
          opt->type = strtol(argv[i + 1], &endptr, 10);
          if (opt->type == 0 && argv[i + 1] == endptr) {
            opt->type = DEFAULTTYPE;
            fprintf(
                stderr,
                "Warning: -t option must have an argument. Setting default.\n");
          }
        }

        if (strncmp("trace", arg, MAX_WORD_LEN) == 0) { // trace points on
          opt->trace = TRUE;
          par->ntrace = (int)strtol(argv[i + 1], NULL, 10);
          if (par->ntrace > 0 && par->ntrace <= TRACE_MAX)
            opt->ntrace = TRUE;
        }
        break;
      case 's':
        if (strncmp("s", arg, MAX_WORD_LEN) == 0 ||
            strncmp("size", arg, MAX_WORD_LEN) == 0) { // size of puntos
          opt->size = TRUE;
          par->size = strtol(argv[i + 1], &endptr, 10);
          if (par->size == 0 && argv[i + 1] == endptr) {
            opt->size = FALSE;
            fprintf(
                stderr,
                "Warning: -s option must have an argument. Setting default.\n");
          }
        }
        break;
      case 'h':
        if (strncmp("h", arg, MAX_WORD_LEN) == 0) { // show help
          return (2);
        }
        break;
      case 'n':
        sw = 0;
        if (strncmp("n", arg, MAX_WORD_LEN) == 0) { // selected puntos
          strncpy(str, str0, MAX_WORD_LEN);
          strncpy(&str[MAX_WORD_LEN - 1], "\0", (size_t)1);
          pointer = strchr(argv[i + 1], ':');
          if (pointer) {
            strncpy(str, argv[i + 1], strlen(argv[i + 1]) - strlen(pointer));
            par->N1 = strtol(str, &endptr, 10);
            if (par->N1 == 0 && str == endptr) {
              sw++;
            }
            par->N2 = strtol(pointer + 1, &endptr, 10);
            if (par->N2 == 0 && (pointer + 1) == endptr) {
              sw++;
            }
            i++;
          } else {
            sw++;
          }
          if (sw == 0) {
            if (par->N1 >= par->N2)
              sw++;
          }
          if (sw == 0) {
            opt->number = TRUE;
          } else {
            fprintf(stderr,
                    "Warning: -n option bad formed. Plotting all puntos.\n");
          }
        }
        break;
      case 'g':
        sw = 0;
        if (strncmp("g", arg, MAX_WORD_LEN) == 0 || // geometry of the window
            strncmp("geom", arg, MAX_WORD_LEN) == 0 ||
            strncmp("geometry", arg, MAX_WORD_LEN) == 0) {
          strncpy(str, str0, MAX_WORD_LEN);
          strncpy(&str[MAX_WORD_LEN - 1], "\0", (size_t)1);
          pointer = strchr(argv[i + 1], 'x');
          if (pointer == NULL)
            pointer = strchr(argv[i + 1], ':');
          if (pointer) {
            strncpy(str, argv[i + 1], strlen(argv[i + 1]) - strlen(pointer));
            par->width = strtol(str, &endptr, 10);
            if (par->width == 0 && str == endptr) {
              sw++;
            }
            par->height = strtol(pointer + 1, &endptr, 10);
            if (par->height == 0 && pointer + 1 == endptr) {
              sw++;
            }
            i++;
          }

          if (sw == 0) {
            opt->geometria = TRUE;
          } else {
            fprintf(stderr,
                    "Warning: -g option bad formed. Using default values.\n");
          }
        }
        break;
      case 'd':
        if (strncmp("d", arg, MAX_WORD_LEN) == 0) { // delay between frames
          opt->pausa = TRUE;
          par->pausa = strtol(argv[i + 1], &endptr, 10);
          if (par->pausa == 0 && argv[i + 1] == endptr) {
            opt->pausa = FALSE;
            fprintf(
                stderr,
                "Warning: -d option must have an argument. Setting default.\n");
          }
        }
        break;
      case 'c':
        if (strncmp("c", arg, MAX_WORD_LEN) == 0 || // there is a color column
            strncmp("color", arg, MAX_WORD_LEN) == 0) {
          opt->color = TRUE;
        }
        break;
      case 'r':
        if (strncmp("r", arg, MAX_WORD_LEN) == 0 || // there is a radio column
            strncmp("radio", arg, MAX_WORD_LEN) == 0) {
          opt->radio = TRUE;
          strncpy(str, str0, MAX_WORD_LEN);
          strncpy(&str[MAX_WORD_LEN - 1], "\0", (size_t)1);
          par->max_r = par->min_r = 0;

          pointer = strchr(argv[i + 1], ':');
          if (pointer) {
            strncpy(str, argv[i + 1], strlen(argv[i + 1]) - strlen(pointer));
            par->min_r = (float)strtod(str, NULL);
            par->max_r = (float)strtod(pointer + 1, NULL);
            i++;
          }
        }
        break;
      case 'D':
        if (strncmp("D", arg, MAX_WORD_LEN) == 0) { // Dimension
          opt->dimension = TRUE;
          par->dimension = (int)strtod(argv[i + 1], NULL);
        }
        break;
      case 'B':
        if (strncmp("B", arg, MAX_WORD_LEN) == 0) { // show a border box
          pointer = strchr(argv[i + 1], ':');
          if (pointer) {
            strncpy(par->boxstr, argv[i + 1], MAX_WORD_LEN);
            strncpy(&par->boxstr[MAX_WORD_LEN - 1], "\0", (size_t)1);
            opt->sizebox = TRUE;
            i++;
          }
          opt->box = TRUE;
        }
        break;
      case 'w':
      case 'G':
        if (strncmp("G", arg, MAX_WORD_LEN) == 0 ||
            strncmp("w", arg, MAX_WORD_LEN) == 0) { // Gradient scale
          strncpy(str, str0, MAX_WORD_LEN);
          strncpy(&str[MAX_WORD_LEN - 1], "\0", (size_t)1);
          pointer = strchr(argv[i + 1], ':');
          par->max_c = par->min_c = 0;
          if (pointer) {
            strncpy(str, argv[i + 1], strlen(argv[i + 1]) - strlen(pointer));
            par->min_c = (float)strtod(argv[i + 1], NULL);
            par->max_c = (float)strtod(pointer + 1, NULL);
            flag = 1;
            i++;
          }
          opt->gradient = TRUE;
          opt->color = TRUE;
        }
        break;
      case 'p':
        if (strncmp("p", arg, MAX_WORD_LEN) == 0) { // choose color palette
          opt->palette = strtol(argv[i + 1], &endptr, 10);
          if (opt->palette == 0 && argv[i + 1] == endptr) {
            opt->palette = DEFAULTPALETTE;
            fprintf(
                stderr,
                "Warning: -p option must have an argument. Setting default.\n");
          }
        }
        break;
      case 'V':
        if (strncmp("V", arg, MAX_WORD_LEN) == 0) { // Vector field
          strncpy(str, str0, MAX_WORD_LEN);
          strncpy(&str[MAX_WORD_LEN - 1], "\0", (size_t)1);
          pointer = strchr(argv[i + 1], ':');
          par->max_f = par->min_f = 0;
          sw = 0;
          if (pointer) {
            strncpy(str, argv[i + 1], strlen(argv[i + 1]) - strlen(pointer));
            par->min_f = (float)strtod(argv[i + 1], &endptr);
            if (par->min_f == 0 && argv[i + 1] == endptr)
              sw++;
            par->max_f = (float)strtod(pointer + 1, &endptr);
            if (par->max_f == 0 && pointer + 1 == endptr)
              sw++;
            if (sw == 0) {
              flag = 1;
              opt->fieldcom = TRUE;
            } else {
              fprintf(stderr,
                      "Warning: -V option bad formed. Using default values.\n");
            }
            i++;
          }
          opt->field = TRUE;
        }
        break;
      case 'e':
        if (strncmp("e", arg, MAX_WORD_LEN) == 0) {
          if (argv[i + 1] == NULL) {
            fprintf(stderr, "-e needs and argument");
            exit(1);
          }
          opt->exit = TRUE;
          strncpy(par->path, argv[i + 1], MAX_WORD_LEN);
        }
        break;
      case 'v':
        if (strncmp("verbose", arg, MAX_WORD_LEN) == 0) { // verbose
          opt->verbose++;
        }
        break;
      case 'b':
        if (strncmp("bg", arg, MAX_WORD_LEN) == 0) { // background color
          opt->back = TRUE;
          strncpy(par->backname, argv[i + 1], MAX_WORD_LEN);
          strncpy(&par->backname[MAX_WORD_LEN - 1], "\0", (size_t)1);
        }
        break;
      case 'f':
        if (strncmp("fg", arg, MAX_WORD_LEN) == 0) { // foreground color
          opt->fore = TRUE;
          strncpy(par->forename, argv[i + 1], MAX_WORD_LEN);
          strncpy(&par->forename[MAX_WORD_LEN - 1], "\0", (size_t)1);
        }
        break;
      case 'l':
        if (strncmp("lc", arg, MAX_WORD_LEN) == 0) { // led color
          opt->led = TRUE;
          strncpy(par->ledname, argv[i + 1], MAX_WORD_LEN);
          strncpy(&par->ledname[MAX_WORD_LEN - 1], "\0", (size_t)1);
        }
        break;
      case 'F':
        if (strncmp("F", arg, MAX_WORD_LEN) ==
            0) { // root file name to save pixmaps
          opt->file = TRUE;
          strncpy(par->rootfilename, argv[i + 1], MAX_WORD_LEN);
          strncpy(&par->rootfilename[MAX_WORD_LEN - 1], "\0", (size_t)1);
        }
        break;
      case 'T':
        if (strncmp("T", arg, MAX_WORD_LEN) == 0) {
          opt->title = TRUE;
          strncpy(par->titlewindow, argv[i + 1], MAX_WORD_LEN);
          strncpy(&par->titlewindow[MAX_WORD_LEN - 1], "\0", (size_t)1);
        }
        break;
      case 'P':
        if (strncmp("P", arg, MAX_WORD_LEN) == 0) {
          opt->periodic = TRUE;
        }
        break;
      default:
        break;
      }
    }
  }
  if (opt->gradient == TRUE)
    opt->real_color = TRUE;
  if (opt->real_color == TRUE)
    opt->color = TRUE;
  if (opt->field) {
    opt->radio = FALSE;
  }
  return (0);
}

void SetValues(struct Values val_d, struct Parametres par, struct Options opt,
               struct Values *val) {
  val->max_r = val_d.max_r;
  val->min_r = val_d.min_r;
  if (opt.radio == TRUE) {
    if (par.max_r != 0 || par.min_r != 0) {
      val->max_r = par.max_r;
      val->min_r = par.min_r;
    }
  }

  val->max_c = val_d.max_c;
  val->min_c = val_d.min_c;
  if (opt.real_color == TRUE) {
    if (opt.verbose > 1)
      printf("real color\n");
    if (par.max_c != par.min_c) {
      val->max_c = par.max_c;
      val->min_c = par.min_c;
    }
  }

  if (opt.field == TRUE) {
    val->max_f = val_d.max_f;
    val->min_f = val_d.min_f;
    if (opt.fieldcom == TRUE) {
      val->max_f = par.max_f;
      val->min_f = par.min_f;
    }
  }

  val->delay = 0;
  if (opt.pausa == TRUE) {
    val->delay = (float)par.pausa;
  }
};

int SetDimension(struct Parametres par, struct Options opt, int ncol,
                 int noptcol) {
  int num_options;
  int d;

  d = 3;
  if (opt.dimension)
    d = par.dimension;

  else {
    if (opt.column)
      ncol = noptcol;

    if (ncol - opt.radio - opt.color < d)
      d = ncol - opt.radio - opt.color;
    if (opt.field) {

      num_options = (opt.color == TRUE) + (option.radio == TRUE);
      if (ncol - num_options >= 6)
        d = 3;
      else if (ncol - num_options >= 4)
        d = 2;
      else if (ncol - num_options >= 2)
        d = 1;
      else {
        fprintf(stderr, "ERROR: wrong number of columns, \n");
        fprintf(stderr, "\twith option V must be 2,4 or 6 columns (sorry)\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  return (d);
}

int GetColumns(char *str, int ncolf, int *p_col) {
  int i;
  char *cpointer;
  int n;

  for (i = 0; i < 7; i++) {
    p_col[i] = 0;
    cpointer = strchr(str, ':');
    if (cpointer) {
      strncpy(str, str, strlen(str) - strlen(cpointer) + 1);
      p_col[i] = atoi(str);
      if (p_col[i] == 0) {
        fprintf(stderr, "ERROR: invalid column number in -z option\n");
        exit(EXIT_FAILURE);
      }
      cpointer++;
      strncpy(str, cpointer, strlen(cpointer) + 1);
    } else {
      strncpy(str, str, strlen(str) + 1);
      p_col[i] = atoi(str);
      if (p_col[i] == 0) {
        fprintf(stderr, "ERROR: invalid column number in -z option\n");
        exit(EXIT_FAILURE);
      }
      break;
    }
  }
  n = i + 1;


  for (i = 0; i < n; i++) {
    if (p_col[i] > ncolf || p_col[i] < 0) {
      fprintf(stderr, "ERROR: invalid column number in -z option\n");
      if (p_col[i] > ncolf) {
        fprintf(stderr, "\tOnly there are %d columns in the first data block\n",
                ncolf);
      }
      exit(EXIT_FAILURE);
    }
  }
  return (n);
}

void OrderOfColumns(struct Options opt, int d, int ncol, int noptc, int *posc,
                    struct PosCol *p) {
  int idx = 0;

  p->x = p->y = p->z = ncol;
  p->r = ncol;
  p->c = ncol;
  p->vx = ncol;
  p->vy = ncol;
  p->vz = ncol;

  if (opt.column == FALSE) {
    if (d > 0)
      p->x = idx++;
    if (d > 1)
      p->y = idx++;
    if (d == 3)
      p->z = idx++;
    if (opt.radio)
      p->r = idx++;
    if (opt.color)
      p->c = idx++;
    if (opt.field) {
      if (d > 0)
        p->vx = idx++;
      if (d > 1)
        p->vy = idx++;
      if (d == 3)
        p->vz = idx++;
    }
  } else {
    if (d * (1 + (opt.field == TRUE)) + opt.radio + opt.color != noptc) {
      fprintf(stderr, "ERROR:\twrong column options\n");
      fprintf(stderr, "\tyou must specified %d columns\n",
              d * (1 + (opt.field == TRUE)) + (opt.radio == TRUE) +
                  (opt.color == TRUE));
      printf("color: %d radio: %d\n", opt.color, opt.radio);
      exit(EXIT_FAILURE);
    }
    idx = 0;
    if (d == 1) {
      p->x = posc[idx] - 1;
      idx++;
    }
    if (d == 2) {
      p->x = posc[idx] - 1;
      idx++;
      p->y = posc[idx] - 1;
      idx++;
    }
    if (d == 3) {
      p->x = posc[idx] - 1;
      idx++;
      p->y = posc[idx] - 1;
      idx++;
      p->z = posc[idx] - 1;
      idx++;
    }
    if (opt.radio == TRUE) {
      p->r = posc[idx] - 1;
      idx++;
    }
    if (opt.color == TRUE) {
      p->c = posc[idx] - 1;
      idx++;
    }

    if (opt.field) {
      if (d == 1) {
        p->vx = posc[idx] - 1;
        idx++;
      }
      if (d == 2) {
        p->vx = posc[idx] - 1;
        idx++;
        p->vy = posc[idx] - 1;
        idx++;
      }
      if (d == 3) {
        p->vx = posc[idx] - 1;
        idx++;
        p->vy = posc[idx] - 1;
        idx++;
        p->vz = posc[idx] - 1;
        idx++;
      }
    }
  } // else.
}

int GetBox(int d, struct Parametres *par) {
  int i, cont = 0;
  double tmp[6] = {0, 0, 0, 1, 1, 1};
  char *cpointer;
  char str[MAX_WORD_LEN] = "";
  char bstr[MAX_WORD_LEN] = "";
  char *endptr = NULL;

  strncpy(bstr, par->boxstr, MAX_WORD_LEN);
  strncpy(&bstr[MAX_WORD_LEN - 1], "\0", (size_t)1);

  cpointer = strchr(bstr, ':');
  if (cpointer) {
    for (i = 0; i < 2 * d; i++) {
      cpointer = strchr(bstr, ':');
      if (cpointer) {
        strncpy(str, bstr, strlen(bstr) - strlen(cpointer) + 1);
        tmp[i] = (float)strtod(str, &endptr);
        if (str != endptr) {
          cont++;
        }
        cpointer++;
        strncpy(bstr, cpointer, strlen(cpointer) + 1);
      } else {
        strncpy(str, bstr, strlen(bstr) + 1);
        tmp[i] = (float)strtod(str, &endptr);
        if (str != endptr) {
          cont++;
        }
        break;
      }
    }
  }
  if (cont != 2 * d) {
    fprintf(stderr, "Warning: Bad former argument -B. Must have %d elements \n",
            2 * d);
    fprintf(stderr, "\t Box parametres readed: %s\n", par->boxstr);
    return (1);
  }

  par->box.x0 = 0;
  par->box.y0 = 0;
  par->box.z0 = 0;
  par->box.x1 = 0;
  par->box.y1 = 0;
  par->box.z1 = 0;
  if (d == 1) {
    par->box.x0 = tmp[0];
    par->box.x1 = tmp[1];
  }
  if (d == 2) {
    par->box.x0 = tmp[0];
    par->box.y0 = tmp[1];
    par->box.x1 = tmp[2];
    par->box.y1 = tmp[3];
  }
  if (d == 3) {
    par->box.x0 = tmp[0];
    par->box.y0 = tmp[1];
    par->box.z0 = tmp[2];
    par->box.x1 = tmp[3];
    par->box.y1 = tmp[4];
    par->box.z1 = tmp[5];
  }
  return (0);
}

void BoxProperties(struct Values val_d, struct Parametres par,
                   struct Options opt, struct Punto *b) {
  b[0].x = val_d.min_x;
  b[0].y = val_d.min_y;
  b[0].z = val_d.max_z;

  b[1].x = val_d.max_x;
  b[1].y = val_d.min_y;
  b[1].z = val_d.max_z;

  b[2].x = val_d.max_x;
  b[2].y = val_d.min_y;
  b[2].z = val_d.min_z;

  b[3].x = val_d.min_x;
  b[3].y = val_d.min_y;
  b[3].z = val_d.min_z;

  b[4].x = val_d.min_x;
  b[4].y = val_d.max_y;
  b[4].z = val_d.max_z;

  b[5].x = val_d.max_x;
  b[5].y = val_d.max_y;
  b[5].z = val_d.max_z;

  b[6].x = val_d.max_x;
  b[6].y = val_d.max_y;
  b[6].z = val_d.min_z;

  b[7].x = val_d.min_x;
  b[7].y = val_d.max_y;
  b[7].z = val_d.min_z;

  if (opt.sizebox == TRUE) {
    b[0].x = par.box.x0;
    b[0].y = par.box.y0;
    b[0].z = par.box.z1;

    b[1].x = par.box.x1;
    b[1].y = par.box.y0;
    b[1].z = par.box.z1;

    b[2].x = par.box.x1;
    b[2].y = par.box.y0;
    b[2].z = par.box.z0;

    b[3].x = par.box.x0;
    b[3].y = par.box.y0;
    b[3].z = par.box.z0;

    b[4].x = par.box.x0;
    b[4].y = par.box.y1;
    b[4].z = par.box.z1;

    b[5].x = par.box.x1;
    b[5].y = par.box.y1;
    b[5].z = par.box.z1;

    b[6].x = par.box.x1;
    b[6].y = par.box.y1;
    b[6].z = par.box.z0;

    b[7].x = par.box.x0;
    b[7].y = par.box.y1;
    b[7].z = par.box.z0;
  }
}

void SetInitialValues(struct Options *opt, struct Keys *k) {
  opt->color = FALSE;
  opt->radio = FALSE;
  opt->geometria = FALSE;
  opt->number = FALSE;
  opt->box = FALSE;
  opt->comment = TRUE;
  opt->realsize = TRUE;
  opt->dimension = FALSE;
  opt->size = FALSE;
  opt->cut = FALSE;
  opt->type = DEFAULTTYPE;
  opt->column = FALSE;
  opt->real_color = FALSE;
  opt->gradient = FALSE;
  opt->field = FALSE;
  opt->fieldcom = FALSE;
  opt->varfield = TRUE;
  opt->palette = FALSE;
  opt->verbose = FALSE;
  opt->back = FALSE;
  opt->fore = FALSE;
  opt->led = FALSE;
  opt->file = FALSE;
  opt->signal = FALSE;
  opt->sizebox = FALSE;
  opt->rotate = TRUE;
  opt->pausa = FALSE;
  opt->mode = ZOOM;
  opt->anim = FALSE;
  opt->reverse = FALSE;
  opt->save = FALSE;
  opt->exit = FALSE;
  opt->title = FALSE;
  opt->trace = FALSE;
  opt->ntrace = FALSE;
  opt->tracelines = FALSE;
  opt->periodic = FALSE;

  k->z = FALSE;
  k->n = FALSE;
  k->i = FALSE;
  k->plus = FALSE;
  k->minus = FALSE;
  k->q = FALSE;
  k->b = FALSE;
  k->f = FALSE;
  k->s = FALSE;
  k->r = FALSE;
  k->F = FALSE;
  k->S = FALSE;
  k->u = FALSE;
  k->t = FALSE;

  k->up = FALSE;
  k->arrow = TRUE + TRUE;
  k->down = FALSE;
  k->left = FALSE;
  k->right = FALSE;
  k->shift = FALSE;
  k->less = FALSE;
  k->k_4 = FALSE;
  k->k_5 = FALSE;
  k->k_6 = FALSE;
  k->k_7 = FALSE;
  k->k_8 = FALSE;
  k->k_9 = FALSE;
  k->k_0 = FALSE;
  k->pressed = FALSE;
  k->user = FALSE;
}
