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
#include <stdio.h>
// sleep thread
#include <chrono>
#include <thread>
// measure time
#include <iostream>
#include <ctime>

void
bin (unsigned short v, int number[], int size)
{
  for (; size - 1 >= 0; size--)
    number[size - 1] = (v >> (size - 1)) & 1;
}

int
main (void)
{
  string path;
  Document *doc;
  Formatter *fmt;
  string errmsg = "";

  int const size = 16;

  int number[size];

  auto start = std::chrono::system_clock::now ();

  path = xpathbuildabs (
      ABS_TOP_SRCDIR, "tests-ncl/test-siggen-bit-13freq-150hz.ncl");

  fmt = new Formatter (nullptr);
  g_assert_nonnull (fmt);
  fmt->start (path, &errmsg);
  doc = fmt->getDocument ();
  g_assert_nonnull (doc);

  short dataToSend[]
      = { 1, 7, 5, 4, 19, 1 }; // highest number should be 2^13-1 = 4095

  fmt->sendTick (4 * GINGA_SECOND, 4 * GINGA_SECOND, 0);

  Context *body = cast (Context *, doc->getRoot ());
  g_assert_nonnull (body);

  Media *m1 = cast (Media *, body->getChildById ("m"));
  g_assert_nonnull (m1);
  Media *m2 = cast (Media *, body->getChildById ("m2"));
  g_assert_nonnull (m2);
  Media *m3 = cast (Media *, body->getChildById ("m3"));
  g_assert_nonnull (m3);
  Media *m4 = cast (Media *, body->getChildById ("m4"));
  g_assert_nonnull (m4);
  Media *m5 = cast (Media *, body->getChildById ("m5"));
  g_assert_nonnull (m5);
  Media *m6 = cast (Media *, body->getChildById ("m6"));
  g_assert_nonnull (m6);
  Media *m7 = cast (Media *, body->getChildById ("m7"));
  g_assert_nonnull (m7);
  Media *m8 = cast (Media *, body->getChildById ("m8"));
  g_assert_nonnull (m8);
  Media *m9 = cast (Media *, body->getChildById ("m9"));
  g_assert_nonnull (m9);
  Media *m10 = cast (Media *, body->getChildById ("m10"));
  g_assert_nonnull (m10);
  Media *m11 = cast (Media *, body->getChildById ("m11"));
  g_assert_nonnull (m11);
  Media *m12 = cast (Media *, body->getChildById ("m12"));
  g_assert_nonnull (m12);
  // Media *m13 = cast (Media *, body->getChildById ("m13"));
  // g_assert_nonnull (m13);

  for (int t = 0; t < size; t++)
    {
      number[t] = 0;
    }

  for (int i = 0; i < sizeof (dataToSend) / sizeof (dataToSend[0]); i++)
    {
      printf ("number: %d\n", dataToSend[i]);
      bin (dataToSend[i], number, size);

      for (int t = 0; t < size; t++)
        {
          printf ("%d", number[t]);
        }

      printf ("\n");

      if (number[0])
        m1->setProperty ("volume", "0.5");
      else
        m1->setProperty ("volume", "0");
      if (number[1])
        m2->setProperty ("volume", "0.5");
      else
        m2->setProperty ("volume", "0");
      if (number[2])
        m3->setProperty ("volume", "0.5");
      else
        m3->setProperty ("volume", "0");
      if (number[3])
        m4->setProperty ("volume", "0.5");
      else
        m4->setProperty ("volume", "0");
      if (number[4])
        m5->setProperty ("volume", "0.5");
      else
        m5->setProperty ("volume", "0");
      if (number[5])
        m6->setProperty ("volume", "0.5");
      else
        m6->setProperty ("volume", "0");
      if (number[6])
        m7->setProperty ("volume", "0.5");
      else
        m7->setProperty ("volume", "0");
      if (number[7])
        m8->setProperty ("volume", "0.5");
      else
        m8->setProperty ("volume", "0");
      if (number[8])
        m9->setProperty ("volume", "0.5");
      else
        m9->setProperty ("volume", "0");
      if (number[9])
        m10->setProperty ("volume", "0.5");
      else
        m10->setProperty ("volume", "0");
      if (number[10])
        m11->setProperty ("volume", "0.5");
      else
        m11->setProperty ("volume", "0");
      if (number[11])
        m12->setProperty ("volume", "0.5");
      else
        m12->setProperty ("volume", "0");
      // if (number[13]) m13->setProperty ("volume", "0.5"); else
      // m13->setProperty ("volume", "0");

      std::this_thread::sleep_for (std::chrono::milliseconds (300));

      for (int t = 0; t < size; t++)
        {
          number[t] = 0;
        }

      m1->setProperty ("volume", "0");
      m2->setProperty ("volume", "0");
      m3->setProperty ("volume", "0");
      m4->setProperty ("volume", "0");
      m5->setProperty ("volume", "0");
      m6->setProperty ("volume", "0");
      m7->setProperty ("volume", "0");
      m8->setProperty ("volume", "0");
      m9->setProperty ("volume", "0");
      m10->setProperty ("volume", "0");
      m11->setProperty ("volume", "0");
      m12->setProperty ("volume", "0");

      std::this_thread::sleep_for (std::chrono::milliseconds (50));
    }

  auto end = std::chrono::system_clock::now ();
  std::chrono::duration<double> elapsed_seconds = end - start;

  std::cout << "elapsed time: " << elapsed_seconds.count () << "s\n";

  while (true)
    ;
}