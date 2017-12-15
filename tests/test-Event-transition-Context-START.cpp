/* Copyright (Ctx) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "Event.h"
#include "Media.h"
#include "Context.h"
#include "Parser.h"

#define PARSE_AND_START(fmt, doc, str)                                     \
  G_STMT_START                                                             \
  {                                                                        \
    string buf = str;                                                      \
    string errmsg;                                                         \
    *fmt = new Formatter (0, nullptr, nullptr);                            \
    g_assert_nonnull (*fmt);                                               \
    if (!(*fmt)->start (buf.c_str (), buf.length (), &errmsg))             \
      {                                                                    \
        g_printerr ("*** Unexpected error: %s", errmsg.c_str ());          \
        g_assert_not_reached ();                                           \
      }                                                                    \
    *doc = (*fmt)->getDocument ();                                         \
    g_assert_nonnull (*doc);                                               \
  }                                                                        \
  G_STMT_END

int
main (void)
{

  // Presentation events ---------------------------------------------------

  // @lambda: START from state OCCURRING.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='p1' component='ctx'/>\n\
  <context id='ctx'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Event *root_lambda = doc->getRoot ()->getLambda ();
    g_assert_nonnull (root_lambda);
    Context *ctx = cast (Context *, doc->getObjectById ("ctx"));
    g_assert_nonnull (ctx);
    Event *ctx_lambda = ctx->getLambda ();
    g_assert_nonnull (ctx_lambda);
    auto iter = ctx->getPorts ()->begin ();
    Event *ctx_port1 = *iter;
    g_assert_nonnull (ctx_port1);
    iter++;
    Event *ctx_port2 = *iter;
    g_assert_nonnull (ctx_port2);
    Event *ctx_p1 = ctx->getAttributionEvent ("p1");

    // --------------------------------
    // check start document

    // when start the document, only the lambda@root is OCCURING
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::SLEEPING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // after advance time, lambda@ctx is OCCURRING, and its
    // anchors and properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // after advance time, lambda@ctx and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::OCCURRING);
    g_assert (ctx_port2->getState () == Event::OCCURRING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // START is not done and return false
    g_assert_false (ctx_lambda->transition (Event::START));

    // after start, lambda@ctx and its anchors
    // are OCCURRING, and its properties are SLEEPING
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::OCCURRING);
    g_assert (ctx_port2->getState () == Event::OCCURRING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: START from state PAUSED.
  {
    Formatter *fmt;
    Document *doc;
    PARSE (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <context id='c'/>\n\
</body>\n\
</ncl>");

    Context *c = cast (Context *, doc->getObjectById ("c"));
    g_assert_nonnull (c);

    Event *lambda = c->getLambda ();
    g_assert_nonnull (lambda);

    g_assert (lambda->getState () == Event::SLEEPING);
    g_assert_true (lambda->transition (Event::START));
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert_true (lambda->transition (Event::PAUSE));
    g_assert (lambda->getState () == Event::PAUSED);

    // Main check
    g_assert_true (lambda->transition (Event::START));
    g_assert (lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // @lambda: START from state SLEEPING.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='p1' component='ctx'/>\n\
  <context id='ctx'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Event *root_lambda = doc->getRoot ()->getLambda ();
    g_assert_nonnull (root_lambda);
    Context *ctx = cast (Context *, doc->getObjectById ("ctx"));
    g_assert_nonnull (ctx);
    Event *ctx_lambda = ctx->getLambda ();
    g_assert_nonnull (ctx_lambda);
    auto iter = ctx->getPorts ()->begin ();
    Event *ctx_port1 = *iter;
    g_assert_nonnull (ctx_port1);
    iter++;
    Event *ctx_port2 = *iter;
    g_assert_nonnull (ctx_port2);
    Event *ctx_p1 = ctx->getAttributionEvent ("p1");

    // --------------------------------
    // check start document

    // when start the document, only the lambda@root is OCCURING
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::SLEEPING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // after advance time, lambda@ctx is OCCURRING, and its
    // anchors and properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // after advance time, lambda@ctx and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::OCCURRING);
    g_assert (ctx_port2->getState () == Event::OCCURRING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // STOP is done and return true
    g_assert (ctx_lambda->transition (Event::STOP));

    // after STOP time,lambda@ctx and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (ctx_lambda->getState () == Event::SLEEPING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // START is done and return true
    g_assert_true (ctx_lambda->transition (Event::START));

    // after START, lambda@ctx is SLEEPING and its anchors
    // and properties are SLEEPING
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // after advance time, lambda@ctx and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::OCCURRING);
    g_assert (ctx_port2->getState () == Event::OCCURRING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // Attribution events ----------------------------------------------------

  // START from state SLEEPING.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='p1' component='ctx'/>\n\
  <context id='ctx'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Event *root_lambda = doc->getRoot ()->getLambda ();
    g_assert_nonnull (root_lambda);
    Context *ctx = cast (Context *, doc->getObjectById ("ctx"));
    g_assert_nonnull (ctx);
    Event *ctx_lambda = ctx->getLambda ();
    g_assert_nonnull (ctx_lambda);
    auto iter = ctx->getPorts ()->begin ();
    Event *ctx_port1 = *iter;
    g_assert_nonnull (ctx_port1);
    iter++;
    Event *ctx_port2 = *iter;
    g_assert_nonnull (ctx_port2);
    Event *ctx_p1 = ctx->getAttributionEvent ("p1");

    // --------------------------------
    // check start document

    // when start the document, only the lambda@root is OCCURING
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::SLEEPING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // after advance time, lambda@ctx is OCCURRING, and its
    // anchors and properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::SLEEPING);
    g_assert (ctx_port2->getState () == Event::SLEEPING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // after advance time, lambda@ctx and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_lambda->getState () == Event::OCCURRING);
    g_assert (ctx_port1->getState () == Event::OCCURRING);
    g_assert (ctx_port2->getState () == Event::OCCURRING);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // START AttributionEvent is done and return true
    g_assert (ctx_p1->setParameter ("value", "1"));
    g_assert_true (ctx_p1->transition (Event::START));

    // after START AttributionEvent is OCCURRING
    g_assert (ctx_p1->getState () == Event::OCCURRING);
    g_assert (ctx->getProperty ("p1") == "1");

    // when advance time AttributionEvent is SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (ctx_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
