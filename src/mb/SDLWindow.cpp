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
#include "SDLWindow.h"

#include "SDLConvert.h"
#include "Display.h"
#include "SDLSurface.h"

#include "util/Color.h"
using namespace ::ginga::util;

GINGA_MB_BEGIN

SDLWindow::SDLWindow (int x, int y, int width,
                      int height, int z)
{
  initialize (0, x, y, width, height, (double) z);
}

SDLWindow::~SDLWindow ()
{
  vector<SDLSurface *>::iterator i;

  lock ();
  lockChilds ();
  if (childSurface != NULL)
    {
      childSurface->setParentWindow (NULL);
    }

  unlockChilds ();

  lockSurface ();
  curSur = NULL;

  releaseWinISur ();

  set<SDLWindow *>::iterator j;
  j = mirrors.begin ();
  while (j != mirrors.end ())
    {
      (*j)->setMirrorSrc (NULL);
      mirrors.erase (j);
      j = mirrors.begin ();
    }
  mirrors.clear ();

  if (mirrorSrc != NULL)
    {
      mirrorSrc->removeMirror (this);
      mirrorSrc = NULL;
    }
  unlockSurface ();

  releaseBGColor ();
  releaseBorderColor ();
  releaseWinColor ();
  releaseColorKey ();

  // release window will delete texture
  Ginga_Display->destroyWindow (this);

  Thread::mutexDestroy (&mutexC);

  this->isWaiting = false;
  Thread::mutexDestroy (&cMutex);
  Thread::condDestroy (&cond);

  Thread::mutexDestroy (&rMutex);

  unlock ();
  Thread::mutexDestroy (&mutex);

  clog << "SDLWindow::~SDLWindow(" << this << ") all done" << endl;
}

void
SDLWindow::initialize (arg_unused (SDLWindow* parentWindowID),
                       int x, int y, int w, int h,
                       double z)
{
  this->texture = NULL;
  this->winISur = 0;
  this->curSur = NULL;

  this->textureUpdate = false;
  this->textureOwner = true;

  this->borderWidth = 0;
  this->bgColor = {0, 0, 0, 0};
  this->borderColor = {0, 0, 0, 0};
  this->winColor = NULL;
  this->colorKey = NULL;

  this->rect.x = x;
  this->rect.y = y;
  this->rect.w = w;
  this->rect.h = h;
  this->z = z;
  this->ghost = false;
  this->visible = false;
  this->childSurface = NULL;
  this->fit = true;
  this->stretch = true;
  this->caps = 0;
  this->transparencyValue = 0x00;
  this->mirrorSrc = NULL;

  Thread::mutexInit (&mutex);
  Thread::mutexInit (&mutexC);
  Thread::mutexInit (&texMutex);
  Thread::mutexInit (&surMutex, true);

  this->isWaiting = false;
  Thread::mutexInit (&cMutex);
  Thread::condInit (&cond, NULL);

  Thread::mutexInit (&rMutex);
}

void
SDLWindow::releaseWinISur ()
{
  if (winISur != 0)
    {
      delete winISur;
      winISur = 0;
    }
}

void
SDLWindow::releaseBGColor ()
{
}

void
SDLWindow::releaseBorderColor ()
{
}

void
SDLWindow::releaseWinColor ()
{
  if (winColor != NULL)
    {
      delete winColor;
      winColor = NULL;
    }
}

void
SDLWindow::releaseColorKey ()
{
  if (colorKey != NULL)
    {
      delete colorKey;
      colorKey = NULL;
    }
}

void
SDLWindow::addMirror (SDLWindow *window)
{
  assert (window != this);
  lockSurface ();
  mirrors.insert (window);
  unlockSurface ();
}

bool
SDLWindow::removeMirror (SDLWindow *window)
{
  set<SDLWindow *>::iterator i;

  lockSurface ();
  i = mirrors.find (window);
  if (i != mirrors.end ())
    {
      mirrors.erase (i);
      unlockSurface ();
      return true;
    }
  unlockSurface ();
  return false;
}

void
SDLWindow::setMirrorSrc (SDLWindow *mirrorSrc)
{
  lockSurface ();
  this->mirrorSrc = mirrorSrc;
  if (this->mirrorSrc != NULL)
    {
      this->mirrorSrc->addMirror (this);
    }
  unlockSurface ();
}

SDLWindow *
SDLWindow::getMirrorSrc ()
{
  return mirrorSrc;
}

// SANITY BEGIN ------------------------------------------------------------
double
SDLWindow::getAlpha ()
{
  return (double)(255 - this->transparencyValue) / 255;
}

void
SDLWindow::setAlpha (double a)
{
  this->transparencyValue = (guint8)(255 - (255 * a));
}

SDL_Color
SDLWindow::getBgColor ()
{
  return this->bgColor;
}

void
SDLWindow::setBgColor (SDL_Color c)
{
  this->bgColor = c;
}

SDL_Rect
SDLWindow::getRect ()
{
  return this->rect;
}

void
SDLWindow::setRect (SDL_Rect r)
{
  this->rect = r;
}
// SANITY END --------------------------------------------------------------

void
SDLWindow::setColorKey (guint8 r, guint8 g, guint8 b)
{
  releaseColorKey ();
  colorKey = new Color (r, g, b);
}

Color *
SDLWindow::getColorKey ()
{
  return colorKey;
}

void
SDLWindow::setWindowColor (guint8 r, guint8 g, guint8 b, guint8 alpha)
{
  releaseWinColor ();
  winColor = new Color (r, g, b, alpha);
}

Color *
SDLWindow::getWindowColor ()
{
  return winColor;
}

void
SDLWindow::setBorder (SDL_Color c, int w)
{
  borderWidth = w;
  borderColor = c;
}

void
SDLWindow::getBorder (SDL_Color *c, int *w)
{
  set_if_nonnull (c, this->borderColor);
  set_if_nonnull (w, this->borderWidth);
}

void
SDLWindow::revertContent ()
{
  lockSurface ();
  releaseWinISur ();
  unlockSurface ();
}

void
SDLWindow::setChildSurface (SDLSurface *iSur)
{
  lockSurface ();
  releaseWinISur ();
  this->childSurface = iSur;
  unlockSurface ();
}

int
SDLWindow::getCap (arg_unused (string cap))
{
  return 0;
}

void
SDLWindow::setCaps (int caps)
{
  this->caps = caps;
}

void
SDLWindow::addCaps (int capability)
{
  this->caps = (this->caps | capability);
}

int
SDLWindow::getCaps ()
{
  return caps;
}

void
SDLWindow::draw ()
{
}

void
SDLWindow::setBounds (int posX, int posY, int w, int h)
{
  this->rect.x = posX;
  this->rect.y = posY;
  this->rect.w = w;
  this->rect.h = h;
}

void
SDLWindow::moveTo (int posX, int posY)
{
  this->rect.x = posX;
  this->rect.y = posY;
}

void
SDLWindow::resize (int width, int height)
{
  this->rect.w = width;
  this->rect.h = height;
}

void
SDLWindow::raiseToTop ()
{
}

void
SDLWindow::lowerToBottom ()
{
}

void
SDLWindow::setCurrentTransparency (guint8 alpha)
{
  if (alpha != 255)
    {
      this->visible = true;
    }
  else
    {
      this->visible = false;
    }

  transparencyValue = alpha;

  lockTexture ();
  if (texture != NULL)
    {
      if (SDL_SetTextureAlphaMod (texture, (guint8)(255 - alpha)) < 0)
        {
          clog << "SDLWindow::setCurrentTransparency SDL error: '";
          clog << SDL_GetError () << "'" << endl;
        }
    }
  unlockTexture ();
}

guint8
SDLWindow::getTransparencyValue ()
{
  return this->transparencyValue;
}

void
SDLWindow::show ()
{
  this->visible = true;
}

void
SDLWindow::hide ()
{
  visible = false;
}

int
SDLWindow::getX ()
{
  return this->rect.x;
}

int
SDLWindow::getY ()
{
  return this->rect.y;
}

int
SDLWindow::getW ()
{
  return this->rect.w;
}

int
SDLWindow::getH ()
{
  return this->rect.h;
}

double
SDLWindow::getZ ()
{
  return z;
}

void
SDLWindow::setX (int x)
{
  this->rect.x = x;
}

void
SDLWindow::setY (int y)
{
  this->rect.y = y;
}

void
SDLWindow::setW (int w)
{
  this->rect.w = w;
}

void
SDLWindow::setH (int h)
{
  this->rect.h = h;
}

void
SDLWindow::setZ (double z)
{
  this->z = z;
}

bool
SDLWindow::isGhostWindow ()
{
  return ghost;
}

void
SDLWindow::setGhostWindow (bool ghost)
{
  this->ghost = ghost;
}

bool
SDLWindow::isVisible ()
{
  return this->visible;
}

void
SDLWindow::validate ()
{
  lockSurface ();
  unprotectedValidate ();
  unlockSurface ();
}

void
SDLWindow::unprotectedValidate ()
{
  if (winISur != 0)
    {
      winISur->flip ();
      curSur = winISur->getContent ();
      textureUpdate = true;
    }
  else if (childSurface != NULL)
    {
      childSurface->flip ();
      textureUpdate = true;
    }
}

vector<DrawData *> *
SDLWindow::createDrawDataList ()
{
  vector<DrawData *> *dd = NULL;

  lockChilds ();
  if (childSurface != NULL
      && Ginga_Display->hasSurface (childSurface))
    {
      dd = ((SDLSurface *)childSurface)->createDrawDataList ();
    }
  unlockChilds ();

  return dd;
}

void
SDLWindow::setStretch (bool stretchTo)
{
  this->stretch = stretchTo;
}

bool
SDLWindow::getStretch ()
{
  return this->stretch;
}

void
SDLWindow::setFit (bool fitTo)
{
  this->fit = fitTo;
}

bool
SDLWindow::getFit ()
{
  return this->fit;
}

void
SDLWindow::clearContent ()
{
  lockSurface ();
  if (curSur != NULL)
    {
      if (SDL_FillRect (curSur, NULL,
                        SDL_MapRGBA (curSur->format, 0, 0, 0, 0))
          < 0)
        {
          clog << "SDLWindow::clearContent SDL error: '";
          clog << SDL_GetError () << "'" << endl;
        }
      textureUpdate = true;
    }
  unlockSurface ();
}

void
SDLWindow::setRenderedSurface (SDL_Surface *uSur)
{
  lockSurface ();
  curSur = uSur;
  if (curSur != NULL)
    {
      textureUpdate = true;
    }
  unlockSurface ();
}

SDL_Surface *
SDLWindow::getContent ()
{
  return curSur;
}

void
SDLWindow::setTexture (SDL_Texture *texture)
{
  lockTexture ();

  if (this->texture == texture)
    {
      unlockTexture ();
      return;
    }

  if (textureOwner && this->texture != NULL)
    {
    }

  if (texture == NULL)
    {
      textureOwner = true;
    }
  else
    {
      textureOwner = false;
    }

  this->texture = texture;
  unlockTexture ();
}

SDL_Texture *
SDLWindow::getTexture (SDL_Renderer *renderer)
{
  SDL_Texture *uTex;

  lockTexture ();
  if (renderer != NULL)
    {
      if (textureOwner && textureUpdate && texture != NULL)
        {
          Display::releaseTexture (texture);
          textureUpdate = false;
          texture = NULL;
        }

      if (texture == NULL)
        {
          lockSurface ();
          if (curSur != NULL)
            {
              textureOwner = true;
              texture = Display::createTextureFromSurface (renderer,
                                                                   curSur);
            }
          unlockSurface ();
        }
    }

  uTex = texture;
  unlockTexture ();

  return uTex;
}

bool
SDLWindow::isTextureOwner (SDL_Texture *texture)
{
  bool itIs = false;
  if (texture != NULL && this->texture == texture && textureOwner)
    {
      itIs = true;
    }

  return itIs;
}

bool
SDLWindow::isMine (SDLSurface *surface)
{
  bool itIs = false;

  if (surface != NULL && surface->getContent () != NULL)
    {
      if (surface == winISur || surface == childSurface)
        {
          itIs = true;
        }
    }

  return itIs;
}

void
SDLWindow::renderFrom (SDLSurface *surface)
{
  SDL_Surface *contentSurface;

  Thread::mutexLock (&rMutex);
  contentSurface = surface->getContent ();
  if (contentSurface == NULL)
    {
      clog << "SDLWindow::renderFrom(" << this;
      clog << ") Warning! NULL underlying ";
      clog << "surface!" << endl;
      Thread::mutexUnlock (&rMutex);
      return;
    }

  lockSurface ();
  if (!isMine (surface))
    {
      releaseWinISur ();
      winISur = Ginga_Display->createSurface (
          contentSurface->w, contentSurface->h);

      winISur->blit (0, 0, surface);
      winISur->flip ();

      curSur = winISur->getContent ();
      textureUpdate = true;
    }
  else
    {
      curSur = contentSurface;
      textureUpdate = true;
    }
  unlockSurface ();

  Thread::mutexUnlock (&rMutex);
}

void
SDLWindow::blit (arg_unused (SDLWindow *src))
{
}

void
SDLWindow::stretchBlit (arg_unused (SDLWindow *src))
{
}

string
SDLWindow::getDumpFileUri (int quality, arg_unused (int dumpW), arg_unused (int dumpH))
{
  string uri;
  SDL_Surface *dumpUSur;

  lockSurface ();
  if (curSur != NULL)
    {
      dumpUSur = curSur;
    }
  else if (texture != NULL)
    {
      dumpUSur
          = Display::createUnderlyingSurfaceFromTexture (texture);
    }
  else
    {
      unlockSurface ();
      return "";
    }

  Display::lockSDL ();
  xstrassign (uri, "%s/dump_%p.jpg", g_get_tmp_dir (), (void *) this);
  int ret
      = SDLConvert::convertSurfaceToJPEG (uri.c_str (), dumpUSur, quality);
  if (ret == -1)
    uri = "";
    //Display::createReleaseContainer (dumpUSur, NULL, NULL);
  Display::unlockSDL ();

  unlockSurface ();
  return uri;
}

void
SDLWindow::lock ()
{
  Thread::mutexLock (&mutex);
}

void
SDLWindow::unlock ()
{
  Thread::mutexUnlock (&mutex);
}

void
SDLWindow::lockChilds ()
{
  Thread::mutexLock (&mutexC);
}

void
SDLWindow::unlockChilds ()
{
  Thread::mutexUnlock (&mutexC);
}

bool
SDLWindow::rendered ()
{
  if (isWaiting)
    {
      Thread::condSignal (&cond);
      return true;
    }
  return false;
}

void
SDLWindow::waitRenderer ()
{
  isWaiting = true;
  Thread::mutexLock (&cMutex);
  Thread::condWait (&cond, &cMutex);
  isWaiting = false;
  Thread::mutexUnlock (&cMutex);
}

void
SDLWindow::lockTexture ()
{
  Thread::mutexLock (&texMutex);
}

void
SDLWindow::unlockTexture ()
{
  Thread::mutexUnlock (&texMutex);
}

void
SDLWindow::lockSurface ()
{
  Thread::mutexLock (&surMutex);
}

void
SDLWindow::unlockSurface ()
{
  Thread::mutexUnlock (&surMutex);
}

GINGA_MB_END
