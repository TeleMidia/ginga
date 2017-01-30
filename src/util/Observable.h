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

#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include "ginga.h"
#include "Observer.h"

GINGA_UTIL_BEGIN

class Observable
{
private:
  set<Observer *> observers;
  pthread_mutex_t vM;

public:
  virtual ~Observable ()
  {
    pthread_mutex_lock (&vM);
    observers.clear ();
    pthread_mutex_unlock (&vM);
    pthread_mutex_destroy (&vM);
  };

protected:
  void
  createObserversVector ()
  {
    pthread_mutex_init (&vM, NULL);
    observers.clear ();
  };

public:
  void
  addObserver (Observer *object)
  {
    pthread_mutex_lock (&vM);
    observers.insert (object);
    pthread_mutex_unlock (&vM);
  };

  void
  removeObserver (Observer *object)
  {
    set<Observer *>::iterator i;

    pthread_mutex_lock (&vM);
    i = observers.find (object);
    if (i != observers.end ())
      {
        observers.erase (i);
      }
    pthread_mutex_unlock (&vM);
  };

  virtual void
  notifyObservers (const void *object)
  {
    set<Observer *>::iterator i;

    pthread_mutex_lock (&vM);
    i = observers.begin ();
    while (i != observers.end ())
      {
        (*i)->update (this, object);
        ++i;
      }
    pthread_mutex_unlock (&vM);
  };
};

GINGA_UTIL_END

#endif /* OBSERVABLE_H */
