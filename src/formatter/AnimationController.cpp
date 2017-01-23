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
#include "AnimationController.h"

GINGA_FORMATTER_BEGIN

AnimationController::AnimationController (NclExecutionObject *execObj,
                                          AdapterPlayerManager *pManager,
                                          AdapterFormatterPlayer *player,
                                          NclAttributionEvent *event,
                                          string value, Animation *anim)
{
  this->execObj = execObj;
  this->pManager = pManager;
  this->player = player;
  this->event = event;

  this->params = split (xstrchomp (value), ",");
  this->propName = (event->getAnchor ())->getPropertyName ();

  this->duration = ::ginga::util::stof (anim->getDuration ());
  this->stepSize = (int)::ginga::util::stof (anim->getBy ());

  this->targetRegion = NULL;

  this->previousValues = "";
}

AnimationController::~AnimationController ()
{
  isDeleting = true;

  pManager = NULL;
  player = NULL;

  if (targetRegion != NULL)
    {
      delete targetRegion;
      targetRegion = NULL;
    }

  if (params != NULL)
    {
      delete params;
      params = NULL;
    }

  targetValues.clear ();
  strTargetValues.clear ();
}

void
AnimationController::startAnimation (NclExecutionObject *execObj,
                                     AdapterPlayerManager *pManager,
                                     AdapterFormatterPlayer *player,
                                     NclAttributionEvent *event,
                                     string value, Animation *anim)
{
  AnimationController *animControl = new AnimationController (
      execObj, pManager, player, event, value, anim);

  animControl->startThread ();
}

/**
 * ATTENTION: this method calls "delete this" when the animation is done.
 */
void
AnimationController::run ()
{
  bool done = false;

  // if the property name is a group, this function will ungroup them.
  if (!ungroupProperty ())
    {
      clog << "AnimationController::run : [WARNING] the animating can ";
      clog << "not be started. The parameters passed by the NCL program ";
      clog << "are wrong." << endl;

      return;
    }

  if (loadInitValues () && loadTargetValues ())
    {
      this->initTime = getCurrentTimeMillis ();
      // the animation can be performed
      while (!done)
        {
          done = animeStep ();
          Thread::mSleep (60);
        }
    }
  else
    {
      clog << "AnimationController::run() : problem with loadInitValues ";
      clog << "or loadTargeValues (false returned)" << endl;
    }

  this->event->stop ();
  delete this;
}

bool
AnimationController::animeStep ()
{
  double time;
  int factor = 1;
  unsigned int i;
  vector<double> *nextValues = new vector<double>;
  string paramValue = "";

  time = getCurrentTimeMillis ();

  //		clog << "AnimationController::animeStep : Next Values = ";
  for (i = 0; i < initValues.size (); i++)
    {
      nextValues->push_back (getNextStepValue (
          this->initValues[i], this->targetValues[i], factor, time,
          this->initTime, (this->duration * 1000), stepSize));
      //			clog << (*nextValues)[i] << " ";
    }
  //		clog << endl;

  if (time >= (this->initTime + (this->duration * 1000)))
    {
      nextValues->clear ();
      delete nextValues;
      paramValue = "";

      for (i = 0; i < targetValues.size (); i++)
        {
          if (i)
            {
              paramValue += ",";
            }

          paramValue += itos (targetValues[i]);
        }

      if (player == NULL || !pManager->hasPlayer (player)
          || player->setPropertyValue (event, paramValue))
        {
          return true;
        }
    }
  else
    {
      for (i = 0; i < nextValues->size (); i++)
        {
          if (i)
            {
              paramValue += ",";
            }

          paramValue += itos ((*nextValues)[i]);
        }

      if (paramValue == previousValues)
        {
          return false;
        }

      previousValues = paramValue;

      if (player == NULL || !pManager->hasPlayer (player))
        {
          nextValues->clear ();
          delete nextValues;

          return true;
        }

      player->setPropertyValue (event, paramValue);

      nextValues->clear ();
      delete nextValues;
    }

  return false;
}

bool
AnimationController::loadInitValues ()
{
  double propValue;
  NclCascadingDescriptor *descriptor = NULL;
  unsigned int i;
  string value = "";

  this->initValues.clear ();
  if (isExecutionObjectProperty (this->propName))
    {
      descriptor = this->execObj->getDescriptor ();

      if (descriptor == NULL || descriptor->getFormatterRegion () == NULL)
        {
          clog << "AnimationController::loadTargetValues : load target";
          clog << " could not be performed. Descriptor or ";
          clog << "formatterRegion is NULL." << endl;

          return false;
        }

      this->targetRegion
          = descriptor->getFormatterRegion ()->getLayoutRegion ();
      for (i = 0; i < this->propertySingleNames.size (); i++)
        {
          value = player->getPropertyValue (this->propertySingleNames[i]);

          if (value == "")
            {
              value = execObj->getPropertyValue (
                  this->propertySingleNames[i]);
            }
          propValue = ::ginga::util::stof (value);

          clog << "AnimationController::loadInitValues (execObj): ";
          clog << propertySingleNames[i] << " value = '";
          clog << propValue << "'" << endl;

          this->initValues.push_back (getSinglePropertyTarget (i));
        }
    }
  return true;
}

bool
AnimationController::loadTargetValues ()
{
  NclCascadingDescriptor *descriptor = NULL;
  NclFormatterRegion *region = NULL;
  unsigned int i;

  this->targetValues.clear ();

  if (isExecutionObjectProperty (this->propName))
    {
      descriptor = this->execObj->getDescriptor ();

      if (descriptor == NULL || descriptor->getFormatterRegion () == NULL)
        {
          clog << "AnimationController::loadTargetValues : load target";
          clog << " could not be performed. Descriptor or ";
          clog << "formatterRegion is NULL." << endl;

          return false;
        }

      region = descriptor->getFormatterRegion ();
      this->initRegion = region->getLayoutRegion ();

      clog << "AnimationController::loadTargetValues : The animation ";
      clog << "should start with values -";
      clog << " top: " << initRegion->getTopInPixels ();
      clog << " left: " << initRegion->getLeftInPixels ();
      clog << " width: " << initRegion->getWidthInPixels ();
      clog << " height: " << initRegion->getHeightInPixels ();
      clog << " right: " << initRegion->getRightInPixels ();
      clog << " bottom: " << initRegion->getBottomInPixels ();
      clog << endl;

      this->targetRegion = initRegion->copyRegion ();

      updateTargetRegion ();

      clog << "AnimationController::loadTargetValues : The animation ";
      clog << "should finish with values -";
      clog << " top: " << targetRegion->getTopInPixels ();
      clog << " left: " << targetRegion->getLeftInPixels ();
      clog << " width: " << targetRegion->getWidthInPixels ();
      clog << " height: " << targetRegion->getHeightInPixels ();
      clog << " right: " << targetRegion->getRightInPixels ();
      clog << " bottom: " << targetRegion->getBottomInPixels ();
      clog << endl;
    }

  for (i = 0; i < propertySingleNames.size (); i++)
    {
      this->targetValues.push_back (getSinglePropertyTarget (i));
    }

  return true;
}

bool
AnimationController::ungroupProperty ()
{
  int i;

  if (this->propName == "")
    {
      return false;
    }

  this->isExecObjAnim = false;

  this->targetValues.clear ();
  this->propertySingleNames.clear ();

  if (this->propName == "size")
    {
      if (this->params->size () == 2)
        {
          this->propertySingleNames.push_back ("width");
          this->propertySingleNames.push_back ("height");

          this->strTargetValues.push_back ((*params)[0]);
          this->strTargetValues.push_back ((*params)[1]);
        }
      else
        {
          return false;
        }
    }
  else if (this->propName == "location")
    {
      if (this->params->size () == 2)
        {
          this->propertySingleNames.push_back ("left");
          this->propertySingleNames.push_back ("top");

          this->strTargetValues.push_back ((*params)[0]);
          this->strTargetValues.push_back ((*params)[1]);
        }
      else
        {
          return false;
        }
    }
  else if (this->propName == "bounds")
    {
      if (this->params->size () == 4)
        {
          this->propertySingleNames.push_back ("left");
          this->propertySingleNames.push_back ("top");
          this->propertySingleNames.push_back ("width");
          this->propertySingleNames.push_back ("height");

          for (i = 0; i < 4; i++)
            {
              this->strTargetValues.push_back ((*params)[i]);
            }
        }
      else
        {
          return false;
        }
    }
  else
    {
      if (this->params->size () == 1)
        {
          this->propertySingleNames.push_back (propName);
          this->strTargetValues.push_back ((*params)[0]);
        }
      else
        {
          return false;
        }
    }

  return true;
}

bool
AnimationController::isExecutionObjectProperty (string name)
{
  if (name == "bounds" || name == "location" || name == "size"
      || propName == "left" || propName == "top" || propName == "bottom"
      || propName == "right" || propName == "width" || propName == "height"
      || propName == "transparency")

    return true;

  return false;
}

// TODO: should return false if any problem occurs
bool
AnimationController::updateTargetRegion ()
{
  bool percent;
  string param, value;
  unsigned int i;

  for (i = 0; i < strTargetValues.size (); i++)
    {
      param = propertySingleNames[i];
      value = strTargetValues[i];
      percent = isPercentualValue (value);

      clog << "AnimationController::updateTargetRegion ";
      clog << "param   = '" << param << "' " << endl;

      clog << "AnimationController::updateTargetRegion ";
      clog << "value   = '" << value << "' " << endl;

      clog << "AnimationController::updateTargetRegion ";
      clog << "percent = '" << percent << "' " << endl;

      if (percent)
        {
          if (param == "left")
            {
              targetRegion->setTargetLeft (getPercentualValue (value),
                                           true);
            }
          else if (param == "top")
            {
              targetRegion->setTargetTop (getPercentualValue (value), true);
            }
          else if (param == "width")
            {
              targetRegion->setTargetWidth (getPercentualValue (value),
                                            true);
            }
          else if (param == "height")
            {
              targetRegion->setTargetHeight (getPercentualValue (value),
                                             true);
            }
          else if (param == "bottom")
            {
              targetRegion->setTargetBottom (getPercentualValue (value),
                                             true);
            }
          else if (param == "right")
            {
              targetRegion->setTargetRight (getPercentualValue (value),
                                            true);
            }
        }
      else
        {
          if (param == "left")
            {
              targetRegion->setTargetLeft (
                  (double)(::ginga::util::stof (value)), false);
            }
          else if (param == "top")
            {
              targetRegion->setTargetTop (
                  (double)(::ginga::util::stof (value)), false);
            }
          else if (param == "width")
            {
              targetRegion->setTargetWidth (
                  (double)(::ginga::util::stof (value)), false);
            }
          else if (param == "height")
            {
              targetRegion->setTargetHeight (
                  (double)(::ginga::util::stof (value)), false);
            }
          else if (param == "bottom")
            {
              targetRegion->setTargetBottom (
                  (double)(::ginga::util::stof (value)), false);
            }
          else if (param == "right")
            {
              targetRegion->setTargetRight (
                  (double)(::ginga::util::stof (value)), false);
            }
        }
    }

  return true;
}

double
AnimationController::getSinglePropertyTarget (int i)
{
  string propName = propertySingleNames[i];
  double target = 0;

  if (propName == "left")
    {
      target = targetRegion->getAbsoluteLeft ();
    }
  else if (propName == "top")
    {
      target = targetRegion->getAbsoluteTop ();
    }
  else if (propName == "width")
    {
      target = targetRegion->getWidthInPixels ();
    }
  else if (propName == "height")
    {
      target = targetRegion->getHeightInPixels ();
    }
  else if (propName == "right")
    {
      target = targetRegion->getRightInPixels ();
    }
  else if (propName == "bottom")
    {
      target = targetRegion->getBottomInPixels ();
    }
  else
    {
      target = (double)(::ginga::util::stof (strTargetValues[i]));
    }

  return target;
}

GINGA_FORMATTER_END
