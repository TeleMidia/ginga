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
#include "SDLFontProvider.h"

#include "Display.h"
#include "SDLWindow.h"
#include "SDLSurface.h"

GINGA_MB_BEGIN

const short SDLFontProvider::A_TOP = 3;
const short SDLFontProvider::A_TOP_CENTER = 4;
const short SDLFontProvider::A_TOP_LEFT = 5;
const short SDLFontProvider::A_TOP_RIGHT = 6;

const short SDLFontProvider::A_BOTTOM = 7;
const short SDLFontProvider::A_BOTTOM_CENTER = 8;
const short SDLFontProvider::A_BOTTOM_LEFT = 9;
const short SDLFontProvider::A_BOTTOM_RIGHT = 10;

map<string, TTF_Font *> SDLFontProvider::fonts;

pthread_mutex_t SDLFontProvider::ntsMutex;
bool SDLFontProvider::initNTSMutex = false;

bool SDLFontProvider::initialized = false;
short SDLFontProvider::fontRefs = 0;

SDLFontProvider::SDLFontProvider (const char *fontUri, int heightInPixel)
{
  type = FontProviderType;

  if (!initNTSMutex)
    {
      initNTSMutex = true;
      Thread::mutexInit (&ntsMutex, true);
    }

  fontRefs++;

  this->fontInit = false;
  this->fontUri = "";
  this->dfltFont = string (GINGA_FONT_DATADIR) + "vera.ttf";
  this->height = heightInPixel;
  this->font = NULL;
  this->content = NULL;
  this->plainText = "";
  this->coordX = 0;
  this->coordY = 0;
  this->align = A_TOP_LEFT;
  this->fontUri.assign (fontUri, strlen (fontUri));
}

SDLFontProvider::~SDLFontProvider ()
{
  Thread::mutexLock (&ntsMutex);

  fontRefs--;

  fonts[fontUri + xstrbuild ("%d", height)] = font;

  content = NULL;
  plainText = "";
  fontUri = "";
  dfltFont = "";

  if (fontRefs == 0)
    {
      // FIXME: Find a better way to do this!
      // initialized = false;
      // TTF_Quit();
    }

  Thread::mutexUnlock (&ntsMutex);
}

void
SDLFontProvider::releaseFonts ()
{
  map<string, TTF_Font *>::iterator i;

  Thread::mutexLock (&ntsMutex);

  i = fonts.begin ();
  while (i != fonts.end ())
    {
      TTF_CloseFont (font);
      ++i;
    }
  fonts.clear ();

  Thread::mutexUnlock (&ntsMutex);
}

bool
SDLFontProvider::initializeFont ()
{
  Thread::mutexLock (&ntsMutex);

  if (!initialized)
    {
      initialized = true;
      if (TTF_Init () < 0)
        {
          clog << "SDLFontProvider::initializeFont Warning! ";
          clog << "Couldn't initialize TTF: " << SDL_GetError ();
          clog << endl;
          Thread::mutexUnlock (&ntsMutex);
          return false;
        }
    }

  if (height < 1)
    {
      height = 12;
    }

  createFont ();
  Thread::mutexUnlock (&ntsMutex);

  return true;
}

bool
SDLFontProvider::createFont ()
{
  map<string, TTF_Font *>::iterator i;

  Thread::mutexLock (&ntsMutex);
  if (fileExists (fontUri))
    {
      i = fonts.find (fontUri + xstrbuild ("%d", height));
      if (i != fonts.end ())
        {
          font = i->second;
          fonts.erase (i);
        }
      else
        {
          if (fonts.size () > 5)
            {
              releaseFonts ();
            }
          font = TTF_OpenFont (fontUri.c_str (), height);
        }

      if (font == NULL)
        {
          clog << "SDLFontProvider::initializeFont Warning! ";
          clog << "Couldn't initialize font: " << fontUri;
          clog << endl;

          Thread::mutexUnlock (&ntsMutex);
          return false;
        }

      TTF_SetFontStyle (font, (int)TTF_STYLE_BOLD);
      TTF_SetFontOutline (font, 0);
      TTF_SetFontKerning (font, 1);
      TTF_SetFontHinting (font, (int)TTF_HINTING_NORMAL);

      Thread::mutexUnlock (&ntsMutex);
      return true;
    }

  Thread::mutexUnlock (&ntsMutex);
  return false;
}

void *
SDLFontProvider::getFontProviderContent ()
{
  return (void *)font;
}

void
SDLFontProvider::getStringExtents (const char *text, int *w, int *h)
{
  Thread::mutexLock (&ntsMutex);

  if (font == NULL)
    {
      initializeFont ();
    }

  if (font != NULL)
    {
      TTF_SizeText (font, text, w, h);
    }
  else
    {
      clog << "SDLFontProvider::getStringExtents Warning! ";
      clog << "Can't get text size: font is NULL." << endl;
    }

  Thread::mutexUnlock (&ntsMutex);
}

int
SDLFontProvider::getStringWidth (const char *text, int textLength)
{
  int w = -1, h = -1;
  string aux = "";

  Thread::mutexLock (&ntsMutex);

  if (textLength == 0 || textLength > (int)strlen (text))
    {
      getStringExtents (text, &w, &h);
    }
  else
    {
      aux.assign (text, strlen (text));
      if (aux.length () > (unsigned int) textLength)
        {
          aux = aux.substr (0, textLength);
        }
      getStringExtents (aux.c_str (), &w, &h);
    }

  Thread::mutexUnlock (&ntsMutex);

  return w;
}

int
SDLFontProvider::getHeight ()
{
  return height;
}

void
SDLFontProvider::playOver (SDLSurface* surface, const char *text, int x,
                           int y, short align)
{
  size_t textLength;
  Thread::mutexLock (&ntsMutex);

  if (font == NULL)
    {
      initializeFont ();
    }

  textLength = strlen (text);
  if (textLength <= 0)
    {
      Thread::mutexUnlock (&ntsMutex);
      return;
    }

  plainText.assign (text, textLength);
  assert (plainText != "");

  coordX = x;
  coordY = y;
  this->align = align;

  playOver (surface);

  Thread::mutexUnlock (&ntsMutex);
}

void
SDLFontProvider::playOver (SDLSurface* surface)
{
  SDLWindow *parent;
  Color *fontColor = NULL;

  SDL_Color sdlColor;
  SDL_Rect rect;
  SDL_Surface *renderedSurface = NULL;
  SDL_Surface *text;
  int pW, pH;

  Thread::mutexLock (&ntsMutex);
  this->content = surface;

  if (font == NULL)
    {
      if (!createFont ())
        {
          clog << "SDLFontProvider::playOver Warning! NULL font.";
          clog << endl;

          Thread::mutexUnlock (&ntsMutex);
          return;
        }
    }

  if (Ginga_Display->hasSurface (content))
    {
      parent = (SDLWindow *)(content->getParentWindow ());

      if (parent == NULL)
        {
          clog << "SDLFontProvider::playOver Warning! NULL parent.";
          clog << endl;
          Thread::mutexUnlock (&ntsMutex);
          return;
        }

      if (coordX >= parent->getW () || coordY >= parent->getH ())
        {
          clog << "SDLFontProvider::playOver Warning! Invalid coords.";
          clog << endl;
          Thread::mutexUnlock (&ntsMutex);
          return;
        }

      fontColor = content->getColor ();

      if (fontColor != NULL)
        {
          sdlColor.r = (gint8) fontColor->getR ();
          sdlColor.g = (gint8) fontColor->getG ();
          sdlColor.b = (gint8) fontColor->getB ();
        }
      else
        {
          sdlColor.r = 0x00;
          sdlColor.g = 0x00;
          sdlColor.b = 0x00;
        }

      assert (font != NULL);

      text = TTF_RenderUTF8_Solid (font, plainText.c_str (), sdlColor);

      if (text == NULL)
        {
          clog << "SDLFontProvider::playOver Warning! Can't create ";
          clog << "underlying surface from text" << endl;
          Thread::mutexUnlock (&ntsMutex);
          return;
        }

      Display::addUnderlyingSurface (text);

      rect.x = coordX;
      rect.y = coordY;
      rect.w = text->w;
      rect.h = text->h;

      renderedSurface = ((SDLSurface *)content)->getPendingSurface ();

      if (renderedSurface == NULL)
        {
          renderedSurface = (SDL_Surface *)(parent->getContent ());
        }

      pW = parent->getW ();
      pH = parent->getH ();
      if (renderedSurface == NULL)
        {
          renderedSurface
              = Display::createUnderlyingSurface (pW, pH);
          content->setContent (renderedSurface);
          parent->setRenderedSurface (renderedSurface);

          clog << "SDLFontProvider::playOver parent = '" << parent;
          clog << "' bounds = '" << parent->getX () << ",";
          clog << parent->getY () << ",";
          clog << pW << ",";
          clog << pH << "' text rectangle = '";
          clog << rect.x << ", " << rect.y << ", ";
          clog << rect.w << ", " << rect.h << "'";
          clog << endl;
        }

      if (rect.w < pW)
        {
          if (align == IFontProvider::FP_TA_CENTER)
            {
              rect.x = rect.x + (int)((pW - rect.w) / 2);
            }
          else if (align == IFontProvider::FP_TA_RIGHT)
            {
              rect.x = pW - rect.w;
            }
        }

      Display::lockSDL ();
      if (renderedSurface != NULL)
        {
          if (SDL_UpperBlit (text, NULL, renderedSurface, &rect) < 0)
            {
              clog << "SDLFontProvider::playOver Warning! Can't blit ";
              clog << "text considering rectangle = '";
              clog << rect.x << ", " << rect.y << ", ";
              clog << rect.w << ", " << rect.h << "': ";
              clog << SDL_GetError ();
              clog << endl;
            }
        }
      Display::unlockSDL ();

      //Display::createReleaseContainer (text, NULL, NULL);
      text = NULL;
    }
  else
    {
      clog << "SDLFontProvider::playOver Warning! Invalid Surface.";
      clog << endl;
    }

  Thread::mutexUnlock (&ntsMutex);
}

GINGA_MB_END
