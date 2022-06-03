#include <math.h>
#include <SDL.h>
#include "include/graphics-sdl.h"
#include "include/fonts-1.xpm"

#define COS45 .70710678118654752440
#define MAX_WORD_LEN 128

void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
{
  Uint8 bpp = surface->format->BytesPerPixel;

  /* Here p is the address to the pixel we want to set */
  Uint8 *p;

  if (x < 0)
    return;
  if (y < 0)
    return;
  if (x >= surface->w)
    return;
  if (y >= surface->h)
    return;

  /*    if (SDL_MUSTLOCK(surface)) { */
  /*      if (SDL_LockSurface(surface) < 0) { */
  /*        printf("Error locking surface: %s\n", SDL_GetError()); */
  /*        abort(); */
  /*      } */
  /*    } */

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
  /* Unlock the surface. */
  //  SDL_UnlockSurface(surface);
}

Uint32 GetPixel(SDL_Surface *surface, int x, int y)
/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
{
  Uint8 bpp = surface->format->BytesPerPixel;

  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
  case 1:
    return *p;

  case 2:
    return *(Uint16 *)p;

  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;

  case 4:
    return *(Uint32 *)p;

  default:
    return 0; /* shouldn't happen, but avoids warnings */
  }
}

void DrawLine(SDL_Surface *surface, int x0, int y0, int x1, int y1,
              Uint32 color) {
  /*
     Draw a line that starts at x0,y0 and ends at x1,y1
     of color color
   */

  int t, distancia;
  int xerr = 0, yerr = 0, d_x, d_y;
  int incx, incy;

  /*    if (SDL_MUSTLOCK(surface)) {  */
  /*      if (SDL_LockSurface(surface) < 0) {  */
  /*        printf("Error locking surface: %s\n", SDL_GetError());  */
  /*        abort();  */
  /*      }  */
  /*    }  */

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
  /* Unlock the surface. */
  /*    SDL_UnlockSurface(surface); */
}

void DrawLine32(SDL_Surface *surf, int x0, int y0, int x1, int y1,
                Uint32 color) {
  /*
     Draw a line that starts at x0,y0 and ends at x1,y1
     of color color
   */

  Uint16 *buffer;
  int drawpos;
  int xspan, yspan;
  int xinc, yinc;
  int sum;
  int i;

  /* If we need to lock this surface before drawing pixels, do so. */
  if (SDL_MUSTLOCK(surf)) {
    if (SDL_LockSurface(surf) < 0) {
      printf("Error locking surface: %s\n", SDL_GetError());
      abort();
    }
  }

  /* Get the surface's data pointer. */
  buffer = (Uint16 *)surf->pixels;

  /* Calculate the x and y spans of the line. */
  xspan = x1 - x0 + 1;
  yspan = y1 - y0 + 1;

  /* Figure out the correct increment for the major axis.
     Account for negative spans (x1 < x0, for instance). */
  if (xspan < 0) {
    xinc = -1;
    xspan = -xspan;
  } else
    xinc = 1;

  if (yspan < 0) {
    yinc = (int)(-surf->pitch / 2);
    yspan = -yspan;
  } else
    yinc = (int)(surf->pitch / 2);

  i = 0;
  sum = 0;

  /* This is our current offset into the buffer. We use this
     variable so that we don't have to calculate the offset at
     each step; we simply increment this by the correct amount.

     Instead of adding 1 to the x coordinate, we add one to drawpos.
     Instead of adding 1 to the y coordinate, we add the surface's
     pitch (scanline width) to drawpos. */
  drawpos = (int)(surf->pitch / 2 * y0) + x0;

  /* Our loop will be different depending on the
     major axis. */
  if (xspan < yspan) {

    /* Loop through each pixel along the major axis. */
    for (i = 0; i < yspan; i++) {

      /* Draw the pixel. */
      buffer[drawpos] = color;

      /* Update the incremental division. */
      sum += xspan;

      /* If we've reached the dividend, advance
         and reset. */
      if (sum >= yspan) {
        drawpos += xinc;
        sum -= yspan;
      }

      /* Increment the drawing position. */
      drawpos += yinc;
    }
  } else {

    /* See comments above. This code is equivalent. */
    for (i = 0; i < xspan; i++) {

      buffer[drawpos] = color;
      sum += yspan;
      if (sum >= xspan) {
        drawpos += yinc;
        sum -= xspan;
      }
      drawpos += xinc;
    }
  }

  /* Unlock the surface. */
  SDL_UnlockSurface(surf);
}

void PutPixelCircle(SDL_Surface *screen, int x, int y, int x0, int y0,
                    double razon, Uint32 color) {
  /*

   */
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
  /*
     Draw a circle
   */
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
  /*
     Draw a ring of center x0,y0 and radius r,h
     with color color
   */
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
  /*
     Fill a circlering of center x0,y0 and radio r
     with color color
   */

  int x, y;
  float inct;
  float r2;

  //  if(r<=0)return;
  inct = (1. / (float)r);
  r2 = (float)r * r;

  //  DrawCircle(surface,x0,y0,r,color);
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
  /*
     Create  a circle bitmap
     of radio radio
     of color color and
     border color bcolor
   */
  SDL_Surface *temp;
  Uint32 background;

  temp = SDL_CreateRGBSurface(SDL_HWSURFACE, 2 * radio + 1, 2 * radio + 1,
                              (int)screen->format->BitsPerPixel, 0, 0, 0, 0);

  background = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
  SDL_FillRect(temp, NULL, background);
  /* Set transparent pixel as the pixel at (0,0) */
  SDL_SetColorKey(temp, (Uint32)(SDL_SRCCOLORKEY | SDL_RLEACCEL),
                  *(Uint8 *)temp->pixels);

  //  color = SDL_MapRGB(temp->format, 255, 0, 0);
  FillCircle(temp, radio, radio, radio, color);
  DrawCircle(temp, radio, radio, radio, bcolor);

  return temp;
}

SDL_Surface *CreateBall3D(SDL_Surface *screen, Uint32 color, int radio) {
  /*
     Create a 3D sphere bitmap
     of radio radio
     of color color
   */

  SDL_Surface *temp;
  SDL_Surface *sprite;
  Uint32 background;
  float i;
  Uint8 red, green, blue;
  float ir, ig, ib;
  Uint8 r, g, b;
  float inci;

  //  printf("icon in 1: %p\n",sprite);
  temp = SDL_CreateRGBSurface(SDL_HWSURFACE, 2 * radio + 1, 2 * radio + 1,
                              (int)screen->format->BitsPerPixel, 0, 0, 0, 0);
  if (temp == NULL) {
    fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  background = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
  SDL_FillRect(temp, NULL, background);
  /* Set transparent pixel as the pixel at (0,0) */
  if (screen->format->palette) {
    SDL_SetColorKey(temp, (Uint32)(SDL_SRCCOLORKEY | SDL_RLEACCEL),
                    *(Uint8 *)temp->pixels);
  }
  SDL_SetColorKey(temp, (Uint32)(SDL_SRCCOLORKEY | SDL_RLEACCEL),
                  *(Uint8 *)temp->pixels);

  SDL_GetRGB(color, screen->format, &red, &green, &blue);
  r = red;
  g = green;
  b = blue;
  ir = ((float)red / radio) * .4;
  ig = ((float)green / radio) * .4;
  ib = ((float)blue / radio) * .4;

  if (radio < 2) {
    color = SDL_MapRGB(screen->format, r, g, b);
    FillCircle(temp, radio, radio, radio, color);
  } else {
    inci = 1;
    //  if(radio>32)inci=(float)radio/32.;
    inci = (float)radio / 20.;
    if (inci < 1)
      inci = 1;
    for (i = (float)radio; i > 0; i -= inci) {
      r = red - ir * i;
      g = green - ig * i;
      b = blue - ib * i;
      color = SDL_MapRGB(screen->format, r, g, b);
      FillRing(temp, radio, radio, (int)(i), (int)(inci * 1.4142 + .5) + 1,
               color);
    }
  }

  sprite = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  if (sprite == NULL) {
    fprintf(stderr, "Couldn't convert background: %s\n", SDL_GetError());
    return (NULL);
  }
  return (sprite);
}

void DrawRectangle(SDL_Surface *surface, int x0, int y0, int x1, int y1,
                   Uint32 color) {
  /*
     Draw a rectangle
   */
  DrawLine(surface, x0, y0, x1, y0, color);
  DrawLine(surface, x0, y0, x0, y1, color);
  DrawLine(surface, x0, y1, x1, y1, color);
  DrawLine(surface, x1, y1, x1, y0, color);
}

void FillRectangle(SDL_Surface *surface, int x0, int y0, int x1, int y1,
                   Uint32 color) {
  /*
     fill a rectangle with color color
   */
  int y;

  for (y = y0; y <= y1; y++) {
    DrawLine(surface, x0, y, x1, y, color);
  }
}

SDL_Surface *CreateRectangle(SDL_Surface *screen, Uint32 color, Uint32 bcolor,
                             int radio) {
  /*
     Create a rectangle bitmap
  */
  SDL_Surface *temp;
  Uint32 background;

  temp = SDL_CreateRGBSurface(SDL_SWSURFACE, radio, radio,
                              (int)screen->format->BitsPerPixel, 0, 0, 0, 0);

  background = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
  SDL_FillRect(temp, NULL, background);

  //  color = SDL_MapRGB(temp->format, 255, 0, 0);
  FillRectangle(temp, 0, 0, radio, radio, color);
  DrawRectangle(temp, 0, 0, radio - 1, radio - 1, bcolor);

  return temp;
}

int DrawText(SDL_Surface *screen, char *text, int x0, int y0, Uint32 color) {
  /*
     Draw the text pointer by *text
     in the position x0,y0
     with the color color
   */
  size_t i, len;
  int x;

  len = strlen(text);
  for (i = 0; i < len; i++) {
    switch (text[i]) {
    case ' ':
      x = DrawChar(screen, font_NULL, x0, y0, color);
      break;
    case '!':
      x = DrawChar(screen, font_EXCLAIM, x0, y0, color);
      break;
    case '\\':
      x = DrawChar(screen, font_BACKSLASH, x0, y0, color);
      break;
    case '"':
      x = DrawChar(screen, font_QUOTEDBL, x0, y0, color);
      break;
    case '#':
      x = DrawChar(screen, font_HASH, x0, y0, color);
      break;
    case '$':
      x = DrawChar(screen, font_DOLLAR, x0, y0, color);
      break;
    case '%':
      x = DrawChar(screen, font_CENT, x0, y0, color);
      break;
    case '&':
      x = DrawChar(screen, font_AMPERSAND, x0, y0, color);
      break;
    case '\'':
      x = DrawChar(screen, font_QUOTE, x0, y0, color);
      break;
    case '(':
      x = DrawChar(screen, font_LEFTPAREN, x0, y0, color);
      break;
    case ')':
      x = DrawChar(screen, font_RIGHTPAREN, x0, y0, color);
      break;
    case '*':
      x = DrawChar(screen, font_ASTERISK, x0, y0, color);
      break;
    case '+':
      x = DrawChar(screen, font_PLUS, x0, y0, color);
      break;
    case ',':
      x = DrawChar(screen, font_COMMA, x0, y0, color);
      break;
    case '-':
      x = DrawChar(screen, font_MINUS, x0, y0, color);
      break;
    case '.':
      x = DrawChar(screen, font_DOT, x0, y0, color);
      break;
    case '/':
      x = DrawChar(screen, font_SLASH, x0, y0, color);
      break;
    case '0':
      x = DrawChar(screen, font_0, x0, y0, color);
      break;
    case '1':
      x = DrawChar(screen, font_1, x0, y0, color);
      break;
    case '2':
      x = DrawChar(screen, font_2, x0, y0, color);
      break;
    case '3':
      x = DrawChar(screen, font_3, x0, y0, color);
      break;
    case '4':
      x = DrawChar(screen, font_4, x0, y0, color);
      break;
    case '5':
      x = DrawChar(screen, font_5, x0, y0, color);
      break;
    case '6':
      x = DrawChar(screen, font_6, x0, y0, color);
      break;
    case '7':
      x = DrawChar(screen, font_7, x0, y0, color);
      break;
    case '8':
      x = DrawChar(screen, font_8, x0, y0, color);
      break;
    case '9':
      x = DrawChar(screen, font_9, x0, y0, color);
      break;
    case ':':
      x = DrawChar(screen, font_COLON, x0, y0, color);
      break;
    case ';':
      x = DrawChar(screen, font_SEMICOLON, x0, y0, color);
      break;
    case '<':
      x = DrawChar(screen, font_LESS, x0, y0, color);
      break;
    case '=':
      x = DrawChar(screen, font_EQUALS, x0, y0, color);
      break;
    case '>':
      x = DrawChar(screen, font_GREATER, x0, y0, color);
      break;
    case '?':
      x = DrawChar(screen, font_QUESTION, x0, y0, color);
      break;
    case '@':
      x = DrawChar(screen, font_AT, x0, y0, color);
      break;
    case 'A':
      x = DrawChar(screen, font_A, x0, y0, color);
      break;
    case 'B':
      x = DrawChar(screen, font_B, x0, y0, color);
      break;
    case 'C':
      x = DrawChar(screen, font_C, x0, y0, color);
      break;
    case 'D':
      x = DrawChar(screen, font_D, x0, y0, color);
      break;
    case 'E':
      x = DrawChar(screen, font_E, x0, y0, color);
      break;
    case 'F':
      x = DrawChar(screen, font_F, x0, y0, color);
      break;
    case 'G':
      x = DrawChar(screen, font_G, x0, y0, color);
      break;
    case 'H':
      x = DrawChar(screen, font_H, x0, y0, color);
      break;
    case 'I':
      x = DrawChar(screen, font_I, x0, y0, color);
      break;
    case 'J':
      x = DrawChar(screen, font_J, x0, y0, color);
      break;
    case 'K':
      x = DrawChar(screen, font_K, x0, y0, color);
      break;
    case 'L':
      x = DrawChar(screen, font_L, x0, y0, color);
      break;
    case 'M':
      x = DrawChar(screen, font_M, x0, y0, color);
      break;
    case 'N':
      x = DrawChar(screen, font_N, x0, y0, color);
      break;
    case 'O':
      x = DrawChar(screen, font_O, x0, y0, color);
      break;
    case 'P':
      x = DrawChar(screen, font_P, x0, y0, color);
      break;
    case 'Q':
      x = DrawChar(screen, font_Q, x0, y0, color);
      break;
    case 'R':
      x = DrawChar(screen, font_R, x0, y0, color);
      break;
    case 'S':
      x = DrawChar(screen, font_S, x0, y0, color);
      break;
    case 'T':
      x = DrawChar(screen, font_T, x0, y0, color);
      break;
    case 'U':
      x = DrawChar(screen, font_U, x0, y0, color);
      break;
    case 'V':
      x = DrawChar(screen, font_V, x0, y0, color);
      break;
    case 'W':
      x = DrawChar(screen, font_W, x0, y0, color);
      break;
    case 'X':
      x = DrawChar(screen, font_X, x0, y0, color);
      break;
    case 'Y':
      x = DrawChar(screen, font_Y, x0, y0, color);
      break;
    case 'Z':
      x = DrawChar(screen, font_Z, x0, y0, color);
      break;
    case '[':
      x = DrawChar(screen, font_LEFTBRACKET, x0, y0, color);
      break;
    case ']':
      x = DrawChar(screen, font_RIGHTBRACKET, x0, y0, color);
      break;
    case '^':
      x = DrawChar(screen, font_CARET, x0, y0, color);
      break;
    case '_':
      x = DrawChar(screen, font_UNDERSCORE, x0, y0, color);
      break;
    case '`':
      x = DrawChar(screen, font_BACKQUOTE, x0, y0, color);
      break;
    case 'a':
      x = DrawChar(screen, font_a, x0, y0, color);
      break;
    case 'b':
      x = DrawChar(screen, font_b, x0, y0, color);
      break;
    case 'c':
      x = DrawChar(screen, font_c, x0, y0, color);
      break;
    case 'd':
      x = DrawChar(screen, font_d, x0, y0, color);
      break;
    case 'e':
      x = DrawChar(screen, font_e, x0, y0, color);
      break;
    case 'f':
      x = DrawChar(screen, font_f, x0, y0, color);
      break;
    case 'g':
      x = DrawChar(screen, font_g, x0, y0, color);
      break;
    case 'h':
      x = DrawChar(screen, font_h, x0, y0, color);
      break;
    case 'i':
      x = DrawChar(screen, font_i, x0, y0, color);
      break;
    case 'j':
      x = DrawChar(screen, font_j, x0, y0, color);
      break;
    case 'k':
      x = DrawChar(screen, font_k, x0, y0, color);
      break;
    case 'l':
      x = DrawChar(screen, font_l, x0, y0, color);
      break;
    case 'm':
      x = DrawChar(screen, font_m, x0, y0, color);
      break;
    case 'n':
      x = DrawChar(screen, font_n, x0, y0, color);
      break;
    case 'o':
      x = DrawChar(screen, font_o, x0, y0, color);
      break;
    case 'p':
      x = DrawChar(screen, font_p, x0, y0, color);
      break;
    case 'q':
      x = DrawChar(screen, font_q, x0, y0, color);
      break;
    case 'r':
      x = DrawChar(screen, font_r, x0, y0, color);
      break;
    case 's':
      x = DrawChar(screen, font_s, x0, y0, color);
      break;
    case 't':
      x = DrawChar(screen, font_tt, x0, y0, color);
      break;
    case 'u':
      x = DrawChar(screen, font_u, x0, y0, color);
      break;
    case 'v':
      x = DrawChar(screen, font_v, x0, y0, color);
      break;
    case 'w':
      x = DrawChar(screen, font_w, x0, y0, color);
      break;
    case 'x':
      x = DrawChar(screen, font_x, x0, y0, color);
      break;
    case 'y':
      x = DrawChar(screen, font_y, x0, y0, color);
      break;
    case 'z':
      x = DrawChar(screen, font_z, x0, y0, color);
      break;
    case '{':
      x = DrawChar(screen, font_LEFTKEY, x0, y0, color);
      break;
    case '|':
      x = DrawChar(screen, font_OR, x0, y0, color);
      break;
    case '}':
      x = DrawChar(screen, font_RIGHTKEY, x0, y0, color);
      break;
    case '~':
      x = DrawChar(screen, font_WORM, x0, y0, color);
      break;
    default:
      x = DrawChar(screen, font_NONE, x0, y0, color);
      break;
    }
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
  /*
    A sqrt() table of the firts 10000 integer numbers
  */
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
