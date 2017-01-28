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

#include "PlainTxtPlayer.h"

GINGA_PLAYER_BEGIN

PlainTxtPlayer::PlainTxtPlayer (arg_unused (const string &mrl))
    : TextPlayer ()
{
  Thread::mutexInit (&mutex, false);
  content = "";
}

PlainTxtPlayer::~PlainTxtPlayer ()
{
  Thread::mutexLock (&mutex);
  if (Ginga_Display->hasSurface (surface))
    {
      surface->setParentWindow (0);
    }
  Thread::mutexUnlock (&mutex);
  Thread::mutexDestroy (&mutex);
}

void
PlainTxtPlayer::setFile (const string &mrl)
{
  if (mrl == "" || !fileExists (mrl))
    {
      clog << "PlainTxtPlayer::setFile Warning! File not found: '";
      clog << mrl << "'" << endl;
      return;
    }

  if (mrl.length () > 4)
    {
      string fileType;

      this->mrl = mrl;
      fileType = this->mrl.substr (this->mrl.length () - 4, 4);
      if (fileType != ".txt")
        {
          clog << "PlainTxtPlayer::loadFile Warning! Unknown file ";
          clog << "type for: '" << this->mrl << "'" << endl;
        }
    }
  else
    {
      clog << "PlainTxtPlayer::loadFile Warning! Unknown extension ";
      clog << "type for: '" << mrl << "'" << endl;
    }
}

void
PlainTxtPlayer::loadTxt ()
{
  ifstream fis;
  string line, aux;
  int surfaceW, surfaceH;

  //		if (surface != NULL) {
  //			surface->clearSurface();
  //		}

  Thread::mutexLock (&mutex);
  fis.open ((this->mrl).c_str (), ifstream::in);
  if (!fis.is_open () && (mrl != "" || content == ""))
    {
      clog << "PlainTxtPlayer::loadFile Warning! can't open input ";
      clog << "file: '" << this->mrl << "'" << endl;
      Thread::mutexUnlock (&mutex);
      return;
    }

  if (fontColor == NULL)
    {
      fontColor = new Color (255, 255, 255, 255);
    }

  if (surface != 0 && surface->getParentWindow () != 0)
    {
      if (bgColor != NULL)
        {
          surface->clearContent ();
          surface->getParentWindow ()->setBgColor (bgColor->getColor ());
        }
      else
        {
          SDLWindow* parentWindow = surface->getParentWindow ();
          parentWindow->clearContent ();
        }
    }

  this->currentLine = 0;
  this->currentColumn = 0;

  if (mrl != "" && content == "")
    {
      surface->getSize (&surfaceW, &surfaceH);
      while (!fis.eof () && fis.good ()
             && (currentLine + fontHeight) < surfaceH)
        {
          getline (fis, line);
          if (line != "")
            {
              if (!drawTextLn (line, currentAlign))
                {
                  break;
                }
            }
        }
    }
  else if (content != "")
    {
      drawTextLn (content, currentAlign);
    }

  if (surface != 0 && surface->getParentWindow () != 0)
    {
      SDLWindow* parentWin = surface->getParentWindow ();
      parentWin->validate ();
    }

  fis.close ();
  Thread::mutexUnlock (&mutex);
}

bool
PlainTxtPlayer::play ()
{
  if (surface != 0)
    {
      clog << "PlainTxtPlayer::play ok" << endl;
      loadTxt ();
      return Player::play ();
    }
  else
    {
      clog << "PlainTxtPlayer::play warning" << endl;
      return false;
    }
}

void
PlainTxtPlayer::stop ()
{
  Player::stop ();
}

void
PlainTxtPlayer::setContent (const string &content)
{
  Thread::mutexLock (&mutex);

  SDLWindow* parentWindow = surface->getParentWindow ();
  if (surface != 0 && parentWindow != 0)
    {
      if (bgColor != NULL)
        {
          surface->clearContent ();
          parentWindow->setBgColor (bgColor->getColor ());
        }
      else
        {
          parentWindow->clearContent ();
        }
    }

  this->currentLine = 0;
  this->currentColumn = 0;
  this->content = content;

  if (content != "")
    {
      drawTextLn (this->content, currentAlign);
      mrl = "";
    }

  if (surface != 0 && parentWindow != 0)
    {
      parentWindow->validate ();
    }

  Thread::mutexUnlock (&mutex);
}

void
PlainTxtPlayer::setTextAlign (const string &align)
{
  if (align == "left")
    {
      currentAlign = IFontProvider::FP_TA_LEFT;
    }
  else if (align == "right")
    {
      currentAlign = IFontProvider::FP_TA_RIGHT;
    }
  else if (align == "center")
    {
      currentAlign = IFontProvider::FP_TA_CENTER;
    }
}

void
PlainTxtPlayer::setPropertyValue (const string &name, const string &value)
{
  Thread::mutexLock (&mutex);
  gint64 size;

  vector<string> *params;
  bool refresh = true;

  if (value == "")
    {
      Thread::mutexUnlock (&mutex);
      return;
    }

  if (surface == 0)
    {
      refresh = false;
    }

  if (name == "fontColor")
    {
      if (fontColor != NULL)
        {
          delete fontColor;
          fontColor = NULL;
        }

      fontColor = new Color (value);
    }
  else if (name == "fontSize" && _xstrtoll (value.c_str (), &size))
    {
      setFontSize ((int) size);
    }
  else if (name == "fontUri")
    {
      setFont (value);
    }
  else if (name == "textAlign")
    {
      setTextAlign (value);
    }
  else if (name == "fontStyle")
    {
      string styleName = "text-align";
      size_t pos = value.find (styleName);
      string strAlign = "";
      if (pos != std::string::npos)
        {
          strAlign = value.substr (pos + styleName.length () + 1);
          setTextAlign (strAlign);
        }
    }
  else if ((name == "x-bgColor" || name == "bgColor"))
    {
      if (surface != 0)
        {
          if (bgColor != NULL)
            {
              delete bgColor;
              bgColor = NULL;
            }

          bgColor = new Color (value);

          SDLWindow* parentWindow = surface->getParentWindow ();
          if (parentWindow != 0)
            {
              // this->surface->setCaps(0);
              surface->clearContent ();
              parentWindow->setBgColor (bgColor->getColor ());
            }
        }
    }
  else if (name == "x-rgbBgColor" || name == "rgbBgColor")
    {
      params = split (value, ",");
      if (params->size () == 3)
        {
          if (surface != 0)
            {
              if (bgColor != NULL)
                {
                  delete bgColor;
                  bgColor = NULL;
                }

              bgColor = new Color (xstrto_uint8 ((*params)[0]),
                                   xstrto_uint8 ((*params)[1]),
                                   xstrto_uint8 ((*params)[2]));

              SDLWindow* parentWindow
                  = surface->getParentWindow ();
              if (parentWindow != 0)
                {
                  // this->surface->setCaps(0);
                  surface->clearContent ();
                  parentWindow->setBgColor (bgColor->getColor ());
                }
            }
        }
      else
        {
          refresh = false;
        }

      delete params;
      params = NULL;
    }
  else if (name == "x-rgbFontColor" || name == "rgbFontColor")
    {
      params = split (value, ",");
      if (params->size () == 3)
        {
          if (fontColor != NULL)
            {
              delete fontColor;
              fontColor = NULL;
            }

          fontColor = new Color (xstrto_uint8 ((*params)[0]),
                                 xstrto_uint8 ((*params)[1]),
                                 xstrto_uint8 ((*params)[2]));
        }
      else
        {
          refresh = false;
        }

      delete params;
      params = NULL;
    }
  else if (name == "x-content")
    {
      Thread::mutexUnlock (&mutex);
      setContent (value);
      Thread::mutexLock (&mutex);
      refresh = false;
    }
  else if (name == "x-setFile")
    {
      setFile (value);
      refresh = false;
    }

  Player::setPropertyValue (name, value);
  Thread::mutexUnlock (&mutex);

  // refreshing changes
  if (refresh)
    {
      loadTxt ();
    }
}

GINGA_PLAYER_END
