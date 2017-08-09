/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "ginga_gtk.h"

#include "formatter/Scheduler.h"
using namespace ::ginga::formatter;

#include "mb/Display.h"
using namespace ::ginga::mb;

Scheduler *scheduler = NULL;

void
start_application (void)
{
  printf("Loading file ... %s", gtk_entry_get_text (GTK_ENTRY (ginga_gui.file_entry)) );  
  string file
      = string (gtk_entry_get_text (GTK_ENTRY (ginga_gui.file_entry)));

  _Ginga_Display = new ginga::mb::Display (presentationAttributes.resolutionWidth, presentationAttributes.resolutionHeight, 60,
                                           false, false, create_sdl_window_from_gtk_widget (ginga_gui.canvas)); 

  scheduler = new Scheduler ();
  scheduler->startDocument (file);
  // _Ginga_Display->renderLoop ();

   
}

void
stop_application (void)
{
 // _Ginga_Display->setFullScreen(SDL_WINDOW_FULLSCREEN);
 //  _Ginga_Display->quit();
 //  delete _Ginga_Display;
  // FIXME: This causes the program to crash!
  // delete scheduler;
 // _Ginga_Display = NULL;
}

void
pause_application (void)
{
}