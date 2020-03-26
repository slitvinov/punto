#include <string.h>
#include <math.h>
#include "punto.h"
#include "SDL.h"
#include "graphics-sdl.h"
#include "file.h"


void ReadRGBColors(char *fname,struct RGBColor *color,int n);
int LookUpColor(char *name,struct RGBColor *table,SDL_Color *color);
int Led1(SDL_Surface *screen,Uint32 color,int x,int y,int n);
void DrawArrow(SDL_Surface *screen,SDL_Rect pos,struct Point f,float factor,Uint32 color,int type);
void DrawScale(SDL_Surface *screen,Uint32 *color,Uint32 border,struct Values,int x,int y);
void DrawFile(SDL_Surface *screen,Uint32 color,Uint32 border,struct Rectangle rec,struct DataFile);
void GenColorTable(SDL_Surface *screen,Uint32 *colortable,int type);
void GenMiniColorTable(SDL_Surface *screen,Uint32 *ct ,int n,SDL_Color color1,SDL_Color color2);
void WriteBitmap(SDL_Surface *bitmap,char *);
void DrawBox(struct Window w,struct Punto *box,int dim,struct Point c_v,float z,Uint32 color);
void InfoColors(struct RGBColor *rgbcolortable);
