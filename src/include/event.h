int ParseEvent(SDL_Event * event);
int CheckEvent(SDL_Event * event);
int NextEvent(SDL_Event * event);
int CleanEvent();
int EventLoop(SDL_Event event, struct Window *w, struct Keys *);
