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

#include "ginga.h"
#include "util/Color.h"
#include "mb/IFontProvider.h"
#include "mb/SDLSurface.h"

#include "TextPlayer.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_PLAYER_BEGIN

TextPlayer::TextPlayer () : Player ("")
{
  initializePlayer ();
}

TextPlayer::~TextPlayer ()
{
  if (fontColor != NULL)
    {
      delete fontColor;
      fontColor = NULL;
    }

  if (bgColor != NULL)
    {
      delete bgColor;
      bgColor = NULL;
    }

  /* release window and surface first; then, release font */
  if (outputWindow != 0 && Ginga_Display->hasWindow (outputWindow))
    {
      outputWindow->revertContent ();
      delete outputWindow;
      outputWindow = 0;
    }

  if (surface != 0 && Ginga_Display->hasSurface (surface))
    {
      delete surface;
      surface = 0;
    }

  if (font != 0)
    {
      Ginga_Display->releaseFontProvider ((IFontProvider*)font);
      font = 0;
    }
}

void
TextPlayer::initializePlayer ()
{
  this->fontHeight = 0;
  this->currentAlign = IFontProvider::FP_TA_LEFT;
  this->currentLine = 0;
  this->currentColumn = 0;
  this->tabSize = 0;
  this->font = 0;
  this->bgColor = NULL;
  this->fontColor = NULL;
  this->fontSize = 12;
  this->fontUri = string (GINGA_FONT_DATADIR) + "vera.ttf";
  this->surface = Ginga_Display->createSurface ();
  if (this->surface != 0)
    {
      int cap = surface->getCap ("ALPHACHANNEL");
      surface->setCaps (cap);
    }
}

int
TextPlayer::write (SDLSurface* s, const string &text,
                   short textAlign, const string &fontUri, int fontSize,
                   Color *fontColor)
{
  if (fontSize < 1 || s == 0 || text == "")
    {
      return 0;
    }

  IFontProvider* font = 0;
  int width = 0;

  font = Ginga_Display->createFontProvider (fontUri.c_str (), fontSize);

  if (fontColor == NULL)
    {
      fontColor = new Color ("black");
    }

  if (font != 0)
    {
      s->setColor (fontColor->getR (), fontColor->getG (),
                           fontColor->getB (), fontColor->getAlpha ());

      width = font->getStringWidth (text.c_str (), (int) text.size ());

      font->playOver (s, text.c_str (), 0, 0, textAlign);

      Ginga_Display->releaseFontProvider ((IFontProvider*)font);
      font = 0;
    }

  delete fontColor;
  fontColor = NULL;

  return width;
}

bool
TextPlayer::setFont (const string &someUri)
{
  if (!fileExists (someUri))
    {
      clog << "TextPlayer::setFont Warning! File not found: '";
      clog << someUri.c_str () << "'" << endl;
      return false;
    }

  this->fontUri = someUri;
  if (font != 0)
    {
      Ginga_Display->releaseFontProvider ((IFontProvider*)font);
      font = 0;
    }

  font = Ginga_Display->createFontProvider (someUri.c_str (), fontSize);
  if (font == 0)
    {
      clog << "TextPlayer::setFont Warning! Can't create Font '";
      clog << someUri << "': '" << font << "'" << endl;
      return false;
    }

  fontHeight = font->getHeight ();
  return true;
}

void
TextPlayer::setFontSize (int size)
{
  fontSize = size;
  setFont (fontUri);
}

int
TextPlayer::getFontSize ()
{
  return fontSize;
}

int
TextPlayer::getFontHeight ()
{
  return fontHeight;
}

void
TextPlayer::setBgColor (guint8 red, guint8 green, guint8 blue, guint8 alpha)
{
  if (bgColor != NULL)
    {
      delete bgColor;
      bgColor = NULL;
    }

  bgColor = new Color (red, green, blue, alpha);
  if (this->surface != 0)
    {
      surface->setBgColor (red, green, blue, alpha);
    }
}

void
TextPlayer::setColor (guint8 red, guint8 green, guint8 blue, guint8 alpha)
{
  if (this->fontColor != NULL)
    {
      delete this->fontColor;
      this->fontColor = NULL;
    }

  this->fontColor = new Color (red, green, blue, alpha);
}

void
TextPlayer::setTabSize (int size)
{
  this->tabSize = size;
}

int
TextPlayer::getTabSize ()
{
  return tabSize;
}

void
TextPlayer::drawText (const string &text, short align)
{
  string uri;
  int textWidth, surWidth, surHeight;

  unsigned int maxToDraw;
  string::size_type splitPos, len;
  unsigned int widthAverage;
  bool space;
  int oldTextWidth;
  string aux, splited;

  aux = text;
  uri = string (GINGA_FONT_DATADIR) + "vera.ttf";
  if (font == 0 && fileExists (uri))
    {
      setFont (uri);
    }

  if (font == 0)
    {
      clog << "TextPlayer::drawText Warning! can't set font" << endl;
      return;
    }

  if (fontColor == NULL)
    {
      fontColor = new Color ("black");
    }

  surface->setColor (fontColor->getR (), fontColor->getG (),
                       fontColor->getB (), fontColor->getAlpha ());

  if (font != 0 && surface != 0)
    {
      surface->getSize (&surWidth, &surHeight);
      textWidth = font->getStringWidth (aux.c_str (), (int) aux.size ());

      if (textWidth > surWidth && aux.length () > 1)
        {
          space = false;

          widthAverage = (int)(textWidth / aux.length ());
          maxToDraw = (int)(((surWidth) / widthAverage) * 0.85);

          len = aux.length ();
          splited = aux.substr (0, maxToDraw);
          splitPos = splited.find_last_of (" ");

          if (splitPos == std::string::npos)
            {
              splitPos = maxToDraw;
              splited = aux.substr (splitPos, len - splitPos);
            }
          else
            {
              splitPos++;
              splited = aux.substr (splitPos, len - splitPos);
              space = true;
            }

          aux = aux.substr (0, splitPos);

          textWidth = font->getStringWidth (aux.c_str ());

          while (textWidth > surWidth)
            {
              if (space)
                {
                  splited = " " + splited;
                }

              len = aux.length ();
              splitPos = aux.find_last_of (" ");

              if (splitPos == std::string::npos)
                {
                  splited = aux[len] + splited;
                  aux = aux.substr (0, len - 1);
                  space = false;
                }
              else
                {
                  splitPos++;
                  splited = aux.substr (splitPos, len - splitPos) + splited;

                  aux = aux.substr (0, splitPos);
                  space = true;
                }

              oldTextWidth = textWidth;
              textWidth = font->getStringWidth (aux.c_str ());

              if (oldTextWidth == textWidth)
                {
                  break;
                }
            }

          font->playOver (surface, aux.c_str (), currentColumn,
                                currentLine, align);

          breakLine ();
          if (splited != text)
            {
              drawText (splited, align);
            }
        }
      else
        {
          font->playOver (surface, aux.c_str (), currentColumn,
                                currentLine, align);

          currentColumn += textWidth;
        }
    }
  else
    {
      clog << "TextPlayer::drawText Warning! FontProvider(" << font;
      clog << ") or Surface(" << surface << ") = NULL";
      clog << endl;
    }
}

bool
TextPlayer::drawTextLn (const string &text, short align)
{
  drawText (text, align);
  return breakLine ();
}

void
TextPlayer::tab ()
{
  currentColumn = currentColumn + (tabSize * 12);
}

bool
TextPlayer::breakLine ()
{
  int w, h;
  if (font == 0)
    {
      setFont (string (GINGA_FONT_DATADIR) + "decker.ttf");
    }

  surface->getSize (&w, &h);
  if ((currentLine + fontHeight) > h)
    {
      clog << "TextPlayer::breakLine() Exceeding surface bounds";
      clog << " currentLine = '" << currentLine << "'";
      clog << " fontHeight = '" << fontHeight << "'";
      clog << " surH = '" << h << "'" << endl;

      currentLine = currentLine + (int)(1.15 * fontHeight);
      currentColumn = 0;
      return false;
    }
  else
    {
      currentLine = currentLine + (int)(1.15 * fontHeight);
      currentColumn = 0;
      return true;
    }
}

int
TextPlayer::getCurrentColumn ()
{
  return this->currentColumn;
}

int
TextPlayer::getCurrentLine ()
{
  return this->currentLine;
}

void
TextPlayer::setPropertyValue (const string &name, const string &value)
{
  // TODO: set font url, font size, font color, ...
  Player::setPropertyValue (name, value);
}

GINGA_PLAYER_END
