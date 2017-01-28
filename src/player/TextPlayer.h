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

#ifndef TEXT_PLAYER_H
#define TEXT_PLAYER_H

#include "mb/SDLWindow.h"
#include "mb/IFontProvider.h"
#include "mb/Display.h"
using namespace ::ginga::mb;

#include "util/functions.h"

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/Thread.h"

#include "AVPlayer.h"

GINGA_PLAYER_BEGIN

class TextPlayer : public Player
{
protected:
  int fontHeight;
  int currentLine;
  int currentColumn;
  int tabSize;
  int fontSize;
  Color *fontColor;
  Color *bgColor;
  string fontUri;
  short currentAlign;
  IFontProvider* font;

public:
  TextPlayer ();
  virtual ~TextPlayer ();

private:
  void initializePlayer ();

public:
  static int write (SDLSurface* s,
                    const string &text,
                    short textAlign,
                    const string &fontUri = "",
                    int fontSize = 12,
                    Color *fontColor = NULL);

  virtual bool
  play ()
  {
    return Player::play ();
  };
  virtual void
  stop ()
  {
    Player::stop ();
  };
  virtual void
  abort ()
  {
    Player::abort ();
  };
  virtual void
  pause ()
  {
    Player::pause ();
  };
  virtual void
  resume ()
  {
    Player::resume ();
  };

  bool setFont (const string &someUri);
  void setFontSize (int size);
  int getFontSize ();
  int getFontHeight ();
  void setBgColor (guint8 red, guint8 green, guint8 blue,
                   guint8 alpha = 255);
  void setColor (guint8 red, guint8 green, guint8 blue, guint8 alpha = 255);
  void setTabSize (int size);
  int getTabSize ();
  void drawText (const string &text, short align);
  bool drawTextLn (const string &text, short align);
  void tab ();
  bool breakLine ();
  int getCurrentColumn ();
  int getCurrentLine ();
  virtual void setPropertyValue (const string &name, const string &value);
};

GINGA_PLAYER_END

#endif /* TEXT_PLAYER_H */
