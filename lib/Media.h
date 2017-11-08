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

#ifndef MEDIA_H
#define MEDIA_H

#include "Object.h"
#include "player/Player.h"

GINGA_NAMESPACE_BEGIN

class Media: public Object
{
public:
  Media (const string &, const string &, const string &);
  virtual ~Media ();

  // Object:
  void setProperty (const string &, const string &,
                    GingaTime dur=0) override;
  void sendKeyEvent (const string &, bool) override;
  void sendTickEvent (GingaTime, GingaTime, GingaTime) override;
  bool startTransition (Event *, Event::Transition) override;
  void endTransition (Event *, Event::Transition) override;

  // Media:
  virtual bool isFocused ();
  virtual bool getZ (int *, int *);
  virtual void redraw (cairo_t *);

protected:
  string _mime;                 // mime-type
  string _uri;                  // content URI
  Player *_player;              // underlying player

  void doStop () override;
};

GINGA_NAMESPACE_END

#endif // MEDIA_H
