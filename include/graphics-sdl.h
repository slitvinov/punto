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

#include "SDL.h"

void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 GetPixel(SDL_Surface *surface, int x, int y);
void DrawLine_old(SDL_Surface *surface, int x0, int y0, int x1,int y1,Uint32 color);
void DrawLine(SDL_Surface *surface, int x0, int y0, int x1,int y1,Uint32 color);
void DrawLine32(SDL_Surface *surf, int x0, int y0, int x1, int y1, Uint32 color);
void PutPixelCircle(SDL_Surface *screen,int x,int y,int x0,int y0,double razon,Uint32 color);
void DrawCircle(SDL_Surface *screen, int x0, int y0, int r,Uint32 color);
void DrawCircleold(SDL_Surface *surface, int x0, int y0, int r,Uint32 color);
void FillCircle(SDL_Surface *surface, int x0, int y0, int r,Uint32 color);
void FillCircleold(SDL_Surface *surface, int x0, int y0, int r,Uint32 color);
SDL_Surface *CreateBall(SDL_Surface *screen,Uint32 color,Uint32 bcolor, int radio);
SDL_Surface * CreateBall3D(SDL_Surface *screen,Uint32 color, int radio);
Uint32 FastestFlags(Uint32 flags, int width, int height, int bpp);
void DrawRectangle(SDL_Surface *surface, int x0, int y0, int x1,int y1,Uint32 color);
void FillRectangle(SDL_Surface *surface, int x0, int y0, int x1,int y1,Uint32 color);
SDL_Surface *CreateRectangle(SDL_Surface *screen,Uint32 color,Uint32 bcolor, int radio);
int DrawText(SDL_Surface *screen,char *text,int x,int y,Uint32 color);
int DrawChar(SDL_Surface *screen,char **text,int x,int y,Uint32 color);
SDL_Surface *MakeIcon(SDL_Surface *screen,SDL_Surface *icon,Uint32 color);

int Sqrt(int x);

