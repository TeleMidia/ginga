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
#include "AdapterFormatterPlayer.h"

#include "AdapterApplicationPlayer.h"
#include "AdapterPlayerManager.h"
#include "NclLinkTransitionTriggerCondition.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

double AdapterFormatterPlayer::eventTS = 0;

AdapterFormatterPlayer::AdapterFormatterPlayer ()
{
  typeSet.insert ("AdapterFormatterPlayer");

  this->manager = NULL;
  this->object = NULL;
  this->player = NULL;
  this->mirrorSrc = NULL;
  this->anchorMonitor = NULL;
  this->mrl = "";
  this->playerCompName = "";
  this->objectDevice = -1;
  this->outTransDur = 0;
  this->outTransTime = -1.0;
  this->isLocked = false;
  Thread::mutexInit (&objectMutex, false);
}

AdapterFormatterPlayer::~AdapterFormatterPlayer ()
{
  int objDevice;

  if (im != NULL)
    {
      im->removeInputEventListener (this);
      im = NULL;
    }

  lockObject ();
  if (anchorMonitor != NULL)
    {
      anchorMonitor->stopMonitor ();
      delete anchorMonitor;
      anchorMonitor = NULL;
    }

  objDevice = getObjectDevice ();

  if (object != NULL)
    {
      object = NULL;
    }

  if (player != NULL)
    {
      player->removeListener (this);
      player->stop ();
      if (objDevice == 0)
        {
          delete player;
        }
      player = NULL;
    }

  unlockObject ();

  Thread::mutexDestroy (&objectMutex);
}

void
AdapterFormatterPlayer::setAdapterManager (void *manager)
{
  this->manager = manager;
  im = Ginga_Display->getInputManager ();
}

bool
AdapterFormatterPlayer::instanceOf (const string &s)
{
  if (!typeSet.empty ())
    {
      return (typeSet.find (s) != typeSet.end ());
    }
  else
    {
      return false;
    }
}

void
AdapterFormatterPlayer::setOutputWindow (SDLWindow* windowId)
{
  if (player != NULL)
    {
      player->setOutWindow (windowId);
    }
  else
    {
      clog << "AdapterFormatterPlayer::setOutputWindow Warning!";
      clog << " Player is NULL" << endl;
    }
}

void
AdapterFormatterPlayer::rebase ()
{
  player->reset ();
  prepareScope (player->getMediaTime ());
  player->rebase ();
}

void
AdapterFormatterPlayer::createPlayer ()
{
  vector<Anchor *> *anchors;
  vector<Anchor *>::iterator i;

  vector<NclFormatterEvent *> *events;
  vector<NclFormatterEvent *>::iterator j;

  NclCascadingDescriptor *descriptor;
  vector<Parameter *> *descParams;
  vector<Parameter *>::iterator it;

  NodeEntity *dataObject;
  PropertyAnchor *property;

  if (player == NULL)
    {
      player = new Player (mrl);
    }

  player->addListener (this);

  if (object == NULL)
    {
      return;
    }

  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      descParams = descriptor->getParameters ();
      if (descParams != NULL)
        {
          Parameter *param;

          it = descParams->begin ();
          while (it != descParams->end ())
            {
              param = (*it);

              player->setPropertyValue (param->getName (),
                                        param->getValue ());

              ++it;
            }

          delete descParams;
        }
    }

  dataObject = (NodeEntity *)(object->getDataObject ());
  if (dataObject->instanceOf ("ContentNode"))
    {
      anchors = ((ContentNode *)dataObject)->getAnchors ();
      if (anchors != NULL)
        {
          i = anchors->begin ();
          while (i != anchors->end ())
            {
              if ((*i)->instanceOf ("PropertyAnchor"))
                {
                  property = ((PropertyAnchor *)(*i));

                  clog << "AdapterFormatterPlayer::createPlayer for '";
                  clog << mrl;
                  clog << "' set property '";
                  clog << property->getPropertyName ();
                  clog << "' value '" << property->getPropertyValue ();
                  clog << "'" << endl;

                  player->setPropertyValue (property->getPropertyName (),
                                            property->getPropertyValue ());
                }
              ++i;
            }
        }
    }

  events = object->getEvents ();
  if (events != NULL)
    {
      j = events->begin ();
      while (j != events->end ())
        {
          if (*j != NULL && (*j)->instanceOf ("NclAttributionEvent"))
            {
              property = ((NclAttributionEvent *)*j)->getAnchor ();
              ((NclAttributionEvent *)(*j))->setValueMaintainer (this);
            }
          ++j;
        }
      delete events;
      events = NULL;
    }

  objectDevice = getObjectDevice ();

  clog << "AdapterFormatterPlayer::createPlayer for '" << mrl;
  clog << "' object = '" << object->getId () << "'";
  clog << " objectDevice = '" << objectDevice << "'" << endl;
}

int
AdapterFormatterPlayer::getObjectDevice ()
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *ncmRegion = NULL;

  if (objectDevice > -1)
    {
      return objectDevice;
    }

  if (object != NULL)
    {
      descriptor = object->getDescriptor ();
      if (descriptor != NULL)
        {
          ncmRegion = descriptor->getRegion ();
          if (ncmRegion != NULL)
            {
              objectDevice = ncmRegion->getDeviceClass ();
              return objectDevice;
            }
        }
    }

  return 0;
}

bool
AdapterFormatterPlayer::hasPrepared ()
{
  bool presented;
  NclFormatterEvent *mEv;
  short st;

  if (object == NULL || player == NULL)
    {
      clog << "AdapterFormatterPlayer::hasPrepared return false because";
      clog << " object = '" << object << "' and player = '";
      clog << player << "'" << endl;
      return false;
    }

  presented = player->hasPresented ();
  if (presented)
    {
      clog << "AdapterFormatterPlayer::hasPrepared return false because";
      clog << " presented is true" << endl;
      return false;
    }

  presented = player->isForcedNaturalEnd ();
  if (presented)
    {
      clog << "AdapterFormatterPlayer::hasPrepared return false because";
      clog << " a natural end was forced" << endl;
      return false;
    }

  mEv = object->getMainEvent ();
  if (mEv != NULL && !object->instanceOf ("NclApplicationExecutionObject"))
    {
      st = mEv->getCurrentState ();
      if (st != EventUtil::ST_SLEEPING)
        {
          return true;
        }
    }

  return isLocked;
}

bool
AdapterFormatterPlayer::setKeyHandler (bool isHandler)
{
  if (player == NULL)
    {
      g_usleep (100000);
    }

  if (player == NULL)
    {
      return false;
    }

  return (player->setKeyHandler (isHandler) && isHandler);
}

double
AdapterFormatterPlayer::prepareProperties (NclExecutionObject *obj)
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region = NULL;
  PropertyAnchor *property;
  vector<string> *params;
  vector<Parameter *> *descParams;
  vector<Parameter *>::iterator i;
  vector<PropertyAnchor *> *anchors;
  vector<PropertyAnchor *>::iterator j;
  string name, value;
  NclFormatterRegion *fRegion = NULL;
  Node *ncmNode;
  double transpValue = -1;
  double parentOpacity = -1;
  bool isPercentual;
  double explicitDur = -1;

  string left = "", top = "", width = "", height = "";
  string bottom = "", right = "";
  string plan = "";

  descriptor = obj->getDescriptor ();
  if (descriptor != NULL)
    {
      fRegion = descriptor->getFormatterRegion ();
      if (fRegion != NULL)
        {
          region = fRegion->getLayoutRegion ();
        }
      else
        {
          region = NULL;
        }
    }

  if (region == NULL)
    {
      property = obj->getNCMProperty ("explicitDur");
      if (property != NULL)
        {
          value = property->getPropertyValue ();
          explicitDur = ::ginga::util::strUTCToSec (value) * 1000;
        }

      return explicitDur;
    }

  descParams = descriptor->getParameters ();
  if (descParams != NULL)
    {
      Parameter *param;

      i = descParams->begin ();
      while (i != descParams->end ())
        {
          param = (*i);
          name = param->getName ();
          value = param->getValue ();

          clog << "AdapterFormatterPlayer::prepareProperties(";
          clog << mrl << ") param '" << name << "' with value '";
          clog << value << "'";
          clog << endl;

          if (value != "")
            {
              if (name == "explicitDur")
                {
                  explicitDur = ::ginga::util::strUTCToSec (value) * 1000;
                }
              else if (name == "left")
                {
                  left = value;
                }
              else if (name == "top")
                {
                  top = value;
                }
              else if (name == "width")
                {
                  width = value;
                }
              else if (name == "height")
                {
                  height = value;
                }
              else if (name == "bottom")
                {
                  bottom = value;
                }
              else if (name == "right")
                {
                  right = value;
                }
              else if (name == "zIndex")
                {
                  region->setZIndex (xstrto_int (value));
                }
              else if (name == "bounds")
                {
                  params = split (xstrchomp (value), ",");
                  if (params->size () == 4)
                    {
                      left = (*params)[0];
                      top = (*params)[1];
                      width = (*params)[2];
                      height = (*params)[3];
                    }

                  delete params;
                }
              else if (name == "location")
                {
                  params = split (xstrchomp (value), ",");
                  if (params->size () == 2)
                    {
                      left = (*params)[0];
                      top = (*params)[1];
                    }

                  delete params;
                }
              else if (name == "size")
                {
                  params = split (xstrchomp (value), ",");
                  if (params->size () == 2)
                    {
                      width = (*params)[0];
                      height = (*params)[1];
                    }
                  delete params;
                }
              else if (name == "transparency")
                {
                  value = cvtPercentual (value, &isPercentual);
                  transpValue = xstrtod (value);
                  if (isPercentual)
                    {
                      transpValue = transpValue / 100;
                    }

                  parentOpacity = (1
                                   - ((AdapterPlayerManager *)manager)
                                         ->getNclPlayerData ()
                                         ->transparency);

                  transpValue = (1 - (parentOpacity
                                      - (parentOpacity * transpValue)));

                  clog << "AdapterFormatterPlayer::prepareProperties";
                  clog << " parent opacity is '" << parentOpacity;
                  clog << "' original transparency property is '";
                  clog << value << "' new value is '";
                  clog << transpValue << "'" << endl;

                  if (fRegion != NULL)
                    {
                      fRegion->setTransparency (transpValue);
                    }
                }
              else if (name == "background")
                {
                  if (fRegion != NULL)
                    {
                      if (value.find (",") == std::string::npos)
                        {
                          fRegion->setBackgroundColor (value);
                        }
                      else
                        {
                          Color *bg = NULL;

                          params = split (xstrchomp (value), ",");
                          if (params->size () == 3)
                            {
                              bg = new Color (xstrto_uint8 ((*params)[0]),
                                              xstrto_uint8 ((*params)[1]),
                                              xstrto_uint8 ((*params)[2]));
                              fRegion->setBackgroundColor (bg);
                            }
                          else if (params->size () == 4)
                            {
                              bg = new Color (xstrto_uint8 ((*params)[0]),
                                              xstrto_uint8 ((*params)[1]),
                                              xstrto_uint8 ((*params)[2]),
                                              xstrto_uint8 ((*params)[3]));
                              fRegion->setBackgroundColor (bg);
                            }
                          delete params;
                        }
                    }
                }
              else if (name == "focusIndex")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setFocusIndex (value);
                    }
                }
              else if (name == "focusBorderColor")
                {
                  Color *color;
                  if (fRegion != NULL)
                    {
                      color = new Color (value);
                      fRegion->setFocusBorderColor (color);
                      delete color;
                    }
                }
              else if (name == "focusBorderWidth")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setFocusBorderWidth (
                          xstrto_int (value));
                    }
                }
              else if (name == "focusComponentSrc")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setFocusComponentSrc (value);
                    }
                }
              else if (name == "selBorderColor")
                {
                  Color *color;
                  if (fRegion != NULL)
                    {
                      color = new Color (value);
                      fRegion->setSelBorderColor (color);
                      delete color;
                    }
                }
              else if (name == "selBorderWidth")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setSelBorderWidth (
                          xstrto_int (value));
                    }
                }
              else if (name == "selComponentSrc")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setSelComponentSrc (value);
                    }
                }
              else if (name == "moveUp")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setMoveUp (value);
                    }
                }
              else if (name == "moveDown")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setMoveDown (value);
                    }
                }
              else if (name == "moveLeft")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setMoveLeft (value);
                    }
                }
              else if (name == "moveRight")
                {
                  if (fRegion != NULL)
                    {
                      fRegion->setMoveRight (value);
                    }
                }
              else if (name == "plan")
                {
                  plan = value;
                }
            }
          ++i;
        }
      delete descParams;
    }

  ncmNode = obj->getDataObject ();
  anchors = ((Node *)ncmNode)->getOriginalPropertyAnchors ();
  if (anchors != NULL)
    {
      j = anchors->begin ();
      while (j != anchors->end ())
        {
          if ((*j)->instanceOf ("PropertyAnchor"))
            {
              property = ((PropertyAnchor *)(*j));
              name = property->getPropertyName ();
              value = property->getPropertyValue ();

              clog << "AdapterFormatterPlayer::prepareProperties(";
              clog << mrl << ") property '" << name << "' with value '";
              clog << value << "'";
              clog << endl;

              if (value != "")
                {
                  if (name == "explicitDur")
                    {
                      explicitDur
                          = ::ginga::util::strUTCToSec (value) * 1000;
                    }
                  else if (name == "left")
                    {
                      left = value;
                    }
                  else if (name == "top")
                    {
                      top = value;
                    }
                  else if (name == "width")
                    {
                      width = value;
                    }
                  else if (name == "height")
                    {
                      height = value;
                    }
                  else if (name == "bottom")
                    {
                      bottom = value;
                    }
                  else if (name == "right")
                    {
                      right = value;
                    }
                  else if (name == "zIndex")
                    {
                      region->setZIndex (xstrto_int (value));
                    }
                  else if (name == "bounds")
                    {
                      params = split (xstrchomp (value), ",");
                      if (params->size () == 4)
                        {
                          left = (*params)[0];
                          top = (*params)[1];
                          width = (*params)[2];
                          height = (*params)[3];
                        }

                      delete params;
                    }
                  else if (name == "location")
                    {
                      params = split (xstrchomp (value), ",");
                      if (params->size () == 2)
                        {
                          left = (*params)[0];
                          top = (*params)[1];
                        }

                      delete params;
                    }
                  else if (name == "size")
                    {
                      params = split (xstrchomp (value), ",");
                      if (params->size () == 2)
                        {
                          width = (*params)[0];
                          height = (*params)[1];
                        }
                      delete params;
                    }
                  else if (name == "transparency")
                    {
                      value = cvtPercentual (value, &isPercentual);
                      transpValue = xstrtod (value);
                      if (isPercentual)
                        {
                          transpValue = transpValue / 100;
                        }

                      parentOpacity = (1
                                       - ((AdapterPlayerManager *)manager)
                                             ->getNclPlayerData ()
                                             ->transparency);

                      transpValue = (1 - (parentOpacity
                                          - (parentOpacity * transpValue)));

                      clog << "AdapterFormatterPlayer::prepareProperties";
                      clog << " parent opacity is '" << parentOpacity;
                      clog << "' original transparency property is '";
                      clog << value << "' new value is '";
                      clog << transpValue << "'" << endl;

                      if (fRegion != NULL)
                        {
                          fRegion->setTransparency (transpValue);
                        }
                    }
                  else if (name == "background")
                    {
                      if (fRegion != NULL)
                        {
                          if (value.find (",") == std::string::npos)
                            {
                              fRegion->setBackgroundColor (value);
                            }
                          else
                            {
                              Color *bg = NULL;

                              params = split (xstrchomp (value), ",");
                              if (params->size () == 3)
                                {
                                  bg = new Color (xstrto_uint8 ((*params)[0]),
                                                  xstrto_uint8 ((*params)[1]),
                                                  xstrto_uint8 ((*params)[2]));
                                  fRegion->setBackgroundColor (bg);
                                }
                              else if (params->size () == 4)
                                {
                                  bg = new Color (xstrto_uint8 ((*params)[0]),
                                                  xstrto_uint8 ((*params)[1]),
                                                  xstrto_uint8 ((*params)[2]),
                                                  xstrto_uint8 ((*params)[3]));
                                  fRegion->setBackgroundColor (bg);
                                }
                              delete params;
                            }
                        }
                    }
                  else if (name == "focusIndex")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setFocusIndex (value);
                        }
                    }
                  else if (name == "focusBorderColor")
                    {
                      Color *color;
                      if (fRegion != NULL)
                        {
                          color = new Color (value);
                          fRegion->setFocusBorderColor (color);
                          delete color;
                        }
                    }
                  else if (name == "focusBorderWidth")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setFocusBorderWidth (
                              xstrto_int (value));
                        }
                    }
                  else if (name == "focusComponentSrc")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setFocusComponentSrc (value);
                        }
                    }
                  else if (name == "selBorderColor")
                    {
                      Color *color;
                      if (fRegion != NULL)
                        {
                          color = new Color (value);
                          fRegion->setSelBorderColor (color);
                          delete color;
                        }
                    }
                  else if (name == "selBorderWidth")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setSelBorderWidth (
                              xstrto_int (value));
                        }
                    }
                  else if (name == "selComponentSrc")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setSelComponentSrc (value);
                        }
                    }
                  else if (name == "moveUp")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setMoveUp (value);
                        }
                    }
                  else if (name == "moveDown")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setMoveDown (value);
                        }
                    }
                  else if (name == "moveLeft")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setMoveLeft (value);
                        }
                    }
                  else if (name == "moveRight")
                    {
                      if (fRegion != NULL)
                        {
                          fRegion->setMoveRight (value);
                        }
                    }
                  else if (name == "plan")
                    {
                      plan = value;
                    }
                }
            }
          ++j;
        }
    }

  if (left != "")
    {
      region->resetLeft ();
    }

  if (top != "")
    {
      region->resetTop ();
    }

  if (width != "")
    {
      region->resetWidth ();
    }

  if (height != "")
    {
      region->resetHeight ();
    }

  if (bottom != "")
    {
      if (top != "")
        {
          region->resetHeight ();
        }
      region->resetBottom ();
    }

  if (right != "")
    {
      if (left != "")
        {
          region->resetWidth ();
        }
      region->resetRight ();
    }

  if (left != "")
    {
      value = cvtPercentual (left, &isPercentual);
      region->setLeft (xstrtod (value), isPercentual);
    }

  if (top != "")
    {
      value = cvtPercentual (top, &isPercentual);
      region->setTop (xstrtod (value), isPercentual);
    }

  if (width != "")
    {
      value = cvtPercentual (width, &isPercentual);
      region->setWidth (xstrtod (value), isPercentual);
    }

  if (height != "")
    {
      value = cvtPercentual (height, &isPercentual);
      region->setHeight (xstrtod (value), isPercentual);
    }

  if (bottom != "")
    {
      value = cvtPercentual (bottom, &isPercentual);
      region->setBottom (xstrtod (value), isPercentual);
    }

  if (right != "")
    {
      value = cvtPercentual (right, &isPercentual);
      region->setRight (xstrtod (value), isPercentual);
    }

  if (plan == "" && mrl.find ("sbtvd-ts://") != std::string::npos)
    {
      plan = "video";
    }

  if (plan == "")
    {
      plan = "graphic";
    }

  if (fRegion != NULL)
    {
      fRegion->setPlan (plan);
    }

  if (transpValue < 0.
      && descriptor->getParameterValue ("transparency") == "")
    {
      transpValue = ((AdapterPlayerManager *)manager)
                        ->getNclPlayerData ()
                        ->transparency;

      if (fRegion != NULL)
        {
          fRegion->setTransparency (transpValue);
        }
    }

  return explicitDur;
}

void
AdapterFormatterPlayer::updatePlayerProperties (arg_unused (NclExecutionObject *obj))
{
  NclCascadingDescriptor *descriptor;
  string value;

  if (object != NULL)
    {
      descriptor = object->getDescriptor ();
      if (descriptor != NULL)
        {
          value = descriptor->getParameterValue ("soundLevel");
          if (value == "")
            {
              value = "1.0";
            }

          if (player != NULL)
            {
              player->setPropertyValue ("soundLevel", value);
            }
        }
    }
}

bool
AdapterFormatterPlayer::prepare (NclExecutionObject *object,
                                 NclFormatterEvent *event)
{
  Content *content;
  NclCascadingDescriptor *descriptor;
  double explicitDur = -1;
  NodeEntity *dataObject;

  if (hasPrepared ())
    {
      clog << "AdapterFormatterPlayer::prepare returns false, ";
      clog << "because player is already prepared" << endl;
      return false;
    }

  if (!lockObject ())
    {
      if (player != NULL)
        {
          if (player->hasPresented () || player->isForcedNaturalEnd ())
            {
              while (!lockObject ())
                ;
            }
          else
            {
              clog << "AdapterFormatterPlayer::prepare returns false, ";
              clog << "because object is locked" << endl;
              return false;
            }
        }
    }

  if (object == NULL)
    {
      clog << "AdapterFormatterPlayer::prepare Warning! ";
      clog << "Trying to prepare a NULL object." << endl;
      unlockObject ();
      return false;
    }

  this->object = object;
  descriptor = object->getDescriptor ();
  dataObject = (NodeEntity *)(object->getDataObject ());

  if (dataObject != NULL && dataObject->getDataEntity () != NULL)
    {
      content
          = ((NodeEntity *)(dataObject->getDataEntity ()))->getContent ();

      if (content != NULL && content->instanceOf ("ReferenceContent"))
        {
          this->mrl
              = ((ReferenceContent *)content)->getCompleteReferenceUrl ();

          this->mrl = SystemCompat::updatePath (this->mrl);
        }
      else
        {
          this->mrl = "";
        }
    }

  explicitDur = prepareProperties (object);

  if (anchorMonitor != NULL)
    {
      anchorMonitor->stopMonitor ();
      delete anchorMonitor;
      anchorMonitor = NULL;
    }

  if (event->instanceOf ("NclPresentationEvent"))
    {
      double duration = ((NclPresentationEvent *)event)->getDuration ();
      bool infDur = (isNaN (duration) || isInfinity (duration));

      if (descriptor != NULL && explicitDur < 0)
        {
          explicitDur = descriptor->getExplicitDuration ();
        }

      if (!infDur && duration <= 0 && explicitDur <= 0)
        {
          clog << "AdapterFormatterPlayer::prepare '";
          clog << object->getId () << "' Warning! Can't prepare an ";
          clog << "object with an event duration <= 0" << endl;
          return false;
        }

      // explicit duration overwrites implicit duration
      if (!isNaN (explicitDur) && explicitDur > 0)
        {
          object->removeEvent (event);

          // The explicit duration is a property of
          // the object. For instance: start an interface with
          // begin = 4s and explicit duration = 5s => new duration
          // will be 1s
          ((NclPresentationEvent *)event)->setEnd (explicitDur);
          object->addEvent (event);

          clog << "AdapterFormatterPlayer::prepare '";
          clog << object->getId () << "' ";
          clog << "with explicitDur = '";
          clog << explicitDur << "' object duration was '";
          clog << duration << "'. Updated info: event begin = '";
          clog << ((NclPresentationEvent *)event)->getBegin () << "'";
          clog << " event end = '";
          clog << ((NclPresentationEvent *)event)->getEnd () << "'";
          clog << endl;
        }
    }

  createPlayer ();
  updatePlayerProperties (object);
  if (event->getCurrentState () == EventUtil::ST_SLEEPING)
    {
      object->prepare ((NclPresentationEvent *)event, 0);
      prepare ();
      return true;
    }
  else
    {
      return false;
    }
}

void
AdapterFormatterPlayer::prepare ()
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *region;

  if (object == NULL)
    {
      return;
    }

  if (player == NULL)
    {
      return;
    }

  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getRegion ();
      if (region != NULL)
        {
          player->setNotifyContentUpdate (region->getDeviceClass () == 1);
        }
    }

  prepareScope ();

  if (player->immediatelyStart ())
    {
      player->setImmediatelyStart (false);
      start ();
    }
}

void
AdapterFormatterPlayer::prepareScope (double offset)
{
  NclPresentationEvent *mainEvent;
  double duration;
  double playerDur;
  double initTime = 0;
  IntervalAnchor *intervalAnchor;

  mainEvent = (NclPresentationEvent *)(object->getMainEvent ());
  if (mainEvent->instanceOf ("NclPresentationEvent"))
    {
      if ((mainEvent->getAnchor ())->instanceOf ("LambdaAnchor"))
        {
          duration = mainEvent->getDuration ();

          if (offset > 0)
            {
              initTime = offset;
            }

          outTransDur = getOutTransDur ();
          if (outTransDur > 0.0)
            {
              playerDur = player->getTotalMediaTime ();
              if (isInfinity (duration) && playerDur > 0.0)
                {
                  duration = playerDur * 1000;
                }

              outTransTime = duration - outTransDur;
              if (outTransTime <= 0.0)
                {
                  outTransTime = 0.1;
                }
            }

          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime,
                                duration / 1000, outTransTime);
            }
          else
            {
              outTransDur = -1.0;
              player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime);
            }
        }
      else if (((mainEvent->getAnchor ()))->instanceOf ("IntervalAnchor"))
        {
          intervalAnchor = (IntervalAnchor *)(mainEvent->getAnchor ());
          initTime = (intervalAnchor->getBegin () / 1000);
          if (offset > 0)
            {
              initTime = offset;
            }

          duration = intervalAnchor->getEnd ();

          outTransDur = getOutTransDur ();
          if (outTransDur > 0.0)
            {
              playerDur = player->getTotalMediaTime ();
              if (isInfinity (duration) && playerDur > 0.0)
                {
                  duration = playerDur * 1000;
                }

              outTransTime = duration - outTransDur;
              if (outTransTime <= 0.0)
                {
                  outTransTime = 0.1;
                }
            }

          if (duration < IntervalAnchor::OBJECT_DURATION)
            {
              player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime,
                                (intervalAnchor->getEnd () / 1000),
                                outTransTime);
            }
          else
            {
              outTransDur = -1.0;
              player->setScope (mainEvent->getAnchor ()->getId (),
                                IPlayer::TYPE_PRESENTATION, initTime);
            }
        }
    }

  if (offset > 0)
    {
      player->setMediaTime (offset);
    }
}

double
AdapterFormatterPlayer::getOutTransDur ()
{
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *fRegion;
  double outTransDur = 0.0;

  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      fRegion = descriptor->getFormatterRegion ();
      if (fRegion != NULL)
        {
          outTransDur = fRegion->getOutTransDur ();
        }
    }

  return outTransDur;
}

double
AdapterFormatterPlayer::getOutTransTime ()
{
  return outTransTime;
}

void
AdapterFormatterPlayer::checkAnchorMonitor ()
{
  ITimeBaseProvider *timeBaseProvider = NULL;
  NclEventTransition *nextTransition;
  NodeEntity *dataObject;

  if (anchorMonitor != NULL)
    {
      anchorMonitor->stopMonitor ();
      delete anchorMonitor;
      anchorMonitor = NULL;
    }

  if (object != NULL)
    {
      nextTransition = object->getNextTransition ();
    }
  else
    {
      return;
    }

  if (nextTransition != NULL && !isInfinity (nextTransition->getTime ()))
    {
      clog << "AdapterFormatterPlayer::checkAnchorMonitor ";
      clog << "creating TIME NominalEventMonitor for '";
      clog << object->getId () << "'";
      clog << endl;
      anchorMonitor = new NominalEventMonitor (object, this);
    }
  else
    {
      clog << "AdapterFormatterPlayer::checkAnchorMonitor ";
      clog << "can't create TIME NominalEventMonitor for '";
      clog << object->getId () << "' ";
      if (nextTransition != NULL)
        {
          clog << "because next transition = '";
          clog << nextTransition->getTime () << "' ";
        }
      else
        {
          clog << "because next transition is null ";
        }
      clog << endl;
    }

  timeBaseProvider
      = ((AdapterPlayerManager *)manager)->getTimeBaseProvider ();

  if (object->hasSampleEvents () && timeBaseProvider != NULL)
    {
      if (anchorMonitor == NULL)
        {
          anchorMonitor = new NominalEventMonitor (object, this);
        }

      anchorMonitor->setTimeBaseProvider (timeBaseProvider);
    }

  dataObject = (NodeEntity *)(object->getDataObject ()->getDataEntity ());
  if (dataObject->instanceOf ("ContentNode"))
    {
      if (((ContentNode *)dataObject)->isTimeNode ()
          && anchorMonitor == NULL)
        {
          anchorMonitor = new NominalEventMonitor (object, this);
        }
    }

  if (im == NULL)
    { // player was recovered from manager gc
      im = Ginga_Display->getInputManager ();
    }

  if (im != NULL)
    {
      im->addInputEventListener (this, object->getInputEvents ());
    }

  if (anchorMonitor != NULL)
    {
      anchorMonitor->startMonitor ();
    }
}

void
AdapterFormatterPlayer::printAction (const string &act,
                                     NclExecutionObject *object)
{
  string action = act;

  NclFormatterEvent *event;

  if (object->instanceOf ("NclApplicationExecutionObject"))
    {
      event = ((NclApplicationExecutionObject *)object)->getCurrentEvent ();
    }
  else
    {
      event = object->getMainEvent ();
    }

  if (event == NULL)
    {
      return;
    }

  action = action + "::" + object->getDataObject ()->getId ();

  printAction (action);
}

void
AdapterFormatterPlayer::printAction (const string &command)
{
  // notifying GingaGUI
  // cmd means a command to an embeddor process
  // 0 means successful status message
  //
  cout << "cmd::0::" << command << endl;
}

bool
AdapterFormatterPlayer::start ()
{
  NclCascadingDescriptor *descriptor;
  LayoutRegion *ncmRegion = NULL;
  string paramValue;
  NclFormatterEvent *objEv;

  assert (object != NULL);

  if (!object->isSleeping ())
    {
      clog << "AdapterFormatterPlayer::start(" << object->getId ();
      clog << ") is occurring or paused" << endl;
      return false;
    }

  /*clog << "AdapterFormatterPlayer::start(" << object->getId();
  clog << ")" << endl;*/
  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      paramValue = descriptor->getParameterValue ("visible");
      if (paramValue == "false")
        {
          setVisible (false);
        }
      else if (paramValue == "true")
        {
          setVisible (true);
        }

      ncmRegion = descriptor->getRegion ();
      if (ncmRegion != NULL && ncmRegion->getDeviceClass () == 2)
        {
          objEv = object->getMainEvent ();
          if (objEv != NULL)
            {
              clog << "AdapterFormatterPlayer::start(";
              clog << object->getId ();
              clog << ") ACTIVE CLASS" << endl;
              objEv->start ();
            }
          return true;
        }
    }

  if (object != NULL)
    {
      bool startSuccess = false;

      if (player != NULL)
        {
          if (mirrorSrc != NULL)
            {
              player->setMirrorSrc (mirrorSrc);
            }
          startSuccess = player->play ();
        }

      if (startSuccess)
        {
          if (!object->start ())
            {
              if (player != NULL)
                {
                  player->stop ();
                }
              startSuccess = false;
            }
          else
            {
              checkAnchorMonitor ();
            }
        }

      return startSuccess;
    }
  return false;
}

bool
AdapterFormatterPlayer::stop ()
{
  NclFormatterEvent *mainEvent = NULL;
  vector<NclFormatterEvent *> *events = NULL;

  if (anchorMonitor != NULL)
    {
      anchorMonitor->stopMonitor ();
    }

  if (player == NULL && object == NULL)
    {
      if (im != NULL)
        {
          im->removeInputEventListener (this);
          im = NULL;
        }
      unlockObject ();
      return false;
    }
  else if (object != NULL)
    {
      mainEvent = object->getMainEvent ();
      events = object->getEvents ();
    }

  if (mainEvent != NULL && mainEvent->instanceOf ("NclPresentationEvent"))
    {
      if (checkRepeat ((NclPresentationEvent *)mainEvent))
        {
          return true;
        }
    }

  /*clog << "AdapterFormatterPlayer::stop(" << object->getId();
  clog << ")" << endl;*/

  if (im != NULL)
    {
      im->removeInputEventListener (this);
      im = NULL;
    }

  if (events != NULL)
    {
      vector<NclFormatterEvent *>::iterator i;
      i = events->begin ();
      while (i != events->end ())
        {
          if (*i != NULL && (*i)->instanceOf ("NclAttributionEvent"))
            {
              ((NclAttributionEvent *)(*i))->setValueMaintainer (NULL);
            }

          if (i != events->end ())
            {
              ++i;
            }
        }
      delete events;
      events = NULL;
    }

  if (player != NULL)
    {
      player->stop ();
      player->notifyReferPlayers (EventUtil::TR_STOPS);
    }

  if (player != NULL && player->isForcedNaturalEnd ())
    {
      player->forceNaturalEnd (false);
      return unprepare ();
    }
  else if (object != NULL)
    {
      object->stop ();
      unprepare ();
      return true;
    }

  return false;
}

bool
AdapterFormatterPlayer::pause ()
{
  if (object != NULL && player != NULL && object->pause ())
    {
      player->pause ();
      if (anchorMonitor != NULL)
        {
          anchorMonitor->pauseMonitor ();
        }

      player->notifyReferPlayers (EventUtil::TR_PAUSES);
      return true;
    }
  else
    {
      return false;
    }
}

bool
AdapterFormatterPlayer::resume ()
{
  if (object != NULL && player != NULL && object->resume ())
    {
      player->resume ();
      if (anchorMonitor != NULL)
        {
          anchorMonitor->resumeMonitor ();
        }

      player->notifyReferPlayers (EventUtil::TR_RESUMES);
      return true;
    }
  return false;
}

bool
AdapterFormatterPlayer::abort ()
{
  if (im != NULL)
    {
      im->removeInputEventListener (this);
      im = NULL;
    }

  if (anchorMonitor != NULL)
    {
      anchorMonitor->stopMonitor ();
    }

  if (player != NULL)
    {
      player->stop ();
      player->notifyReferPlayers (EventUtil::TR_ABORTS);
    }

  if (object != NULL)
    {
      if (!object->isSleeping ())
        {
          object->abort ();
          unprepare ();
          return true;
        }
    }

  unlockObject ();
  return false;
}

void
AdapterFormatterPlayer::naturalEnd ()
{
  bool freeze;

  if (object == NULL || player == NULL)
    {
      unlockObject ();
      return;
    }

  // if freeze is true the natural end is not performed
  if (object->getDescriptor () != NULL)
    {
      freeze = object->getDescriptor ()->getFreeze ();
      if (freeze)
        {
          return;
        }
    }

  printAction ("stop", object);
  stop ();
}

bool
AdapterFormatterPlayer::checkRepeat (NclPresentationEvent *event)
{
  if (event->getRepetitions () > 1)
    {
      // clog << endl << "AdapterFormatterPlayer::checkRepeat";
      if (anchorMonitor != NULL)
        {
          anchorMonitor->stopMonitor ();
          delete anchorMonitor;
          anchorMonitor = NULL;
        }

      player->stop ();
      player->notifyReferPlayers (EventUtil::TR_STOPS);

      if (object != NULL)
        {
          // clog << "'" << object->getId() << "'";
          object->stop ();
        }
      // clog << endl << endl;

      player->setImmediatelyStart (true);
      prepare ();
      return true;
    }

  return false;
}

bool
AdapterFormatterPlayer::unprepare ()
{
  if (object != NULL && object->getMainEvent () != NULL
      && (object->getMainEvent ()->getCurrentState ()
              == EventUtil::ST_OCCURRING
          || object->getMainEvent ()->getCurrentState ()
                 == EventUtil::ST_PAUSED))
    {
      return stop ();
    }

  ((AdapterPlayerManager *)manager)->removePlayer (object);

  if (NclExecutionObject::hasInstance (object, false))
    {
      object->unprepare ();
    }

  object = NULL;
  unlockObject ();

  return true;
}

bool
AdapterFormatterPlayer::setPropertyValue (NclAttributionEvent *event,
                                          const string &v)
{
  string propName;
  string value = v;

  if (player == NULL || object == NULL)
    {
      clog << "AdapterFormatterPlayer::setPropertyValue Warning!";
      clog << " cant set property '" << event->getId ();
      clog << "' value = '" << value << "' object = '";
      if (object != NULL)
        {
          clog << object->getId ();
        }
      else
        {
          clog << object;
        }
      clog << "' player = '";
      clog << player << "' for '" << mrl << "'" << endl;

      return false;
    }

  propName = (event->getAnchor ())->getPropertyName ();
  if (propName == "visible")
    {
      if (value == "false")
        {
          setVisible (false);
        }
      else if (value == "true")
        {
          setVisible (true);
        }
    }
  else
    {
      if (this->instanceOf ("AdapterApplicationPlayer"))
        {
          if (!((AdapterApplicationPlayer *)this)
                   ->setAndLockCurrentEvent (event))
            {
              return false;
            }
        }

      if (object->setPropertyValue (event, value))
        {
          player->setPropertyValue (propName,
                                    object->getPropertyValue (propName));
        }
      else
        {
          if (propName == "transparency")
            {
              bool isPercentual;
              double transpValue, parentOpacity;
              NclFormatterRegion *fRegion;
              NclCascadingDescriptor *descriptor;

              value = cvtPercentual (value, &isPercentual);
              transpValue = xstrtod (value);
              if (isPercentual)
                {
                  transpValue = transpValue / 100;
                }

              parentOpacity = (1
                               - ((AdapterPlayerManager *)manager)
                                     ->getNclPlayerData ()
                                     ->transparency);

              transpValue
                  = (1 - (parentOpacity - (parentOpacity * transpValue)));

              clog << "AdapterFormatterPlayer::setPropertyValue ";
              clog << "parent opacity is '" << parentOpacity;
              clog << "' original transparency property is '";
              clog << value << "' new value is '";
              clog << transpValue << "'" << endl;

              descriptor = object->getDescriptor ();
              if (descriptor != NULL)
                {
                  fRegion = descriptor->getFormatterRegion ();
                  if (fRegion != NULL)
                    {
                      fRegion->setTransparency (transpValue);
                    }
                }
            }
          player->setPropertyValue (propName, value);
        }

      if (this->instanceOf ("AdapterApplicationPlayer"))
        {
          ((AdapterApplicationPlayer *)this)->unlockCurrentEvent (event);
        }
    }

  return true;
}

void
AdapterFormatterPlayer::setPropertyValue (const string &name, const string &value)
{
  if (player != NULL)
    {
      clog << "AdapterFormatterPlayer::setPropertyValue name = '";
      clog << name << "' value = '" << value << "' to player ";
      clog << " address '" << player << "'";
      clog << endl;

      player->setPropertyValue (name, value);
    }
}

string
AdapterFormatterPlayer::getPropertyValue (void *event)
{
  string value = "";
  string name;

  if (event == NULL)
    {
      return "";
    }

  name = ((NclAttributionEvent *)event)->getAnchor ()->getPropertyName ();
  value = getPropertyValue (name);

  return value;
}

string
AdapterFormatterPlayer::getPropertyValue (const string &name)
{
  string value = "";

  if (player != NULL)
    {
      value = player->getPropertyValue (name);
    }

  if (value == "" && object != NULL)
    {
      value = object->getPropertyValue (name);
    }

  clog << "AdapterFormatterPlayer::getPropertyValue name = '";
  clog << name << "' value = '" << value << "'";
  clog << endl;

  return value;
}

double
AdapterFormatterPlayer::getObjectExpectedDuration ()
{
  return IntervalAnchor::OBJECT_DURATION;
}

void
AdapterFormatterPlayer::updateObjectExpectedDuration ()
{
  NclPresentationEvent *wholeContentEvent;
  double duration;
  double implicitDur;

  wholeContentEvent = object->getWholeContentPresentationEvent ();
  duration = wholeContentEvent->getDuration ();
  if ((object->getDescriptor () == NULL)
      || (isNaN ((object->getDescriptor ())->getExplicitDuration ()))
      || (duration < 0) || (isNaN (duration)))
    {
      implicitDur = getObjectExpectedDuration ();
      ((IntervalAnchor *)wholeContentEvent->getAnchor ())
          ->setEnd (implicitDur);

      wholeContentEvent->setDuration (implicitDur);
    }
}

double
AdapterFormatterPlayer::getMediaTime ()
{
  if (player != NULL)
    {
      return player->getMediaTime ();
    }
  return 0;
}

IPlayer *
AdapterFormatterPlayer::getPlayer ()
{
  return player;
}

void
AdapterFormatterPlayer::setTimeBasePlayer (
    AdapterFormatterPlayer *timeBasePlayerAdapter)
{
  IPlayer *timePlayer;
  timePlayer = timeBasePlayerAdapter->getPlayer ();
  if (timePlayer != NULL)
    {
      player->setReferenceTimePlayer (timePlayer);
      player->setTimeBasePlayer (timePlayer);
    }
}

void
AdapterFormatterPlayer::updateStatus (short code,
                                      const string &parameter,
                                      short type,
                                      arg_unused (const string &value))
{
  NclFormatterEvent *mainEvent;

  switch (code)
    {
    case IPlayer::PL_NOTIFY_OUTTRANS:
      if (outTransDur > 0.0)
        {
          NclCascadingDescriptor *descriptor;
          NclFormatterRegion *fRegion;

          outTransDur = -1.0;
          outTransTime = -1.0;
          descriptor = object->getDescriptor ();
          if (descriptor != NULL)
            {
              fRegion = descriptor->getFormatterRegion ();
              if (fRegion != NULL)
                {
                  clog << "AdapterFormatterPlayer::";
                  clog << "updateStatus transition" << endl;
                  fRegion->performOutTrans ();
                }
            }
        }
      break;

    case IPlayer::PL_NOTIFY_STOP:
      if (object != NULL)
        {
          if (type == IPlayer::TYPE_PRESENTATION)
            {
              if (parameter == "")
                {
                  naturalEnd ();
                }
            }
          else if (type == IPlayer::TYPE_SIGNAL)
            {
              mainEvent = object->getMainEvent ();
              if (mainEvent != NULL
                  && mainEvent->getCurrentState ()
                         != EventUtil::ST_SLEEPING)
                {
                  rebase ();
                  clog << "AdapterFormatterPlayer::updateStatus ";
                  clog << "process recovered";
                  clog << endl;
                }
            }
        }
      break;

    default:
      break;
    }
}

bool
AdapterFormatterPlayer::userEventReceived (SDLInputEvent *ev)
{
  int keyCode;

  keyCode = ev->getKeyCode ();

  if (keyCode == CodeMap::KEY_QUIT)
    {
      this->im = NULL;
      return true;
    }

  if (xruntime_ms () - eventTS < 300)
    {
      return true;
    }

  if (object != NULL && player != NULL)
    {
      clog << "AdapterFormatterPlayer::userEventReceived for '";
      clog << mrl << "' player visibility = '" << player->isVisible ();
      clog << "' event key code = '" << ev->getKeyCode ();
      clog << "'";
      clog << endl;
      if (player->isVisible ())
        {
          eventTS = xruntime_ms ();
          object->selectionEvent (keyCode, player->getMediaTime () * 1000);
        }
    }
  return true;
}

void
AdapterFormatterPlayer::setVisible (bool visible)
{
  NclCascadingDescriptor *descriptor;
  NclFormatterRegion *region;

  descriptor = object->getDescriptor ();
  if (descriptor != NULL)
    {
      region = descriptor->getFormatterRegion ();
      if (region != NULL)
        {
          region->setRegionVisibility (visible);
          player->setVisible (visible);
        }
    }
}

void
AdapterFormatterPlayer::timeShift (const string &direction)
{
  if (player == NULL)
    {
      return;
    }
  player->timeShift (direction);
}

bool
AdapterFormatterPlayer::lockObject ()
{
  if (isLocked)
    {
      return false;
    }
  isLocked = true;
  Thread::mutexLock (&objectMutex);
  return true;
}

bool
AdapterFormatterPlayer::unlockObject ()
{
  if (!isLocked)
    {
      return false;
    }
  Thread::mutexUnlock (&objectMutex);
  isLocked = false;
  return true;
}

GINGA_FORMATTER_END
