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
#include "SDLSurface.h"


//#include "IFontProvider.h" removed
#include "Display.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

SDLSurface::SDLSurface ()
{
  initialize ();
}

SDLSurface::SDLSurface (void *underlyingSurface)
{
  initialize ();
  this->sur = (SDL_Surface *)underlyingSurface;
}

SDLSurface::~SDLSurface ()
{
  isDeleting = true;
  Thread::mutexLock (&sMutex);
  Thread::mutexLock (&pMutex);

  SDLWindow *w = (SDLWindow *)parent;
  Ginga_Display->releaseSurface (this);

  releaseChromaColor ();
  releaseBorderColor ();
  releaseBgColor ();
  releaseSurfaceColor ();
  releaseFont ();

  if (w != NULL && Ginga_Display->hasWindow (w))
    {
      if (w->getContent () == sur)
        {
          w->setRenderedSurface (NULL);
        }
    }

  if (sur != NULL)
    {
    }

  sur = NULL;

  Thread::mutexUnlock (&sMutex);
  Thread::mutexDestroy (&sMutex);

  releasePendingSurface ();
  Thread::mutexUnlock (&pMutex);
  Thread::mutexDestroy (&pMutex);

  releaseDrawData ();
  Thread::mutexLock (&ddMutex);
  Thread::mutexUnlock (&ddMutex);
  Thread::mutexDestroy (&ddMutex);
}

void
SDLSurface::releasePendingSurface ()
{
  if (pending != NULL)
    {
      //Display::createReleaseContainer (pending, NULL, NULL);
      pending = NULL;
    }
}

bool
SDLSurface::createPendingSurface ()
{
  if (pending == NULL)
    {
      pending = createSurface ();

      if (sur != NULL && pending != NULL)
        {
          Display::lockSDL ();
          if (SDL_UpperBlit (sur, NULL, pending, NULL) < 0)
            {
              clog << "SDLSurface::createPendingSurface SDL error: '";
              clog << SDL_GetError () << "'" << endl;
            }
          Display::unlockSDL ();
        }
    }

  return (pending != NULL);
}

void
SDLSurface::checkPendingSurface ()
{
  Thread::mutexLock (&pMutex);
  SDLWindow *w = (SDLWindow *)parent;
  if (pending != NULL)
    {
      if (parent != NULL && w->getContent () == sur)
        {
          w->setRenderedSurface (pending);
        }

      Thread::mutexLock (&sMutex);
      //Display::createReleaseContainer (sur, NULL, NULL);
      sur = pending;
      pending = NULL;
      Thread::mutexUnlock (&sMutex);
      Thread::mutexUnlock (&pMutex);
      releaseDrawData ();
    }
  else
    {
      Thread::mutexUnlock (&pMutex);
    }
}

void
SDLSurface::fill ()
{
  guint8 r = 0, g = 0, b = 0;

  Thread::mutexLock (&sMutex);
  if (sur != NULL)
    {
      if (bgColor != NULL)
        {
          r = bgColor->getR ();
          g = bgColor->getG ();
          b = bgColor->getB ();
        }

      Thread::mutexLock (&pMutex);
      releasePendingSurface ();

      pending = createSurface ();

      if (pending != NULL && bgColor != NULL)
        {
          // TODO: check why we have to set BGR instead of RGB
          if (SDL_FillRect (pending, NULL,
                            SDL_MapRGB (pending->format, b, g, r))
              < 0)
            {
              clog << "SDLSurface::fill SDL error: '";
              clog << SDL_GetError () << "'" << endl;
            }
        }

      Thread::mutexUnlock (&pMutex);
    }

  releaseDrawData ();
  Thread::mutexUnlock (&sMutex);
}

void
SDLSurface::releaseChromaColor ()
{
  if (this->chromaColor != NULL)
    {
      delete this->chromaColor;
      chromaColor = NULL;
    }
}

void
SDLSurface::releaseBorderColor ()
{
  if (this->borderColor != NULL)
    {
      delete this->borderColor;
      this->borderColor = NULL;
    }
}

void
SDLSurface::releaseBgColor ()
{
  if (this->bgColor != NULL)
    {
      delete this->bgColor;
      this->bgColor = NULL;
    }
}

void
SDLSurface::releaseSurfaceColor ()
{
  if (this->surfaceColor != NULL)
    {
      delete this->surfaceColor;
      this->surfaceColor = NULL;
    }
}

void
SDLSurface::releaseFont ()
{
  if (iFont != NULL)
    {
    //  Ginga_Display->releaseFontProvider ((IFontProvider*)iFont);
    //  iFont = NULL;
    }
}

void
SDLSurface::releaseDrawData ()
{
  vector<DrawData *>::iterator i;

  Thread::mutexLock (&ddMutex);
  if (!drawData.empty ())
    {
      i = drawData.begin ();
      while (i != drawData.end ())
        {
          delete (*i);
          ++i;
        }
      drawData.clear ();
    }
  Thread::mutexUnlock (&ddMutex);
}

void
SDLSurface::initialize ()
{
  this->sur = NULL;
  this->iFont = NULL;
  this->parent = NULL;
  this->chromaColor = NULL;
  this->borderColor = NULL;
  this->bgColor = NULL;
  this->surfaceColor = NULL;
  this->caps = 1;
  this->hasExtHandler = false;
  this->isDeleting = false;
  this->pending = NULL;

  this->drawData.clear ();

  Thread::mutexInit (&ddMutex);
  Thread::mutexInit (&sMutex);
  Thread::mutexInit (&pMutex);
}

void
SDLSurface::takeOwnership ()
{
  sur = NULL;
}

SDL_Surface *
SDLSurface::getPendingSurface ()
{
  return pending;
}

void
SDLSurface::setExternalHandler (bool extHandler)
{
  this->hasExtHandler = extHandler;
}

bool
SDLSurface::hasExternalHandler ()
{
  return this->hasExtHandler;
}

void
SDLSurface::addCaps (int caps)
{
  this->caps = this->caps | caps;
}

void
SDLSurface::setCaps (int caps)
{
  this->caps = caps;
}

int
SDLSurface::getCap (arg_unused (const string &cap))
{
  return 1;
}

int
SDLSurface::getCaps ()
{
  return this->caps;
}

SDL_Surface *
SDLSurface::getContent ()
{
  return sur;
}

void
SDLSurface::setContent (SDL_Surface *surface)
{
  Thread::mutexLock (&sMutex);
  SDLWindow *w = (SDLWindow *)parent;
  if (sur != NULL && surface == sur)
    {
      Thread::mutexUnlock (&sMutex);
      return;
    }

  if (parent != NULL)
    {
      if (w->getContent () == sur && sur != NULL)
        {
          ((SDLWindow *)parent)
              ->setRenderedSurface ((SDL_Surface *)surface);
        }
    }

  if (sur != NULL)
    {
      //Display::createReleaseContainer (sur, NULL, NULL);
    }
  this->sur = (SDL_Surface *)surface;
  Thread::mutexUnlock (&sMutex);
}

bool
SDLSurface::setParentWindow (SDLWindow *parentWindow)
{
  Thread::mutexLock (&sMutex);
  SDLWindow *w = (SDLWindow *)parent;
  if (w != NULL)
    {
      w->setChildSurface (NULL);
    }

  this->parent = parentWindow;
  w = (SDLWindow *)this->parent;

  if (parent != NULL)
    {
      if (chromaColor != NULL)
        {
          w->setColorKey (chromaColor->getR (), chromaColor->getG (),
                          chromaColor->getB ());
        }

      w->setChildSurface (this);
    }

  Thread::mutexUnlock (&sMutex);

  return true;
}

SDLWindow *
SDLSurface::getParentWindow ()
{
  return this->parent;
}

void
SDLSurface::clearContent ()
{
  clearSurface ();
}

void
SDLSurface::clearSurface ()
{
  Thread::mutexLock (&sMutex);
  if (sur == NULL)
    {
      releaseDrawData ();
      Thread::mutexUnlock (&sMutex);
    }
  else
    {
      Thread::mutexUnlock (&sMutex);
      fill ();
    }
}

vector<DrawData *> *
SDLSurface::createDrawDataList ()
{
  vector<DrawData *> *cloneDD = NULL;

  Thread::mutexLock (&ddMutex);
  if (!drawData.empty ())
    {
      cloneDD = new vector<DrawData *> (drawData);
    }
  Thread::mutexUnlock (&ddMutex);

  return cloneDD;
}

void
SDLSurface::pushDrawData (int c1, int c2, int c3, int c4, short type)
{
  DrawData *dd;

  if (surfaceColor != NULL)
    {
      Thread::mutexLock (&ddMutex);
      dd = new DrawData;
      dd->coord1 = c1;
      dd->coord2 = c2;
      dd->coord3 = c3;
      dd->coord4 = c4;
      dd->dataType = type;
      dd->r = surfaceColor->getR ();
      dd->g = surfaceColor->getG ();
      dd->b = surfaceColor->getB ();
      dd->a = surfaceColor->getAlpha ();

      clog << "SDLSurface::pushDrawData current size = '";
      clog << drawData.size () << "'" << endl;

      drawData.push_back (dd);
      Thread::mutexUnlock (&ddMutex);
    }
}

void
SDLSurface::setChromaColor (guint8 r, guint8 g, guint8 b, guint8 alpha)
{
  releaseChromaColor ();
  SDLWindow *w = (SDLWindow *)parent;

  this->chromaColor = new Color (r, g, b, alpha);

  Thread::mutexLock (&sMutex);
  if (sur != NULL)
    {
      Thread::mutexLock (&pMutex);
      if (createPendingSurface ())
        {
          if (SDL_SetColorKey (pending, SDL_TRUE,
                               SDL_MapRGB (pending->format, r, g, b))
              < 0)
            {
              clog << "SDLSurface::setChromaColor SDL error: '";
              clog << SDL_GetError () << "'" << endl;
            }
        }
      Thread::mutexUnlock (&pMutex);
    }

  if (parent != NULL)
    {
      w->setColorKey (r, g, b);
    }

  Thread::mutexUnlock (&sMutex);
}

void
SDLSurface::setBgColor (guint8 r, guint8 g, guint8 b, guint8 alpha)
{
  releaseBgColor ();

  this->bgColor = new Color (r, g, b, alpha);
  fill ();
}

Color *
SDLSurface::getBgColor ()
{
  return bgColor;
}

void
SDLSurface::setColor (guint8 r, guint8 g, guint8 b, guint8 alpha)
{
  releaseSurfaceColor ();

  if (r < 10 && g < 10 && b < 10)
    {
      r = 10;
      g = 10;
      b = 10;
    }
  this->surfaceColor = new Color (r, g, b, alpha);
}

Color *
SDLSurface::getColor ()
{
  return surfaceColor;
}

void
SDLSurface::setSurfaceFont (void *font)
{
  if (iFont != font)
    {
      releaseFont ();
    }

 // iFont = (IFontProvider *)font;
}

void
SDLSurface::flip ()
{
  checkPendingSurface ();
}

void G_GNUC_NORETURN
SDLSurface::scale (arg_unused (double x), arg_unused (double y))
{
  g_assert_not_reached ();
}

void
SDLSurface::initContentSurface ()
{
  if (sur == NULL || parent == NULL)
    return;
  SDLWindow *w = (SDLWindow *)parent;
  sur = (SDL_Surface *)(w->getContent ());
  if (sur == NULL)
    return;
  sur = createSurface ();
  w->setRenderedSurface (sur);
}

SDL_Surface *
SDLSurface::createSurface ()
{
  SDLWindow *win = (SDLWindow *)parent;
  SDL_Surface *sdlSurface = NULL;
  int w, h;

  if (win == NULL)
    return NULL;

  if (Ginga_Display->hasWindow (win))
    {
      w = win->getW ();
      h = win->getH ();
    }
  else if (sur != NULL)
    {
      w = sur->w;
      h = sur->h;
    }
  else
    {
      return NULL;
    }

  sdlSurface = Display::createUnderlyingSurface (w, h);
  if (sdlSurface == NULL)
    return NULL;

  if (bgColor == NULL && caps != 0)
    SDL_SetColorKey (sdlSurface, 1, *((Uint8 *)sdlSurface->pixels));

  return sdlSurface;
}

void
SDLSurface::blit (int x, int y, SDLSurface *src, int srcX, int srcY,
                  int srcW, int srcH)
{
  SDL_Rect srcRect;
  SDL_Rect *srcPtr = NULL;
  SDL_Rect dstRect;
  SDL_Surface *uSur;

  Thread::mutexLock (&sMutex);
  initContentSurface ();

  if (sur != NULL)
    {
      Thread::mutexLock (&pMutex);

      if (src != this)
        {
          Thread::mutexLock (&((SDLSurface *)src)->sMutex);
          Thread::mutexLock (&((SDLSurface *)src)->pMutex);
        }

      uSur = (SDL_Surface *)(src->getContent ());

      if (uSur != NULL)
        {
          if (srcX >= 0)
            {
              srcRect.x = srcX;
              srcRect.y = srcY;
              srcRect.w = srcW;
              srcRect.h = srcH;

              srcPtr = &srcRect;
            }

          dstRect.x = x;
          dstRect.y = y;

          if (srcW > 0)
            {
              dstRect.w = srcW;
              dstRect.h = srcH;
            }
          else
            {
              dstRect.w = uSur->w;
              dstRect.h = uSur->h;
            }

          if (createPendingSurface ())
            {
              Display::lockSDL ();
              if (SDL_UpperBlit (uSur, srcPtr, pending, &dstRect) < 0)
                {
                  clog << "SDLSurface::blit SDL error: '";
                  clog << SDL_GetError () << "'" << endl;
                }
              Display::unlockSDL ();
            }
        }

      Thread::mutexUnlock (&pMutex);

      if (src != this)
        {
          Thread::mutexUnlock (&((SDLSurface *)src)->sMutex);
          Thread::mutexUnlock (&((SDLSurface *)src)->pMutex);
        }
    }
  else
    {
      clog << "SDLSurface::blit(" << this << ") Warning! ";
      clog << "Can't blit: ";
      clog << "underlying surface is NULL. Destination SDLSurface ";
      clog << "address would be '" << src << "'" << endl;
    }

  Thread::mutexUnlock (&sMutex);
}

void
SDLSurface::getStringExtents (const char *text, int *w, int *h)
{
  if (iFont != NULL)
    {
    //   ((IFontProvider*)iFont)->getStringExtents (text, w, h);
    }
  else
    {
      clog << "SDLSurface::getStringExtends Warning! ";
      clog << "Can't get string info: ";
      clog << "font provider is NULL" << endl;
    }
}

void
SDLSurface::setClip (int x, int y, int w, int h)
{
  SDL_Rect rect;

  Thread::mutexLock (&sMutex);
  if (sur != NULL)
    {
      rect.x = x;
      rect.y = y;
      rect.w = w;
      rect.h = h;

      Thread::mutexLock (&pMutex);
      if (createPendingSurface ())
        {
          SDL_SetClipRect (pending, &rect);
        }
      Thread::mutexUnlock (&pMutex);
    }
  else
    {
      clog << "SDLSurface::setClip Warning! NULL underlying surface";
      clog << endl;
    }
  Thread::mutexUnlock (&sMutex);
}

void
SDLSurface::getSize (int *w, int *h)
{
  Thread::mutexLock (&sMutex);
  if (sur != NULL)
    {
      *w = sur->w;
      *h = sur->h;
    }
  else if (parent != NULL)
    {
      *w = ((SDLWindow *)parent)->getW ();
      *h = ((SDLWindow *)parent)->getH ();
    }
  else
    {
      *w = *h = 0;
      clog << "SDLSurface::getSize Warning! NULL underlying surface and";
      clog << " parent";
      clog << endl;
    }
  Thread::mutexUnlock (&sMutex);
}

string
SDLSurface::getDumpFileUri ()
{
  string uri;
  Thread::mutexLock (&sMutex);
  if (sur == NULL)
    {
      clog << "DFBSurface::getDumpFileUri Warning! ";
      clog << "Can't dump surface bitmap: ";
      clog << "internal surface is NULL" << endl;

      uri = "";
    }
  else
    {
      uri = string (g_get_tmp_dir ()) + "/" + "dump_0000.bmp";

      remove (deconst (char *, uri.c_str ()));
      SDL_SaveBMP (sur, uri.c_str ());
    }
  Thread::mutexUnlock (&sMutex);
  return uri;
}

GINGA_MB_END
