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

#include "tests.h"

int
main (void)
{
  list<Object *> objs;

  objs.push_back (new Media ("m"));
  objs.push_back (new MediaSettings ("m"));
  objs.push_back (new Context ("c"));
  objs.push_back (new Switch ("s"));

  for (auto obj : objs)
    {
      size_t n;
      Event *evt;
      Time begin, end;

      n = (instanceof (MediaSettings *, obj)) ? 2 : 1;
      g_assert (obj->getEvents ()->size () == n);

      // Presentation events.
      g_assert_null (obj->getPresentationEvent ("p1"));
      obj->addPresentationEvent ("p1", 0, 0);
      evt = obj->getPresentationEvent ("p1");
      g_assert_nonnull (evt);
      evt->getInterval (&begin, &end);
      g_assert (begin == 0);
      g_assert (end == 0);

      g_assert_null (obj->getPresentationEvent ("p2"));
      obj->addPresentationEvent ("p2", 1, 10);
      evt = obj->getPresentationEvent ("p2");
      g_assert_nonnull (evt);
      evt->getInterval (&begin, &end);
      g_assert (begin == 1);
      g_assert (end == 10);

      g_assert_null (obj->getPresentationEvent ("p3"));
      obj->addPresentationEvent ("p3", GINGA_TIME_NONE, GINGA_TIME_NONE);
      evt = obj->getPresentationEvent ("p3");
      g_assert_nonnull (evt);
      evt->getInterval (&begin, &end);
      g_assert (begin == GINGA_TIME_NONE);
      g_assert (end == GINGA_TIME_NONE);

      g_assert_null (obj->getPresentationEvent ("p4"));
      obj->addPresentationEvent ("p4", "label1");
      evt = obj->getPresentationEvent ("p4");
      g_assert_nonnull (evt);

      g_assert (obj->getEvents ()->size () == n + 4);
      obj->addPresentationEvent ("p1", 0, 0);
      obj->addPresentationEvent ("p2", 0, 0);
      obj->addPresentationEvent ("p3", 0, 0);
      obj->addPresentationEvent ("p4", "label1");
      g_assert (obj->getEvents ()->size () == n + 4);

      // Attribution events.
      g_assert_null (obj->getAttributionEvent ("a1"));
      obj->addAttributionEvent ("a1");
      g_assert_nonnull (obj->getAttributionEvent ("a1"));

      g_assert_null (obj->getAttributionEvent ("a2"));
      obj->addAttributionEvent ("a2");
      g_assert_nonnull (obj->getAttributionEvent ("a2"));

      g_assert_null (obj->getAttributionEvent ("a3"));
      obj->addAttributionEvent ("a3");
      g_assert_nonnull (obj->getAttributionEvent ("a3"));

      g_assert (obj->getEvents ()->size () == n + 7);
      obj->addAttributionEvent ("a1");
      obj->addAttributionEvent ("a2");
      obj->addAttributionEvent ("a3");
      g_assert (obj->getEvents ()->size () == n + 7);

      // Selection events.
      g_assert_null (obj->getSelectionEvent ("s1"));
      obj->addSelectionEvent ("s1");
      g_assert_nonnull (obj->getSelectionEvent ("s1"));

      g_assert_null (obj->getSelectionEvent ("s2"));
      obj->addSelectionEvent ("s2");
      g_assert_nonnull (obj->getSelectionEvent ("s2"));

      g_assert_null (obj->getSelectionEvent ("s3"));
      obj->addSelectionEvent ("s3");
      g_assert_nonnull (obj->getSelectionEvent ("s3"));

      g_assert (obj->getEvents ()->size () == n + 10);
      obj->addSelectionEvent ("s1");
      obj->addSelectionEvent ("s2");
      obj->addSelectionEvent ("s3");
      g_assert (obj->getEvents ()->size () == n + 10);

      delete obj;
    }

  exit (EXIT_SUCCESS);
}
