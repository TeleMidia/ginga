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
#include "ginga-color-table.h"
#include "NclFormatterRegion.h"

#include "NclCascadingDescriptor.h"
#include "NclFormatterLayout.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

//#include "util/Color.h"
//using namespace ::ginga::util;

GINGA_FORMATTER_BEGIN

NclFormatterRegion::NclFormatterRegion (const string &objectId, void *descriptor,
                                        void *layoutManager)
{
  this->layoutManager = layoutManager;
  this->objectId = objectId;
  this->descriptor = descriptor;

  initializeNCMRegion ();

  this->outputDisplay = 0;
  this->imVisible = false;
  this->externHandler = false;
  this->focusState = NclFormatterRegion::UNSELECTED;
  this->focusBorderColor = NULL;
  this->focusBorderWidth = 0;
  this->focusComponentSrc = "";
  this->selBorderColor = NULL;
  this->selBorderWidth = 0;
  this->selComponentSrc = "";
  this->chromaKey = NULL;
  this->bgColor = NULL;
  this->transitionIn = "";
  this->transitionOut = "";
  this->abortTransitionIn = false;
  this->abortTransitionOut = false;
  this->focusIndex = "";
  this->moveUp = "";
  this->moveDown = "";
  this->moveLeft = "";
  this->moveRight = "";
  this->plan = "";
  this->zIndex = -1;

  Thread::mutexInit (&mutex);
  Thread::mutexInit (&mutexT);
  Thread::mutexInit (&mutexFI);

  // TODO: look for descriptor parameters overriding region attributes
  string value;
  value = ((NclCascadingDescriptor *)descriptor)
              ->getParameterValue ("transparency");

  this->setTransparency (value);

  value = ((NclCascadingDescriptor *)descriptor)
              ->getParameterValue ("background");

  if (xstrchomp (value) != "")
    {
      if (value.find (",") == std::string::npos)
        {
          this->setBackgroundColor (value);
        }
      else
        {
          SDL_Color *bg = NULL;
          vector<string> params;

          params = xstrsplit (xstrchomp (value), ',');
          if (params.size () == 3)
            {
              bg = new SDL_Color();
              bg->r =  xstrtouint8 (params[0], 10);
              bg->g =  xstrtouint8 (params[1], 10);
              bg->b =  xstrtouint8 (params[2], 10);
              bg->a = 255;
              setBackgroundColor (bg);
            }
          else if (params.size () == 4)
            {
              bg = new SDL_Color();
              bg->r =  xstrtouint8 (params[0], 10);
              bg->g =  xstrtouint8 (params[1], 10);
              bg->b =  xstrtouint8 (params[2], 10);
              bg->a =  xstrtouint8 (params[3], 10);
              setBackgroundColor (bg);
            }
        }
    }

  value = ((NclCascadingDescriptor *)descriptor)->getParameterValue ("fit");

  this->setFit (value);

  value = ((NclCascadingDescriptor *)descriptor)
              ->getParameterValue ("scroll");

  this->setScroll (value);

  value = ((NclCascadingDescriptor *)descriptor)
              ->getParameterValue ("chromakey");

  this->setChromaKey (value);

  value = ((NclCascadingDescriptor *)descriptor)
              ->getParameterValue ("rgbChromakey");

  if (value == "")
    {
      value = ((NclCascadingDescriptor *)descriptor)
                  ->getParameterValue ("x-rgbChromakey");
    }

  this->setRgbChromaKey (value);

  // TODO: methods setTransIn and setTransOut
  value = ((NclCascadingDescriptor *)descriptor)
              ->getParameterValue ("transitionIn");

  if (value != "")
    {
      transitionIn = value;
    }

  value = ((NclCascadingDescriptor *)descriptor)
              ->getParameterValue ("transitionOut");

  if (value != "")
    {
      transitionOut = value;
    }

  // TODO: fit and scroll
}

NclFormatterRegion::~NclFormatterRegion ()
{
  lock ();
  lockFocusInfo ();
  layoutManager = NULL;
  descriptor = NULL;

  originalRegion = NULL;
  if (ncmRegion != NULL)
    {
      // delete ncmRegion;
      ncmRegion = NULL;
    }

  disposeOutputDisplay ();

  if (focusBorderColor != NULL)
    {
      delete focusBorderColor;
      focusBorderColor = NULL;
    }

  if (selBorderColor != NULL)
    {
      delete selBorderColor;
      selBorderColor = NULL;
    }

  if (bgColor != NULL)
    {
      delete bgColor;
      bgColor = NULL;
    }

  if (chromaKey != NULL)
    {
      delete chromaKey;
      chromaKey = NULL;
    }

  unlock ();
  Thread::mutexDestroy (&mutex);
  lockTransition ();
  unlockTransition ();
  Thread::mutexDestroy (&mutexT);
  unlockFocusInfo ();
  Thread::mutexDestroy (&mutexFI);
}

void
NclFormatterRegion::initializeNCMRegion ()
{
  originalRegion = NULL;

  if (descriptor != NULL)
    {
      originalRegion = ((NclCascadingDescriptor *)descriptor)->getRegion ();
    }

  if (originalRegion != NULL)
    {
      ncmRegion = originalRegion->cloneRegion ();
    }
  else
    {
      ncmRegion = NULL;
    }
}



void
NclFormatterRegion::setZIndex (int zIndex)
{
  string layoutId;
  double cvtZIndex;

  this->zIndex = zIndex;

  if (ncmRegion != NULL)
    {
      ncmRegion->setZIndex (zIndex);
    }

  if (originalRegion != NULL && layoutManager != NULL)
    {
      layoutId = originalRegion->getId ();

      cvtZIndex = ((NclFormatterLayout *)layoutManager)
                      ->refreshZIndex (this, layoutId, zIndex, plan);
      (void) cvtZIndex;

      if (outputDisplay != 0)
        {
          outputDisplay->setZ (zIndex);
        }
    }
}

int
NclFormatterRegion::getZIndex ()
{
  return zIndex;
}

void
NclFormatterRegion::setPlan (const string &plan)
{
  if (this->plan != plan)
    {
      this->plan = plan;

      if (zIndex < 0)
        {
          zIndex = ncmRegion->getZIndexValue ();
        }

      setZIndex (this->zIndex);
    }
}

string
NclFormatterRegion::getPlan ()
{
  return plan;
}

void
NclFormatterRegion::setFocusIndex (const string &focusIndex)
{
  this->focusIndex = focusIndex;
}

string
NclFormatterRegion::getFocusIndex ()
{
  if (focusIndex != "")
    {
      return focusIndex;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getFocusIndex ();
    }

  return "";
}

void
NclFormatterRegion::setMoveUp (const string &moveUp)
{
  this->moveUp = moveUp;
}

string
NclFormatterRegion::getMoveUp ()
{
  if (moveUp != "")
    {
      return moveUp;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getMoveUp ();
    }

  return "";
}

void
NclFormatterRegion::setMoveDown (const string &moveDown)
{
  this->moveDown = moveDown;
}

string
NclFormatterRegion::getMoveDown ()
{
  if (moveDown != "")
    {
      return moveDown;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getMoveDown ();
    }

  return "";
}

void
NclFormatterRegion::setMoveLeft (const string &moveLeft)
{
  this->moveLeft = moveLeft;
}

string
NclFormatterRegion::getMoveLeft ()
{
  if (moveLeft != "")
    {
      return moveLeft;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getMoveLeft ();
    }

  return "";
}

void
NclFormatterRegion::setMoveRight (const string &moveRight)
{
  this->moveRight = moveRight;
}

string
NclFormatterRegion::getMoveRight ()
{
  if (moveRight != "")
    {
      return moveRight;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getMoveRight ();
    }

  return "";
}

void
NclFormatterRegion::setFocusBorderColor (SDL_Color *focusBorderColor)
{
  lockFocusInfo ();
  if (this->focusBorderColor == focusBorderColor)
    {
      unlockFocusInfo ();
      return;
    }

  if (this->focusBorderColor != NULL)
    {
      delete this->focusBorderColor;
      this->focusBorderColor = NULL;
    }

  if (focusBorderColor != NULL)
    {
      this->focusBorderColor = focusBorderColor;
    }
  unlockFocusInfo ();
}

SDL_Color *
NclFormatterRegion::getFocusBorderColor ()
{
  SDL_Color *bColor = NULL;

  lockFocusInfo ();
  if (focusBorderColor != NULL)
    {
      bColor = focusBorderColor;
    }
  else if (descriptor != NULL)
    {
      bColor
          = ((NclCascadingDescriptor *)descriptor)->getFocusBorderColor ();
    }
  unlockFocusInfo ();

  return bColor;
}

void
NclFormatterRegion::setFocusBorderWidth (int focusBorderWidth)
{
  this->focusBorderWidth = focusBorderWidth;
}

int
NclFormatterRegion::getFocusBorderWidth ()
{
  if (focusBorderWidth != 0)
    {
      return focusBorderWidth;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getFocusBorderWidth ();
    }

  return 0;
}

void
NclFormatterRegion::setFocusComponentSrc (const string &focusComponentSrc)
{
  this->focusComponentSrc = focusComponentSrc;
}

string
NclFormatterRegion::getFocusComponentSrc ()
{
  if (focusComponentSrc != "")
    {
      return focusComponentSrc;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getFocusSrc ();
    }

  return "";
}

void
NclFormatterRegion::setSelBorderColor (SDL_Color *selBorderColor)
{
  lockFocusInfo ();
  if (this->selBorderColor == selBorderColor)
    {
      unlockFocusInfo ();
      return;
    }

  if (this->selBorderColor != NULL)
    {
      delete this->selBorderColor;
      this->selBorderColor = NULL;
    }

  if (selBorderColor != NULL)
    {
      this->selBorderColor = selBorderColor;
    }

  unlockFocusInfo ();
}

SDL_Color *
NclFormatterRegion::getSelBorderColor ()
{
  SDL_Color *sColor = NULL;

  lockFocusInfo ();
  if (selBorderColor != NULL)
    {
      sColor = selBorderColor;
    }
  else if (descriptor != NULL)
    {
      sColor = ((NclCascadingDescriptor *)descriptor)->getSelBorderColor ();
    }

  unlockFocusInfo ();

  return sColor;
}

void
NclFormatterRegion::setSelBorderWidth (int selBorderWidth)
{
  this->selBorderWidth = selBorderWidth;
}

int
NclFormatterRegion::getSelBorderWidth ()
{
  if (selBorderWidth != 0)
    {
      return selBorderWidth;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getSelBorderWidth ();
    }

  return 0;
}

void
NclFormatterRegion::setSelComponentSrc (const string &selComponentSrc)
{
  this->selComponentSrc = selComponentSrc;
}

string
NclFormatterRegion::getSelComponentSrc ()
{
  if (selComponentSrc != "")
    {
      return selComponentSrc;
    }
  else if (descriptor != NULL)
    {
      return ((NclCascadingDescriptor *)descriptor)->getSelectionSrc ();
    }

  return "";
}

void
NclFormatterRegion::setFocusInfo (SDL_Color *focusBorderColor,
                                  int focusBorderWidth,
                                  const string &focusComponentSrc,
                                  SDL_Color *selBorderColor, int selBorderWidth,
                                  const string &selComponentSrc)
{
  setFocusBorderColor (focusBorderColor);
  setFocusBorderWidth (focusBorderWidth);
  setFocusComponentSrc (focusComponentSrc);
  setSelBorderColor (selBorderColor);
  setSelBorderWidth (selBorderWidth);
  setSelComponentSrc (selComponentSrc);
}

void *
NclFormatterRegion::getLayoutManager ()
{
  return layoutManager;
}

SDLWindow*
NclFormatterRegion::getOutputId ()
{
  return outputDisplay;
}

void
NclFormatterRegion::meetComponent (arg_unused (int width), arg_unused (int height), arg_unused (int prefWidth),
                                   arg_unused (int prefHeight))
{
}

void
NclFormatterRegion::sliceComponent (arg_unused (int width), arg_unused (int height), arg_unused (int prefWidth),
                                    arg_unused (int prefHeight))
{
}

void
NclFormatterRegion::updateCurrentComponentSize ()
{
}

void
NclFormatterRegion::updateRegionBounds ()
{
  sizeRegion ();
  if (focusState == NclFormatterRegion::UNSELECTED)
    {
      unselect ();
    }
  else
    { // is focused (at least)
      // if is focused and selected
      if (focusState == NclFormatterRegion::SELECTED)
        {
          setSelection (true);
        }
      else if (focusState == NclFormatterRegion::FOCUSED)
        {
          setFocus (true);
        }
    }
}

void
NclFormatterRegion::sizeRegion ()
{
  int left = 0;
  int top = 0;
  int width = 0;
  int height = 0;

  if (ncmRegion != NULL)
    {
      left = ncmRegion->getLeftInPixels ();
      top = ncmRegion->getTopInPixels ();
      width = ncmRegion->getWidthInPixels ();
      height = ncmRegion->getHeightInPixels ();
    }

  /*clog << "NclFormatterRegion::sizeRegion windowAdd = '" << outputDisplay;
  clog << "' x = '" << left;
  clog << "' y = '" << top;
  clog << "' w = '" << width;
  clog << "' h = '" << height << "'" << endl;*/

  if (left < 0)
    left = 0;

  if (top < 0)
    top = 0;

  if (width <= 0)
    width = 1;

  if (height <= 0)
    height = 1;

  lock ();
  if (outputDisplay != 0)
    {
      outputDisplay->setBounds (left, top, width, height);
    }
  unlock ();
}

LayoutRegion *
NclFormatterRegion::getLayoutRegion ()
{
  return ncmRegion;
}

LayoutRegion *
NclFormatterRegion::getOriginalRegion ()
{
  return originalRegion;
}

SDLWindow*
NclFormatterRegion::prepareOutputDisplay (double cvtIndex)
{
  lock ();

  if (outputDisplay == 0)
    {
      string title;
      int left = 0;
      int top = 0;
      int width = 0;
      int height = 0;

      if (ncmRegion == NULL)
        {
          title = objectId;
        }
      else
        {
          left = ncmRegion->getAbsoluteLeft ();
          top = ncmRegion->getAbsoluteTop ();
          width = ncmRegion->getWidthInPixels ();
          height = ncmRegion->getHeightInPixels ();
        }

      if (left < 0)
        left = 0;

      if (top < 0)
        top = 0;

      if (width <= 0)
        width = 1;

      if (height <= 0)
        height = 1;

    

      if (!externHandler)
        {
          outputDisplay = Ginga_Display
            ->createWindow (left, top, width, height, (int)cvtIndex);
        }

      clog << "NclFormatterRegion::prepareOutputDisplay '" << outputDisplay;
      clog << "' created with ";
      clog << "left   = '" << left << "' ";
      clog << "top    = '" << top << "' ";
      clog << "width  = '" << width << "' ";
      clog << "height = '" << height << "' ";
      clog << endl;

      lockFocusInfo ();
      if (bgColor != NULL)
        {
          clog << "NclFormatterRegion::prepareOutputDisplay bg color ";

          clog << "r = '" << bgColor->r << "', ";
          clog << "g = '" << bgColor->g << "' and ";
          clog << "b = '" << bgColor->b << "' ";
          clog << endl;

          outputDisplay->setBgColor (*bgColor);
        }
      unlockFocusInfo ();
     
    }
  else
    {
      clog << "NclFormatterRegion::prepareOutputDisplay Warning!";
      clog << "window != NULL" << endl;
    }


  unlock ();

  return outputDisplay;
}

void
NclFormatterRegion::showContent ()
{
  string value;
  NclCascadingDescriptor *desc;
  int transitionType;
  unsigned int i;
  vector<Transition *> *transitions;
  Transition *transition;
  TransInfo *t;
  pthread_t threadId_;

  lockTransition ();
  desc = ((NclCascadingDescriptor *)descriptor);
  value = desc->getParameterValue ("visible");
  abortTransitionIn = false;
  abortTransitionOut = true;
  if (value != "false")
    {
      imVisible = true;
      transitions = desc->getInputTransitions ();

      if (!transitions->empty ())
        {
          for (i = 0; i < transitions->size (); i++)
            {
              transition = (*transitions)[i];
              transitionType = transition->getType ();
              if (transitionType == Transition::TYPE_FADE)
                {
                  // fade(transition, true);
                  t = new TransInfo;
                  t->fr = this;
                  t->t = transition;

                  // show with fade transition type
                  pthread_create (&threadId_, 0, NclFormatterRegion::fadeT,
                                  (void *)t);

                  pthread_detach (threadId_);
                  unlockTransition ();
                  return;
                }
              else if (transitionType == Transition::TYPE_BARWIPE)
                {
                  // barWipe(transition, true);
                  t = new TransInfo;
                  t->fr = this;
                  t->t = transition;

                  // show with barwipe transition type
                  pthread_create (&threadId_, 0,
                                  NclFormatterRegion::barWipeT, (void *)t);

                  pthread_detach (threadId_);
                  unlockTransition ();
                  return;
                }
            }
        }

      unlockTransition ();
      setRegionVisibility (true);
      /*clog << "NclFormatterRegion::showContent '" << desc->getId();
      clog << "'" << endl;*/
    }
  else
    {
      unlockTransition ();
    }
}

void
NclFormatterRegion::hideContent ()
{
  setRegionVisibility (false);
  abortTransitionIn = true;
  abortTransitionOut = true;

  lock ();
  disposeOutputDisplay ();
  unlock ();
}

void
NclFormatterRegion::performOutTrans ()
{
  vector<Transition *> *transitions;
  unsigned int i;
  Transition *transition;
  int transitionType;
  bool currentVisibility;

  abortTransitionIn = true;
  abortTransitionOut = false;
  focusState = NclFormatterRegion::UNSELECTED;

  lockTransition ();

  currentVisibility = imVisible;
  if (currentVisibility)
    {
      transitions
          = ((NclCascadingDescriptor *)descriptor)->getOutputTransitions ();

      if (!transitions->empty ())
        {
          for (i = 0; i < transitions->size (); i++)
            {
              transition = (*transitions)[i];
              transitionType = transition->getType ();
              if (transitionType == Transition::TYPE_FADE)
                {
                  // hide with fade transition type
                  fade (transition, false);
                  unlockTransition ();
                  return;
                }
              else if (transitionType == Transition::TYPE_BARWIPE)
                {
                  // hide with bar wipe transition type
                  barWipe (transition, false);
                  unlockTransition ();
                  return;
                }
            }
        }
    }

  unlockTransition ();
}

double
NclFormatterRegion::getOutTransDur ()
{
  vector<Transition *> *transitions;
  unsigned int i;
  Transition *transition;
  int transitionType;

  lockTransition ();

  transitions
      = ((NclCascadingDescriptor *)descriptor)->getOutputTransitions ();

  if (!transitions->empty ())
    {
      for (i = 0; i < transitions->size (); i++)
        {
          transition = (*transitions)[i];
          transitionType = transition->getType ();

          switch (transitionType)
            {
            case Transition::TYPE_FADE:
              unlockTransition ();
              return transition->getDur ();

            case Transition::TYPE_BARWIPE:
              unlockTransition ();
              return transition->getDur ();

            default:
              g_assert_not_reached ();
            }
        }
    }

  unlockTransition ();
  return 0.0;
}

void
NclFormatterRegion::setRegionVisibility (bool visible)
{
  lock ();
  if (externHandler)
    {
      unlock ();
      return;
    }

  if (outputDisplay != 0)
    {
      if (!visible)
        {
          clog << "NclFormatterRegion::setRegionVisibility (" << this;
          clog << ") object '" << objectId << "' display '";
          clog << outputDisplay;
          clog << "' HIDE" << endl;

          outputDisplay->hide ();
        }
      else
        {
          clog << "NclFormatterRegion::setRegionVisibility (" << this;
          clog << ") object '" << objectId << "' display '";
          clog << outputDisplay;
          clog << "' SHOW" << endl;

          outputDisplay->show ();
        }
    }
  imVisible = visible;
  unlock ();
}

void
NclFormatterRegion::disposeOutputDisplay ()
{
  if (outputDisplay != 0)
    {
      if (!externHandler)
        {
          Ginga_Display->destroyWindow (outputDisplay);
        }
      outputDisplay = 0;
    }
}

void
NclFormatterRegion::setGhostRegion (bool ghost)
{
  lock ();
  if (outputDisplay != 0 && !externHandler)
    {
      outputDisplay->setGhostWindow (ghost);
    }
  unlock ();
}

bool
NclFormatterRegion::isVisible ()
{
  return imVisible;
}

short
NclFormatterRegion::getFocusState ()
{
  return focusState;
}

bool
NclFormatterRegion::setSelection (bool selOn)
{

  if (selOn && focusState == NclFormatterRegion::SELECTED)
    {
      return false;
    }

  if (selOn)
    {
      focusState = NclFormatterRegion::SELECTED;
    
      lock ();
      if (outputDisplay != 0 && !externHandler)
        {
          lockFocusInfo ();
          if (selComponentSrc == "")
            {
             // outputDisplay->validate ();
            }

          if (selBorderColor != NULL)
            {
              outputDisplay->setBorder (*selBorderColor,
                  selBorderWidth);
            }

          unlockFocusInfo ();
        }
      unlock ();
    }
  else
    {
      unselect ();
    }

  return selOn;
}

void
NclFormatterRegion::setFocus (bool focusOn)
{
//  SDLSurface* focusSurface;

  if (focusOn)
    {
      focusState = NclFormatterRegion::FOCUSED;

      if (focusComponentSrc != "")
        {
          lock ();
/*
          if (focusSurface != 0)
            {
              if (outputDisplay != 0 && !externHandler)
                {
                //  outputDisplay->renderFrom (
                //                        focusSurface);
                }
              delete focusSurface;
            }  */
          unlock ();
        }

      lock ();
      if (outputDisplay != 0 && !externHandler)
        {
          lockFocusInfo ();
          if (focusComponentSrc == "")
            {
             // outputDisplay->validate ();
            }

          if (focusBorderColor != NULL)
            {
              outputDisplay->setBorder (*focusBorderColor,
                                        focusBorderWidth);
            }

          unlockFocusInfo ();
        }
      unlock ();
    }
  else
    {
      unselect ();
    }
}

void
NclFormatterRegion::unselect ()
{
  focusState = NclFormatterRegion::UNSELECTED;

  lock ();
  if (outputDisplay != 0 && !externHandler)
    {
      SDL_Color c = {0, 0, 0, 0};
      outputDisplay->setBorder (c, 0);
     
    //  outputDisplay->validate ();
    }
  unlock ();
}

SDL_Color *
NclFormatterRegion::getBackgroundColor ()
{
  return bgColor;
}

void
NclFormatterRegion::barWipe (arg_unused (Transition *transition),
                             arg_unused (bool isShowEffect))
{
  // if (transitionSubType == Transition::SUBTYPE_BARWIPE_LEFTTORIGHT)
  //   {
  //   }
  // else if (transitionSubType == Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM)
  //   {
  //   }
}

void *
NclFormatterRegion::barWipeT (arg_unused (void *ptr))
{
  return NULL;
}

void
NclFormatterRegion::fade (arg_unused (Transition *transition),
                          arg_unused (bool isShowEffect))
{
}

void *
NclFormatterRegion::fadeT (arg_unused (void *ptr))
{
  return NULL;
}

void
NclFormatterRegion::lock ()
{
  Thread::mutexLock (&mutex);
}

void
NclFormatterRegion::unlock ()
{
  Thread::mutexUnlock (&mutex);
}

void
NclFormatterRegion::lockTransition ()
{
  Thread::mutexLock (&mutexT);
}

void
NclFormatterRegion::unlockTransition ()
{
  Thread::mutexUnlock (&mutexT);
}

void
NclFormatterRegion::lockFocusInfo ()
{
  Thread::mutexLock (&mutexFI);
}

void
NclFormatterRegion::unlockFocusInfo ()
{
  Thread::mutexUnlock (&mutexFI);
}

void
NclFormatterRegion::setTransparency (const string &strTrans)
{
  double trans;
  if (strTrans == "")
    {
      trans = 0.0;
    }
  else
    {
      trans = xstrtod (strTrans);
    }

  setTransparency (trans);
}

double
NclFormatterRegion::getTransparency ()
{
  return this->transparency;
}

void
NclFormatterRegion::setTransparency (double transparency)
{
  lock ();
  if (transparency < 0.0)
    {
      this->transparency = 0.0;
    }
  else if (transparency > 1.0)
    {
      this->transparency = 1.0;
    }
  else
    {
      this->transparency = transparency;
    }

  unlock ();
}

void
NclFormatterRegion::setBackgroundColor (const string &c)
{
    SDL_Color *bg = new SDL_Color();
    ginga_color_input_to_sdl_color(c,bg);
    bg->a = (guint8)(transparency * 255);
    setBackgroundColor (bg);
}

void
NclFormatterRegion::setBackgroundColor (SDL_Color *color)
{
  lockFocusInfo ();
  if (color != bgColor && bgColor != NULL)
    {
      delete bgColor;
      bgColor = NULL;
    }

  this->bgColor = color;
  unlockFocusInfo ();
}

void
NclFormatterRegion::setChromaKey (const string &value)
{
  if (value != "")
    {
      if (value == "black")
        {
             ginga_color_input_to_sdl_color( "#0", this->chromaKey);
        }
      else
        {
             ginga_color_input_to_sdl_color( value, this->chromaKey);
        }
    }
}

void
NclFormatterRegion::setRgbChromaKey (const string &value)
{
  if (value != "")
    {
      this->chromaKey = new SDL_Color();
      ginga_color_input_to_sdl_color(value, this->chromaKey);   
    }
}

void
NclFormatterRegion::setFit (const string &value)
{
  if (value != "")
    {
      setFit (DescriptorUtil::getFitCode (value));
    }
  else
    {
      setFit (Descriptor::FIT_FILL);
    }
}

void
NclFormatterRegion::setFit (short value)
{
  if (value < 0)
    {
      fit = Descriptor::FIT_FILL;
    }
  else
    {
      fit = value;
    }
}

void
NclFormatterRegion::setScroll (const string &value)
{
  if (value != "")
    {
      setScroll (DescriptorUtil::getScrollCode (value));
    }
  else
    {
      setScroll (Descriptor::SCROLL_NONE);
    }
}

void
NclFormatterRegion::setScroll (short value)
{
  if (value < 0)
    {
      scroll = Descriptor::SCROLL_NONE;
    }
  else
    {
      scroll = value;
    }
}

GINGA_FORMATTER_END
