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

int ParseEvent(SDL_Event * event);
int CheckEvent(SDL_Event * event);
int NextEvent(SDL_Event * event);
int CleanEvent();
int EventLoop(SDL_Event event, struct Window *w, struct Keys *);
