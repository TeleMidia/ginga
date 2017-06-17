#include "ginga.h"

#define CHECK_COLOR(c,_r,_g,_b,_a)              \
  g_assert ((c).r == (_r)                       \
            && (c).g == (_g)                    \
            && (c).b == (_b)                    \
            && (c).a == (_a))

int
main (void)
{
  SDL_Color c;

  // unknown color
  g_assert (!ginga_color_parse ("*** unknown ***", &c));

  // black
  g_assert (ginga_color_parse ("black", &c));
  CHECK_COLOR (c, 0, 0, 0, 255);

  g_assert (ginga_color_parse ("#000000", &c));
  CHECK_COLOR (c, 0, 0, 0, 255);

  // white
  g_assert (ginga_color_parse ("white", &c));
  CHECK_COLOR (c, 255, 255, 255, 255);

  g_assert (ginga_color_parse ("#ffffff", &c));
  CHECK_COLOR (c, 255, 255, 255, 255);

  // red
  g_assert (ginga_color_parse ("red", &c));
  CHECK_COLOR (c, 255, 0, 0, 255);

  g_assert (ginga_color_parse ("#ff0000", &c));
  CHECK_COLOR (c, 255, 0, 0, 255);

  exit (0);
}
