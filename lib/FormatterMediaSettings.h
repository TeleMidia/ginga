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

#ifndef FORMATTER_MEDIA_SETTINGS_H
#define FORMATTER_MEDIA_SETTINGS_H

#include "FormatterMedia.h"

GINGA_NAMESPACE_BEGIN

class FormatterMediaSettings: public FormatterMedia
{
public:
  FormatterMediaSettings (const string &);
  ~FormatterMediaSettings ();

  // FormatterObject:
  void setProperty (const string &, const string &, GingaTime) override;
  void sendTickEvent (GingaTime, GingaTime, GingaTime) override;

  // FormatterMedia;
  bool isFocused () override;
  bool getZ (int *, int *) override;
  void redraw (cairo_t *) override;

  // FormatterSettings:
  void updateCurrentFocus (const string &);
  void scheduleFocusUpdate (const string &);

 private:
  string _nextFocus;            // next focus index
  bool _hasNextFocus;           // true if a focus update is scheduled
};

GINGA_NAMESPACE_END

#endif
