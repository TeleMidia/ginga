/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef MEDIA_REFER_H
#define MEDIA_REFER_H

#include "Media.h"

GINGA_NAMESPACE_BEGIN

class MediaRefer: public Media
{
public:
  MediaRefer (const string &);
  ~MediaRefer ();

  // Object:
  string getObjectTypeAsString () override;
  // void setProperty (const string &, const string &, Time) override;
  void sendTick (Time, Time, Time) override;

  // Media;
  // bool isFocused () override;
  // bool getZ (int *, int *) override;
  // void redraw (cairo_t *) override;

  // MediaRefer:
  void initRefer (Media *);

 private:
  Media *_refer;                // referred media
};

GINGA_NAMESPACE_END

#endif // MEDIA_REFER_H
