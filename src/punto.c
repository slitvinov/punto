#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include "punto.h"
#include "maths.h"
#include "graphics.h"
#include "graphics-sdl.h"
#include "file.h"
#include "event.h"
#include "rgb.h"
#include "help.h"
#include "buffer.h"

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

char version[] = {"1.0.04"};
char last_revision[] = {"Jun 2009"};

struct Sprite *bola;
int numsprites;

long total_memory = 0;

int actualizar = TRUE;
int ntrace = TRACE_DEFAULT;

char defaultcolorname[MAX_WORD_LEN] = "blue";
struct Options option;

struct Punto box[8]; /* coordinates of the box */

int nsprites;
int max_nsize; /* max. number of diferent sizes (for sprites) */

long numactivados;
long N;     /* number of particles of one block */
long max_N; /*  max. number of puntos along all the file */

int ContBlock = 0;
int nextblock = 0; /* next block to read */

struct Block *block;
struct Punto *punto; /*  original data (file data) */
long *tabla;
int reset_box = TRUE;

int Cont = 0;
int kEOF = FALSE;

Uint32 colortable[MAX_NSCOLORS];
SDL_Surface *screen, *screen2;
Uint32 videoflags;
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

int main(int argc, char *argv[]) {
  Uint32 background;
  long i;
  int done;
  SDL_Event event;
  Uint32 then, now, frames;
  char title[MAX_WORD_LEN] = "";
  int pos_columns[7];

  int n_option_columns = 0; /* number of columns in input (-z option) */

  struct Window window;
  int statusblock;
  int inmouse = 0;
  Uint32 time, time0;
  long filesize;

  int sw_trace = 0;

  struct Block *b;
  struct PosCol pos; /* position of the columns */

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

  /*
   * data file properties
   */

  if ((dfile.fp = fopen(dfile.name, "rt")) == NULL) {
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

  /* Read file properties: n. of columns , n. data rows of the first block  */

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
  /* dimension */

  val_run.dimension = SetDimension(param, option, dfile.ncol, n_option_columns);
  if (val_run.dimension > 3 || val_run.dimension < 1) {
    fprintf(stderr, "ERROR. Wrong Dimension of the data\n");
    fprintf(stderr, "\tDimension calculated: %d\n", val_run.dimension);
    exit(EXIT_FAILURE);
  }
  /* --dimension */

  if (option.type > 2 || option.type < 0) { /* Default punto type */
    option.type = SPHERE;
    fprintf(stderr, "Warning: incorrect option, setting type to sphere\n");
  }

  /* order of columns  */
  OrderOfColumns(option, val_run.dimension, dfile.ncol, n_option_columns,
                 pos_columns, &pos);
  /*  --order of columns */

  /* assign memory */

  max_N = N + 1000;

  if ((punto = (struct Punto *)calloc((size_t)max_N, sizeof(struct Punto))) ==
      NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  if ((tabla = (long *)calloc((size_t)max_N, sizeof(long))) == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  total_memory += max_N * (sizeof(struct Punto) + sizeof(long));

  /* initializate the memory */
  for (i = 0; i < max_N; i++)
    tabla[i] = i;

  dfile.nblocks = 0;
  if ((block = (struct Block *)calloc((size_t)1, sizeof(struct Block))) ==
      NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }
  block[0].read = FALSE;
  block[0].comment = nocomment;

  /* reading blocks */

  broot = CreateBlock();
  broot->pos = 0;
  broot->len = 0;
  broot->next = 0;
  broot->prev = 0;

  dfile.nblocks += ReadNBlocks(dfile.name, broot, 0);
  punto = ReadBlock(&nextblock, dfile, pos, &numactivados, punto, &statusblock);

  RangeOfValues(punto, option, &val_data);

  universe.cm.x = (val_data.max_x + val_data.min_x) / 2;
  universe.cm.y = (val_data.max_y + val_data.min_y) / 2;
  universe.cm.z = (val_data.max_z + val_data.min_z) / 2;
  universe.cv = universe.cm;

  /* values to use in the program */

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

  /* --values to use in the program */

  /* box */
  if (option.sizebox == TRUE) {
    if (GetBox(val_run.dimension, &param))
      option.sizebox = FALSE;
  }
  BoxProperties(val_data, param, option, box); /* properties of the box */
  /* --box */
  if (option.verbose > 1) {
    printf("CM: %f %f %f\n", universe.cm.x, universe.cm.y, universe.cm.z);
    printf("Cv: %f %f %f\n", universe.cv.x, universe.cv.y, universe.cv.z);
  }
  numsprites = (max_nsize + 1) * MAX_NSCOLORS;
  if ((bola = (struct Sprite *)malloc((size_t)(numsprites) *
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
    punto[i].active = 1;
  }

  /* Initialize SDL */

  if (SDL_Init((Uint32)(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE |
                        SDL_INIT_TIMER)) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  atexit(SDL_Quit);

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  /* Set video mode */
  // window.flags = SDL_SWSURFACE|SDL_ANYFORMAT|SDL_RESIZABLE;
  window.flags = (Uint32)(SDL_HWSURFACE | SDL_ANYFORMAT | SDL_RESIZABLE |
                          SDL_DOUBLEBUF | SDL_ASYNCBLIT);
  //  window.flags = SDL_ANYFORMAT;
  window.w = WIDTH;
  window.h = HEIGHT;
  if (option.geometria == TRUE) {
    window.w = param.width;
    window.h = param.height;
  }
  window.bpp = 8;

  screen = SDL_SetVideoMode(window.w, window.h, window.bpp, window.flags);
  if (!screen) {
    fprintf(stderr, "Couldn't set %dx%d video mode: %s\n", window.w, window.h,
            SDL_GetError());
    exit(EXIT_FAILURE);
  }
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
  SDL_WM_SetIcon(CreateBall3D(screen, fgcolor, 15), NULL);
  SDL_WM_SetCaption(title, "punto");

  /* Print out information about our surfaces */
  if (option.verbose > 1) {
    printf("Screen is at %d bits per pixel\n",
           (int)screen->format->BitsPerPixel);
    if ((screen->flags & SDL_HWSURFACE) == SDL_HWSURFACE) {
      printf("Screen is in video memory\n");
    } else {
      printf("Screen is in system memory\n");
    }
    if ((screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF) {
      printf("Screen has double-buffering enabled\n");
    }
    if ((screen->flags & SDL_HWACCEL) == SDL_HWACCEL) {
      printf("Sprite blit uses hardware acceleration\n");
    }
  }
  /* --Initialize SDL */

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

  universe.escale.x = (float)window.w / (val_data.max_x - val_data.min_x);
  universe.escale.y = (float)window.h / (val_data.max_y - val_data.min_y);
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
    //    printf("optionradio:%d universe.psize: %d
    //    spr:%p\n",option.radio,universe.psize,bola[0].surface);
    for (i = 1; i < numactivados; ++i) {
      punto[i].sprite = bola[0].surface;
    }
  }

  /* Loop, blitting sprites and waiting for a keystroke */
  frames = 0;
  then = SDL_GetTicks();
  done = 0;

  /* Enable UNICODE translation for keyboard input */
  SDL_EnableUNICODE(1);

  /* Enable auto repeat for keyboard input */
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  for (i = 0; i < ntrace; i++) {
    if ((traceball[i] =
             (struct Punto *)calloc((size_t)1, sizeof(struct Punto))) == NULL) {
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
    int md_x, md_y; /* coordinates when mouse is pressed */
    int mu_x, mu_y; /* coordinates when mouse is released */

    /* Check for events */
    ++frames;
    inc_ang2 = INC_ANG;
    status = FALSE;
    //    printf("%g ",zoom);fflush(stdout);
    if (!delay && option.anim == TRUE)
      delay = val_run.delay * 10;

    if (cont != 0) {

      if (((option.anim == TRUE && kEOF == FALSE) || kp.space == TRUE ||
           kp.pressed)) { /* animation */

        do {
          status = CheckEvent(
              &event); /* look for events, if exists, puts the first in event */
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
          status = NextEvent(&event); /* wait for a event and put it in event */
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
      option.mode = ZOOM; // HERE TODO option mode and navigation

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
          /* freeing memory  */
          //      DrawTracePuntos(screen,window,universe,0,0);
        }
        option.trace++;
        if (option.trace > 2)
          option.trace = 0;
        //  option.trace=(option.trace==TRUE) ? FALSE : TRUE;
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

    } /* if status==TRUE */

    if (option.trace == FALSE && sw_trace) {
      if (sw_trace - nextblock > 1 || nextblock - sw_trace > 1) {
        /* freeing memory  */
        DrawTracePuntos(screen, window, universe, param, 0, 0);
        sw_trace = 0;
      }
    }

    /* mouse click handle */
    {
      inmouse = 0;
      md_x = md_y = mu_x = mu_y = 0;
      if (kp.mbdown == TRUE) {
        (void)SDL_GetMouseState(&md_x, &md_y);
        kp.mbdown = FALSE;
      }
      if (kp.mbup == TRUE && kp.mclick == TRUE) {
        (void)SDL_GetMouseState(&mu_x, &mu_y);
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
      /* the mouse clic event its finished */
      if (kp.mbup == TRUE)
        kp.mclick = FALSE;
    }
    /* --mouse click handle */

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
      //      option.anim=FALSE;
      kp.space = FALSE;
    }

    Cont++;
    cont++;
  } /* while(done) */
  for (i = 0; i < numsprites; i++) {
    if (bola[i].on == TRUE) {
      SDL_FreeSurface(bola[i].surface);
    }
  }
  now = SDL_GetTicks();
  SDL_Quit();
  return (0);
}

void DrawInfo(SDL_Surface *screen, struct Window win, struct Universe u,
              struct DataFile df) {
  // void DrawInfo(SDL_Surface *screen, struct Window win,struct Universe
  // u,Uint32 background){
  //  int mouse_x,mouse_y;
  int x;
  float y;
  int fs = FONTSIZE;

  /*      int fw=fs+3; */
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

  /*    SDL_GetMouseState(&mouse_x, &mouse_y); */
  /*    snprintf(num,MAX_WORD_LEN,"%d",mouse_x); */
  /*    x=DrawText(screen,num,5,win.h-40,ledcolor); */
  /*    snprintf(num,MAX_WORD_LEN,"%d",mouse_y); */
  /*    x=DrawText(screen,num,5+x,win.h-40,ledcolor); */

  if (option.fast == FALSE) {
    //    if(option.comment==TRUE)
    //    x=DrawText(screen,comment,5,win.h-20,ledcolor);
    if (option.real_color)
      DrawScale(screen, colortable, ledcolor, val_run, 10, 60);
  }
}

void MovePuntos(struct Universe u, struct Parametres par, struct Punto *p) {
  // void MovePuntos(struct Universe u,struct Punto *p){
  long n1, n2;
  static int cont;

  if (cont == 0) { /* initialization */
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

    /* delete the screen */

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
      //      printf("%f %f\n",u.escale.x,u.zoom);
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

    /* Update the screen! */
    if ((screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF) {
      SDL_Flip(screen);
      printf(".");
    } else if (option.fast == FALSE || actualizar == TRUE)
      SDL_UpdateRect(screen, 0, 0, 0, 0);
    else {
      SDL_UpdateRect(screen, 0, 0, LEDW, LEDH);
      printf(".");
    }
    /*      SDL_Flip(screen2); */
    /*      SDL_UpdateRect(screen2, 0,0,LEDW,LEDH); */
  }
}

void DrawPuntos(SDL_Surface *screen, struct Window w, struct Universe u,
                struct Punto *p, long *t, struct Point cwin, float z,
                struct Parametres par, struct Values data) {
  int i;
  SDL_Rect position;
  Uint32 time;
  float x, y;
  int max, max2;
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
  max2 = 2 * max;
  factor = .5 * u.psize * u.zoom * u.zoom_size;
  /* draw the sprite */
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
        //  printf("%f %f %f\n",radio,z,u.zoom_size);

        sizeidx = (int)(radio * z * u.zoom_size);
      }
      //     printf("%d ",sizeidx);
    }
    if (sizeidx > max_nsize)
      sizeidx = max_nsize;
    if (sizeidx < 1)
      sizeidx = 0;

    if (option.color) {
      fgcolor = colortable[coloridx];
    }

    idx = coloridx + MAX_NSCOLORS * sizeidx;

    //     printf("n: %d  IDX:%d option_type:%d
    //     size_idx:%d\n",i,idx,option.type,sizeidx);
    bola[idx].cont = Cont;
    bola[idx].time = time;

    if (option.field == FALSE) {
      if (bola[idx].on == FALSE) {
        if (option.verbose > 1) {
          printf(" (%d) %d+%d=%d ", idx, sizeidx, coloridx, idx);
          fflush(stdout);
        }

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
        total_memory += (sizeidx + 1) * (sizeidx + 1) * 16;
        bola[idx].on = TRUE;
      }

      p[t[i]].sprite = bola[idx].surface;
      /* Blit the sprite onto the screen */

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
        /* factor=.5*size*universe.zoom*u.zoom_size*sqrt(f.x*f.x+f.y*f.y)/val_run.max_f;
         */
        norm = val_run.max_f;
      } else {
        norm = sqrt(f.x * f.x + f.y * f.y);
      }
      if (norm == 0)
        norm = 1;
      DrawArrow(screen, position, f, factor / norm, fgcolor, kp.arrow);
    }
  }

  /*    posx=0;  */
  /*    posy=20;  */

  /*    for(i=0;i<numsprites;i++){   */
  /*      if(bola[i].on==TRUE){  */
  /*        position.x =posx;  */
  /*        position.y =posy;  */
  /*        SDL_BlitSurface(bola[i].surface, NULL, screen, &position);  */
  /*        posx+=10;  */
  /*        if(posx>620){posx=0;posy+=50;}  */
  /*      }  */
  /*    }  */

  if (option.verbose > 1)
    printf("m: %ldkB t:%d z:%f\n", total_memory / 1024, (int)(time / 1000), z);

  if (option.field) {
    /* Unlock the surface. */
    SDL_UnlockSurface(screen);
  }
} /* --DrawPuntos */

void DrawTracePuntos(SDL_Surface *screen, struct Window w, struct Universe u,
                     struct Parametres par, float z, int order) {
  long int i, n1, n2;
  int j, k, l;
  float lenx, leny, lenz;
  static int block;
  static int trace_num = 0;
  static int trace_pos = 0;
  static int trace_cont = 1;

  if (order == 0) { /* freeing memory */
    trace_pos = 0;
    trace_cont = 1;
    trace_num = 1;
    if (option.verbose > 0)
      printf("FREEING MEM (trace points)\n");
    for (i = 0; i < ntrace; i++) {
      if ((traceball[i] = (struct Punto *)realloc(
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
      //      return(0);
    }

    block = nextblock;
  }

  /* reallocating memory for the trace points */

  if (numactivados > trace_num) {

    if (option.verbose > 1)
      printf("REALLOCATING\n");
    for (i = 0; i < ntrace; i++) {
      if ((traceball[i] = (struct Punto *)realloc(
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
  /* --reallocating memory for the trace points */

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
  struct Punto *p0;
  float ix, iy, iz;
  long n1, n2;
  struct Point c;

  c = u.cm;
  if ((p0 = (struct Punto *)malloc((size_t)numactivados *
                                   sizeof(struct Punto))) == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
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

  free(p0);
  return (0);
}

void RangeOfValues(struct Punto *p, struct Options opt, struct Values *data) {
  /*
     function RangeofValues
     set data with the max and min values of the first data block.
   */
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
  long maxpos, minpos;
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
        minpos = (long)b[i].surface;
        sw = 1;
      }
      if ((long)b[i].surface < minpos)
        minpos = (long)b[i].surface;
      if ((long)b[i].surface > maxpos) {
        maxpos = (long)b[i].surface;
      }
    }
  }
  if (mem) {
    if (option.verbose) {
      printf("num spr: %ld (%ld) mem: %ld\n", cont, numactivados, mem);
      printf("MAXPOS: %ld MINPOS: %ld dif: %ld\n", maxpos, minpos,
             maxpos - minpos);
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
  //  printf("time:%lu\n",time);
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
        //  total_memory-=(i%max_nsize)*(i%max_nsize)*16;
        total_memory -=
            ((float)i / MAX_NSCOLORS) * ((float)i / MAX_NSCOLORS) * 16;
        cont1++;
      } else {
        cont2++;
      }
    }
    //    printf("cont:%d %d %d\n",cont1,cont2,cont3);
    break;

  case 1:
    for (i = 0; i < numsprites; i++) {
      if (b[i].on == TRUE) {
        //  printf("%d %ld\n",i,b[i].surface);
        b[i].on = FALSE;
        b[i].cont = 0;
        b[i].time = 0;
        SDL_FreeSurface(b[i].surface);
        b[i].surface = NULL;
        if (option.verbose > 1)
          printf(" <(%d) %d+%d ", i, i % max_nsize, (int)(i / max_nsize));
        total_memory -= (i % max_nsize) * (i % max_nsize) * 16;
      }
    }
    break;

  case 2:
    for (i = 0; i < numsprites; i++) {
      // for(j=0;j<64;j++){
      //      i=(int)((float)rand()/RAND_MAX*(max_nsize+1)*(MAX_NSCOLORS));
      // printf("i:%d\n",i);
      //    printf("Purge 2: %d\n",Cont);
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
          //    total_memory-=(i%max_nsize)*(i%max_nsize)*16;
          total_memory -=
              ((float)i / MAX_NSCOLORS) * ((float)i / MAX_NSCOLORS) * 16;
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
        //  total_memory-=(i%max_nsize)*(i%max_nsize)*16;
        total_memory -=
            ((float)i / MAX_NSCOLORS) * ((float)i / MAX_NSCOLORS) * 16;
        continit++;
      }
    }
    //    printf("continit: %d %d %d\n",continit,init,i);
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
        total_memory -= (i % max_nsize) * (i % max_nsize) * 16;
      }
    }
    break;
  }
}

void Reescal_Color(int N, struct Punto *p1, struct Punto *p, float max_nc,
                   float min_color, float color_factor) {
  int i;

  for (i = 0; i < N; i++) {
    p1[i].color = (p[i].color - min_color) * color_factor;
    if (p1[i].color < 0) {
      p1[i].color = 0;
    }
    if (p1[i].color > max_nc - 1) {
      p1[i].color = max_nc - 1;
    }
  }
}

struct Punto *ReadBlock(int *next, struct DataFile df, struct PosCol pos,
                        long *numact, struct Punto *p, int *status) {
  /*
     Read the next block, and actualize the struct Punto p.
     keep the new comment if exists.

     returns:
     the struct Punto that has the data,
     may be different if there is a reallocation of memory.
   */
  size_t i, j, k;
  double *temp;
  long cont = 0;
  int reallocpunto = FALSE;
  static int lastblock = -1; /* last block readed */
  static int maxblock = 0;   /* max number block  */

  //  char line[MAX_LINE_LEN];
  struct Block *b;
  char line[MAX_LINE_LEN];
  long rbytes;
  int typeline;
  size_t lenline;
  int sw;
  int rvalue;
  int linecont = 0;

  /* is the same block? */

  if (*next == lastblock) {
    *status = FALSE;
    return (p);
  }

  //  printf("next: %d lblock:%d  mb:%d\n",*next,lastblock,maxblock);
  *status = TRUE;
  /* the first time check the size of the file */

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

  if ((temp = (double *)malloc((size_t)(df.ncol + 1) * sizeof(double))) ==
      NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for (i = 0; (int)i < df.ncol + 1; i++)
    temp[i] = 0;

  comment = nocomment;

  /* go to the n=next block */

  ////    kEOF=FALSE;    //!!!!!

  if ((*next - lastblock != 1 || maxblock <= lastblock)) {
    if (fseek(df.fp, b->pos, SEEK_SET) == -1)
      printf("ERROR Seeking file\n");
  }

  sw = 0;
  do {
    rbytes = RLine(line, MAX_LINE_LEN, df.fp);
    linecont++;
    //    printf("%d:%s\n",linecont,line);

    typeline = AnalizeLine(line);
    //    printf("type: %d -- %s  (%d)\n",typeline,line,rbytes);
    if (typeline == COMMENT && sw == 0) { /* copy the first comment */
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
      //      printf("%d:%s\n",linecont,line);

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
        /* reallocating memory if it is necesary */
        if ((p = (struct Punto *)realloc(
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
    if ((tabla = (long *)realloc(tabla, (size_t)(max_N + 1) * sizeof(long))) ==
        NULL) {
      perror("realloc");
      exit(EXIT_FAILURE);
    }
    // printf("asignando %ld a tabla. numact: %ld\n",max_N,*numact);
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
} /*-- function ReadBlock  */

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

/*  independent Functions */

int Arguments(int argc, char *argv[], struct Options *opt,
              struct Parametres *par)
/* function Arguments
   Check the validity of command line arguments.
   set in struct Parametres *par the command line arguments.
   set options acording with the command line arguments
   return:
   an error code or
   zero if there no error.
*/
{
  int i;
  char c;
  int sw = 0;
  char *pointer;
  char *endptr = NULL;
  static char str[MAX_WORD_LEN] = "",
              str0[MAX_WORD_LEN] = ""; /* str0[] is used for deleting str[] */
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
} /* --funtion Arguments  */

void SetValues(struct Values val_d, struct Parametres par, struct Options opt,
               struct Values *val) {
  /*
     Set the program values from the data values
   */

  /*    float max_r,min_r; */
  val->max_r = val_d.max_r;
  val->min_r = val_d.min_r;
  if (opt.radio == TRUE) {
    if (par.max_r != 0 || par.min_r != 0) {
      val->max_r = par.max_r;
      val->min_r = par.min_r;
    }
  }

  /*    float max_c,min_c; */
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

  /*    float max_f,min_f; */
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
  /*
     return the dimension of the data file
   */

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
  /*
     return:
     the number of columns given as option -w
   */
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
    } else { /* last enter */
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

  /* Checking values */

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
} //   if(option.column==TRUE)

void OrderOfColumns(struct Options opt, int d, int ncol, int noptc, int *posc,
                    struct PosCol *p) {
  /*
     set in struct PosCol p which represent each column;
     what column represent x,y,z, etc.
   */
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
  /*
     get box values from command line options
     return:
     0 if no error is found
     1 if an error is found in command line -B parameters
   */

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
      } else { /* last element */
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
  /*
     Set the led box values
   */

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
  /*
     set the initial values for the structs options and kp.
   */
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
