#include <stdlib.h>
#include <SDL.h>
#include "punto.h"
#include "event.h"

int
ParseEvent(SDL_Event * event)
{
    int status;

    status = FALSE;

    switch (event->type) {
    case SDL_USEREVENT:
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        /*    case SDL_PRESSED:  */
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_VIDEORESIZE:
    case SDL_QUIT:
        status = TRUE;
        break;

    default:
        break;
    }

    return (status);
}

int
CheckEvent(SDL_Event * event)
{
    int status;

    status = FALSE;
    /*
     * Looking for an interesting event
     */

    while (SDL_PollEvent(NULL) > 0 && status == FALSE) {
        SDL_PollEvent(event);
        status = ParseEvent(event);
    }
    /* deleting the rest of events */
    //  CleanEvent();

    return (status);
}


int
NextEvent(SDL_Event * event)
{

    /*
     *      NextEvent() incorpora algunas funciones de manejo de eventos.
     *      Primero bloquea el programa hasta la llegada de un evento X,
     *      llamando a SDL_WaitEvent().  Luego, NetxEvent() llama a ParseEvent(),
     *      quien o filtra el evento o le deja pasar.
     *
     */

    /* -- funcion NextEvent */
    int status;


    /*
     * Bloqueo hasta que llegue algun evento interesante
     */
    status = FALSE;             /* -- aun no hay eventos interesantes */

    while (status == FALSE) {
        //    SDL_PumpEvents();
        SDL_WaitEvent(event);
        status = ParseEvent(event);
    }
    return (status);
}                               /* -- funcion NextEvent */

int
CleanEvent()
{
    int status;
    SDL_Event event;

    status = FALSE;

    while (SDL_PollEvent(NULL) > 0) {
        SDL_PollEvent(&event);
        status = TRUE;
    }
    return (status);
}

int
EventLoop(SDL_Event event, struct Window *w, struct Keys *k)
{
    //  int width,height;
    int status;

    static int block_a = FALSE;

    k->pressed = FALSE;
    status = FALSE;
    switch (event.type) {

    case SDL_USEREVENT:

        status = TRUE;

        k->user = TRUE;
        break;
    case SDL_VIDEORESIZE:
        w->shift.x += ((double) event.resize.w - w->w) / 2.;
        w->shift.y += ((double) event.resize.h - w->h) / 2.;
        w->w = event.resize.w;
        w->h = event.resize.h;

        w->screen = SDL_SetVideoMode(w->w, w->h, (int) w->bpp, w->flags);
        status = TRUE;

        break;
    case SDL_KEYDOWN:
        //    PrintKey(&event.key.keysym, 1);
        status = TRUE;

        k->shift = FALSE;
        if (event.key.keysym.mod & KMOD_LSHIFT ||
            event.key.keysym.mod & KMOD_RSHIFT) {
            k->shift = TRUE;
            //      status=FALSE;
        }

        switch (event.key.keysym.sym) {
        case SDLK_q:
            /* quits the app... */
            k->q = TRUE;
            break;
        case SDLK_0:
        case SDLK_KP0:
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
        case SDLK_KP7:
        case SDLK_7:
            k->pressed = TRUE;
            k->k_7 = TRUE;
            break;
        case SDLK_8:
        case SDLK_KP8:
            k->pressed = TRUE;
            k->k_8 = TRUE;
            break;
        case SDLK_9:
        case SDLK_KP9:
            k->pressed = TRUE;
            k->k_9 = TRUE;
            break;
        case SDLK_4:
        case SDLK_KP4:
            k->pressed = TRUE;
            k->k_4 = TRUE;
            break;
        case SDLK_5:
        case SDLK_KP5:
            k->pressed = TRUE;
            k->k_5 = TRUE;
            break;
        case SDLK_6:
        case SDLK_KP6:
            k->pressed = TRUE;
            k->k_6 = TRUE;
            break;
        default:
            status = FALSE;
            break;
        }
        break;
    case SDL_KEYUP:
        //  PrintKey(&event.key.keysym, 0);
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
        case SDLK_KP4:
            k->k_4 = FALSE;
            break;
        case SDLK_5:
        case SDLK_KP5:
            k->k_5 = FALSE;
            break;
        case SDLK_6:
        case SDLK_KP6:
            k->k_6 = FALSE;
            break;
        case SDLK_7:
        case SDLK_KP7:
            k->k_7 = FALSE;
            break;
        case SDLK_8:
        case SDLK_KP8:
            k->k_8 = FALSE;
            break;
        case SDLK_9:
        case SDLK_KP9:
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
/*    case SDL_PRESSED: */
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
