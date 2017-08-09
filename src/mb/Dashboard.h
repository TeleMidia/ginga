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

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "ginga-internal.h"
#include "IEventListener.h"

GINGA_MB_BEGIN

class Dashboard : public IEventListener
{
public:
  Dashboard ();
  ~Dashboard ();
  void redraw2 (cairo_t *);

  // IEventListener.
  void handleTickEvent (GingaTime, GingaTime, int) override;
  void handleKeyEvent (const string &, bool) override {};

private:
  GingaTime _total;
  GingaTime _diff;
  int _frameno;
};

GINGA_MB_END

#endif // DASHBOARD_H
