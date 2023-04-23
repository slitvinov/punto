void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 GetPixel(SDL_Surface *surface, int x, int y);
void DrawLine_old(SDL_Surface *surface, int x0, int y0, int x1, int y1,
                  Uint32 color);
void DrawLine(SDL_Surface *surface, int x0, int y0, int x1, int y1,
              Uint32 color);
void DrawLine32(SDL_Surface *surf, int x0, int y0, int x1, int y1,
                Uint32 color);
void PutPixelCircle(SDL_Surface *screen, int x, int y, int x0, int y0,
                    double razon, Uint32 color);
void DrawCircle(SDL_Surface *screen, int x0, int y0, int r, Uint32 color);
void DrawCircleold(SDL_Surface *surface, int x0, int y0, int r, Uint32 color);
void FillCircle(SDL_Surface *surface, int x0, int y0, int r, Uint32 color);
void FillCircleold(SDL_Surface *surface, int x0, int y0, int r, Uint32 color);
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
