#define TRUE 1
#define FALSE 0

#define INFO FALSE

#define CIRCLE 0
#define SQUARE 1
#define SPHERE 2

//#define PI 3.14159265358979323846
#define PI 3.141592
#define INC_ANG PI / 200.;

#define ZOOM 0
#define NAV 1

#define FORE 1
#define BACK 2

#define BLOCK_BUF_SIZE 8192
#define MAX_LINE_LEN 4096
#define MAX_WORD_LEN 128
#define HEIGHT_OFFSET 20

#define BIGINT 1000000 /* max. number of puntos per frame */

#define ARROWCOLOR 2
#define LEDCOLOR 2

#define LEDW 50 /* width & height of the led zone */
#define LEDH 50

#define MAXNFILES 9999 /* max number of files to save */

#define WIDTH 640
#define HEIGHT 480 /* dimensions of the window */

#define MAX_NSIZE 64           /* num. of diferent sizes  */
#define MAX_NCOLORS 752        /*  464 number of names for colors  */
#define MAX_NSCOLORS 64        /* number of colors 3D */
#define MAX_NGRADIENTS 64      /* number of gradients of color, for 3D */
#define MAX_NCOLOR_GRADIENT 64 /* numero de gradientes para paleta */
#define NPIXSIMBOLS 92

#define TRACE_DEFAULT 50
#define TRACE_MAX 500 /* max number of trace points */

/* palettes */
#define SPECTRUM 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define GREY 4
#define GRAY 4

#define FONTSIZE 3

#define RETURN 1
#define COMMENT 2
#define DATA 3

#define CONFFILE ".puntorc"

#define DEFAULTZOOM 0.9
#define DEFAULTPALETTE SPECTRUM
#define DEFAULTRADIO 12
#define DEFAULTTYPE SPHERE
#define DEFAULTCOLOR 0

struct Options;
struct Parametres;

struct Point {
  float x, y, z;
};

struct Rectangle {
  int x, y; /* upper left corner */
  int l, h; /* lenght and hight */
};

struct Vector {
  double x0, y0, z0; /* origin */
  double x1, y1, z1; /* final  */
};

struct Punto {
  float x, y, z;
  float x1, y1, z1;
  float radio;
  float color;
  float vx, vy, vz;
  float vx1, vy1, vz1;
  int active;
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
  FILE *fp;                /* file descriptor. */
  char name[MAX_WORD_LEN]; /* name of the file */
  int nblocks;             /* numbre of blocks */
  int ncol;                /* number of columns; */
  long size;               /* size of the  file */
  int eof;                 /* its reached the end of file? */
};

struct RGBColor {
  Uint8 r, g, b;
  char name[MAX_WORD_LEN];
};

struct Buffer {
  long ipos, fpos;
  long size;
  char buf[BLOCK_BUF_SIZE]; // 8192
};

struct Block {
  int read; /* its already readed? */
  long pos; /* address to the data file position of the block */
  long len;
  long num;           /* number of elements */
  char *comment;      /* pointer to the comment of the block */
  struct Block *next; /* address of the next block */
  struct Block *prev; /* address of the previous block */
};

struct Values {
  float delay;
  int dimension;
  float max_r, min_r;
  float max_c, min_c;
  float max_f, min_f;
  float max_x, min_x, max_y, min_y, max_z, min_z;
  /*  float max_fx,min_fx,max_fy,min_fy,max_fz,min_fz; */
};

struct Keys {
  short z;                     /* zoom */
  short i;                     /* increase decrease size. */
  short q;                     /* quit */
  short b;                     /* border */
  short c;                     /* comments on off */
  short f;                     /* fast */
  short s;                     /* start stop */
  short S;                     /* save mode on off */
  short r;                     /* reverse animation */
  short F;                     /* save image */
  short n;                     /* navigation mode */
  short u;                     /* normalized to unit on off. */
  short t;                     /* trace on off */
  short p;                     /* periodic boundaries on off */
  short plus;                  /* increase delay */
  short minus;                 /* decrease delay */
  short period;                /* set delay to initial value. */
  short arrow;                 /* arrow type */
  short up, down, left, right; /* move */
  short pageup, pagedown;
  short shift;                             /* fast move */
  short less;                              /* slow move */
  short space;                             /* one by one step animation */
  short k_4, k_5, k_6, k_7, k_8, k_9, k_0; /* rotate */
  short pressed;
  short mclick;
  short mbdown; /* mouse buttom down */
  short mbup;   /* mouse buttom up */
  short user;   /* user signal */
};

struct Window {
  SDL_Surface *screen;
  int w, h; /* width, height */
  int bpp;
  Uint32 flags;
  struct Point shift;
};

struct Universe {
  struct Point ang;        /* polar coordenates, angles */
  float zoom;              /* zoom */
  float zoom_size;         /* zoom */
  struct Point dshift;     /* default shift of data */
  struct Point cm, cw, cv; /* centro geometrico datos, centro de la ventana,
                              centro visualizacion */
  struct Point escale;
  int psize; /* default size for puntos */
};

/*** funciones**/
/* void tam_sistema(RECTANGULO *); coord. limite de los datos */

int Arguments(int argc, char *argv[], struct Options *option,
              struct Parametres *val);

void SetValues(struct Values val_d, struct Parametres par, struct Options opt,
               struct Values *val);
void BoxProperties(struct Values val_d, struct Parametres param,
                   struct Options opt, struct Punto *b);
int SetDimension(struct Parametres par, struct Options opt, int ncol,
                 int noptcol);
int GetColumns(char *str, int ncolf, int *p_col);
void OrderOfColumns(struct Options opt, int d, int ncol, int noptc, int *posc,
                    struct PosCol *p);
int GetBox(int d, struct Parametres *param);
void SetInitialValues(struct Options *opt, struct Keys *k);

struct Punto *ReadBlock(int *n, struct DataFile, struct PosCol pos, long *,
                        struct Punto *p, int *);
struct Punto *ReadBlock_00(int *n, FILE *fp, struct PosCol pos, int num_col,
                           long *, struct Punto *p, int *);

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
void DrawInfo(SDL_Surface *screen, struct Window win, struct Universe,
              struct DataFile df);

void MovePuntos(struct Universe, struct Parametres par, struct Punto *);
void PurgeSprites(struct Sprite *, int);

void RangeOfValues(struct Punto *p, struct Options, struct Values *data);

int CountSprites(struct Sprite *b, int nmax);
void InfoSprites(struct Sprite *b, struct Options opt);

int NextBlock(float x, char *fname);
