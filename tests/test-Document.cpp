#include "Document.h"

int
main (void)
{
  /// getEvent()
  {
    Document *doc;
    Event *evt;

    doc = new Document ();

    // bad format
    evt = doc->getEvent ("__root__");
    g_assert_null (evt);

    // no such event
    evt = doc->getEvent ("__root__@x");
    g_assert_null (evt);

    // presentation
    evt = doc->getEvent ("__root__@lambda");
    g_assert_nonnull (evt);
    g_assert (evt->getId () == "@lambda");

    // attribution
    evt = doc->createEvent (Event::ATTRIBUTION, "__settings__", "x");
    g_assert_nonnull (evt);
    g_assert (doc->getEvent ("__settings__.x") == evt);

    // selection
    evt = doc->createEvent (Event::SELECTION, "__settings__", "RED");
    g_assert_nonnull (evt);
    g_assert (doc->getEvent ("__settings__<RED>") == evt);

    delete doc;
  }

  exit (EXIT_SUCCESS);
}
