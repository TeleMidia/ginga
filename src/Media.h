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

#ifndef GINGA_MEDIA_H
#define GINGA_MEDIA_H

#include "Object.h"
#include "Player.h"

GINGA_NAMESPACE_BEGIN

class Media : public Object
{
public:

  /// Creates a new media object.
  Media (Document *doc, const string &id);

  /// Destroys media object.
  ~Media ();

  /// Tests whether media object is focused.
  bool isFocused ();

protected:

  /// Gets the player associated with media object (if any).
  Player *_getPlayer ();


  // TODO ------------------------------------------------------------------
public:
  void sendKey (const string &, bool);

};

GINGA_NAMESPACE_END

#endif // GINGA_MEDIA_H
