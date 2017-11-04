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

#ifndef FORMATTER_MEDIA_H
#define FORMATTER_MEDIA_H

#include "FormatterObject.h"
#include "player/Player.h"

GINGA_NAMESPACE_BEGIN

class FormatterMedia: public FormatterObject
{
public:
  FormatterMedia (Formatter *, const string &, const string &,
                  const string &);
  virtual ~FormatterMedia ();

  string getProperty (const string &) override;
  void setProperty (const string &, const string &,
                    GingaTime dur=0) override;

  void sendKeyEvent (const string &, bool) override;
  void sendTickEvent (GingaTime, GingaTime, GingaTime) override;
  bool exec (FormatterEvent *, NclEventState, NclEventState,
             NclEventStateTransition) override;

  virtual bool isFocused ();
  virtual bool getZ (int *, int *);
  virtual void redraw (cairo_t *);

protected:
  string _mimetype;              // mime-type
  string _uri;                   // content URI
  Player *_player;               // underlying player

  void doStop () override;
};

GINGA_NAMESPACE_END

#endif // FORMATTER_MEDIA_H
