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

#ifndef BASE_H
#define BASE_H

GINGA_NAMESPACE_BEGIN

class Base
{
public:
  Base (Context *);
  virtual ~Base ();

  const set<Object *> *getObjects ();
  Object *getObjectById (const string &);
  Object *getObjectByIdOrAlias (const string &);
  void addObject (Object *);

  Context *getRoot ();
  MediaSettings *getSettings ();
  const set<Media *> *getMedias ();

private:
  set<Object *> _objects;                    // all objects
  Context *_root;                            // root object
  MediaSettings *_settings;                  // settings object
  set<Media *> _medias;                      // media objects
};

GINGA_NAMESPACE_END

#endif // BASE_H
