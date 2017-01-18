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

#include "config.h"
#include "NclCascadingDescriptor.h"
#include "NclFormatterLayout.h"

#include "ncl/ContentNode.h"
#include "ncl/NodeEntity.h"
using namespace ::ginga::ncl;

#include "NclExecutionObject.h"

GINGA_FORMATTER_BEGIN

int NclCascadingDescriptor::dummyCount = 0;

NclCascadingDescriptor::NclCascadingDescriptor (
    GenericDescriptor *firstDescriptor)
{

  initializeCascadingDescriptor ();

  if (firstDescriptor != NULL)
    {
      cascade (firstDescriptor);
    }
}

NclCascadingDescriptor::NclCascadingDescriptor (
    NclCascadingDescriptor *descriptor)
{
  int i, size;

  initializeCascadingDescriptor ();
  if (descriptor != NULL)
    {
      size = ((NclCascadingDescriptor *)descriptor)->descriptors.size ();
      for (i = 0; i < size; i++)
        {
          cascade (
              (GenericDescriptor *)(((NclCascadingDescriptor *)descriptor)
                                        ->descriptors[i]));
        }

      size = ((NclCascadingDescriptor *)descriptor)
                 ->unsolvedDescriptors.size ();

      for (i = 0; i < size; i++)
        {
          cascade (
              (GenericDescriptor *)(((NclCascadingDescriptor *)descriptor)
                                        ->unsolvedDescriptors[i]));
        }
    }
}

NclCascadingDescriptor::~NclCascadingDescriptor ()
{
  vector<GenericDescriptor *>::iterator i;
  DescriptorSwitch *ds;
  focusBorderColor = NULL;
  selBorderColor = NULL;

  clog << "NclCascadingDescriptor::~NclCascadingDescriptor(";
  clog << getId () << ")" << endl;

  if (formatterRegion != NULL)
    {
      delete formatterRegion;
      formatterRegion = NULL;
    }

  if (inputTransitions != NULL)
    {
      delete inputTransitions;
      inputTransitions = NULL;
    }

  if (outputTransitions != NULL)
    {
      delete outputTransitions;
      outputTransitions = NULL;
    }

  i = descriptors.begin ();
  while (i != descriptors.end ())
    {
      if (DescriptorSwitch::hasInstance ((DescriptorSwitch *)*i, false))
        {

          ds = dynamic_cast<DescriptorSwitch *> (*i);
          if (ds != NULL)
            {
              ds->select (NULL);
            }
        }
      ++i;
    }
}

void
NclCascadingDescriptor::initializeCascadingDescriptor ()
{
  typeSet.insert ("NclCascadingDescriptor");

  id = "";
  explicitDuration = NaN ();
  playerName = "";
  repetitions = 0;
  freeze = false;
  region = NULL;
  formatterRegion = NULL;

  focusIndex = "";
  moveUp = "";
  moveDown = "";
  moveLeft = "";
  moveRight = "";

  focusSrc = "";
  selectionSrc = "";
  focusBorderColor = NULL;
  selBorderColor = NULL;
  focusBorderWidth = 0;
  selBorderWidth = 0;
  focusBorderTransparency = NaN ();
  inputTransitions = new vector<Transition *>;
  outputTransitions = new vector<Transition *>;
}

bool
NclCascadingDescriptor::instanceOf (string s)
{
  if (typeSet.empty ())
    return false;
  else
    return (typeSet.find (s) != typeSet.end ());
}

string
NclCascadingDescriptor::getId ()
{
  return id;
}

void
NclCascadingDescriptor::cascadeDescriptor (Descriptor *descriptor)
{
  vector<Transition *> *transitions = NULL;

  if (descriptor->getPlayerName () != "")
    {
      playerName = descriptor->getPlayerName ();
    }

  region = descriptor->getRegion ();
  if (!isNaN (descriptor->getExplicitDuration ()))
    {
      explicitDuration = descriptor->getExplicitDuration ();
    }

  this->freeze = descriptor->isFreeze ();

  if (descriptor->getKeyNavigation () != NULL)
    {
      KeyNavigation *keyNavigation;
      keyNavigation = (KeyNavigation *)(descriptor->getKeyNavigation ());

      if (keyNavigation->getFocusIndex () != "")
        {
          focusIndex = keyNavigation->getFocusIndex ();
        }

      if (keyNavigation->getMoveUp () != "")
        {
          moveUp = keyNavigation->getMoveUp ();
        }

      if (keyNavigation->getMoveDown () != "")
        {
          moveDown = keyNavigation->getMoveDown ();
        }

      if (keyNavigation->getMoveLeft () != "")
        {
          moveLeft = keyNavigation->getMoveLeft ();
        }

      if (keyNavigation->getMoveRight () != "")
        {
          moveRight = keyNavigation->getMoveRight ();
        }
    }

  if (descriptor->getFocusDecoration () != NULL)
    {
      FocusDecoration *focusDecoration;
      focusDecoration
          = (FocusDecoration *)(descriptor->getFocusDecoration ());

      if (focusDecoration->getFocusBorderColor () != NULL)
        {
          focusBorderColor = focusDecoration->getFocusBorderColor ();
        }

      if (focusDecoration->getSelBorderColor () != NULL)
        {
          selBorderColor = focusDecoration->getSelBorderColor ();
        }

      focusBorderWidth = focusDecoration->getFocusBorderWidth ();

      if (!(isNaN (focusDecoration->getFocusBorderTransparency ())))
        {

          focusBorderTransparency
              = focusDecoration->getFocusBorderTransparency ();
        }

      if (focusDecoration->getFocusSrc () != "")
        {
          focusSrc = focusDecoration->getFocusSrc ();
        }

      if (focusDecoration->getFocusSelSrc () != "")
        {
          selectionSrc = focusDecoration->getFocusSelSrc ();
        }
    }

  transitions = descriptor->getInputTransitions ();
  if (transitions != NULL)
    {
      if (this->inputTransitions == NULL)
        {
          this->inputTransitions = new vector<Transition *>;
        }

      inputTransitions->insert (inputTransitions->end (),
                                transitions->begin (), transitions->end ());
    }

  transitions = descriptor->getOutputTransitions ();
  if (transitions != NULL)
    {
      if (this->outputTransitions == NULL)
        {
          this->outputTransitions = new vector<Transition *>;
        }

      outputTransitions->insert (outputTransitions->end (),
                                 transitions->begin (),
                                 transitions->end ());
    }

  Parameter *param;
  vector<Parameter *> *paramsMap;
  vector<Parameter *>::iterator it;

  paramsMap = descriptor->getParameters ();
  if (paramsMap != NULL)
    {
      for (it = paramsMap->begin (); it != paramsMap->end (); ++it)
        {
          param = *it;
          parameters[param->getName ()] = param->getValue ();
        }
      delete paramsMap;
      paramsMap = NULL;
    }
}

bool
NclCascadingDescriptor::isLastDescriptor (GenericDescriptor *descriptor)
{

  if ((descriptors.size () > 0)
      && (descriptor->getId ()
          == ((GenericDescriptor *)descriptors[descriptors.size () - 1])
                 ->getId ()))

    return true;
  else
    return false;
}

void
NclCascadingDescriptor::cascade (GenericDescriptor *descriptor)
{
  GenericDescriptor *preferredDescriptor;

  preferredDescriptor = (GenericDescriptor *)(descriptor->getDataEntity ());

  if ((preferredDescriptor == NULL)
      || preferredDescriptor->instanceOf ("NclCascadingDescriptor"))

    return;

  if (isLastDescriptor (preferredDescriptor))
    return;

  descriptors.push_back (preferredDescriptor);
  if (id == "")
    id = preferredDescriptor->getId ();
  else
    id = id + "+" + preferredDescriptor->getId ();

  if (preferredDescriptor->instanceOf ("Descriptor")
      && unsolvedDescriptors.empty ())
    {

      cascadeDescriptor ((Descriptor *)preferredDescriptor);
    }
  else
    {
      unsolvedDescriptors.push_back (preferredDescriptor);
    }
}

GenericDescriptor *
NclCascadingDescriptor::getUnsolvedDescriptor (int i)
{
  if (i >= unsolvedDescriptors.size ())
    {
      return NULL;
    }

  return unsolvedDescriptors.at (i);
}

vector<GenericDescriptor *> *
NclCascadingDescriptor::getUnsolvedDescriptors ()
{
  return &unsolvedDescriptors;
}

void
NclCascadingDescriptor::cascadeUnsolvedDescriptor ()
{
  if (unsolvedDescriptors.empty ())
    return;

  GenericDescriptor *genericDescriptor, *descriptor;
  DescriptorSwitch *descAlternatives;
  GenericDescriptor *auxDescriptor;

  genericDescriptor = (GenericDescriptor *)(unsolvedDescriptors[0]);

  if (genericDescriptor->instanceOf ("DescriptorSwitch"))
    {
      descAlternatives = (DescriptorSwitch *)genericDescriptor;
      auxDescriptor = descAlternatives->getSelectedDescriptor ();
      descriptor = (GenericDescriptor *)auxDescriptor->getDataEntity ();
    }
  else
    {
      descriptor = (Descriptor *)genericDescriptor;
      unsolvedDescriptors.erase (unsolvedDescriptors.begin ());
    }

  if (isLastDescriptor (descriptor))
    {
      return;
    }

  cascadeDescriptor ((Descriptor *)descriptor);
}

double
NclCascadingDescriptor::getExplicitDuration ()
{
  return (this->explicitDuration);
}

bool
NclCascadingDescriptor::getFreeze ()
{
  return freeze;
}

string
NclCascadingDescriptor::getPlayerName ()
{
  return playerName;
}

LayoutRegion *
NclCascadingDescriptor::getRegion ()
{
  return region;
}

LayoutRegion *
NclCascadingDescriptor::getRegion (void *formatterLayout, void *object)
{

  if (region == NULL)
    {
      createDummyRegion (formatterLayout, object);
    }

  return region;
}

NclFormatterRegion *
NclCascadingDescriptor::getFormatterRegion ()
{
  return formatterRegion;
}

void
NclCascadingDescriptor::updateRegion (void *formatterLayout, string name,
                                      string value)
{

  vector<string> *params;
  bool isPercentual;

  if (name == "left")
    {
      createDummyRegion (formatterLayout);
      value = cvtPercentual (value, &isPercentual);
      region->setLeft (::ginga::util::stof (value), isPercentual);
    }
  else if (name == "top")
    {
      createDummyRegion (formatterLayout);
      value = cvtPercentual (value, &isPercentual);
      region->setTop (::ginga::util::stof (value), isPercentual);
    }
  else if (name == "width")
    {
      createDummyRegion (formatterLayout);
      value = cvtPercentual (value, &isPercentual);
      region->setWidth (::ginga::util::stof (value), isPercentual);
    }
  else if (name == "height")
    {
      createDummyRegion (formatterLayout);
      value = cvtPercentual (value, &isPercentual);
      region->setHeight (::ginga::util::stof (value), isPercentual);
    }
  else if (name == "bottom")
    {
      createDummyRegion (formatterLayout);
      value = cvtPercentual (value, &isPercentual);
      region->setBottom (::ginga::util::stof (value), isPercentual);
    }
  else if (name == "right")
    {
      createDummyRegion (formatterLayout);
      value = cvtPercentual (value, &isPercentual);
      region->setRight (::ginga::util::stof (value), isPercentual);
    }
  else if (name == "zIndex")
    {
      createDummyRegion (formatterLayout);
      region->setZIndex (::ginga::util::stof (value));
    }
  else if (name == "bounds")
    {
      createDummyRegion (formatterLayout);
      params = split (trim (value), ",");
      if (params->size () == 4)
        {
          if (region != NULL)
            {
              value = cvtPercentual ((*params)[0], &isPercentual);

              region->setLeft (::ginga::util::stof (value), isPercentual);

              value = cvtPercentual ((*params)[1], &isPercentual);

              region->setTop (::ginga::util::stof (value), isPercentual);

              value = cvtPercentual ((*params)[2], &isPercentual);

              region->setWidth (::ginga::util::stof (value), isPercentual);

              value = cvtPercentual ((*params)[3], &isPercentual);

              region->setHeight (::ginga::util::stof (value), isPercentual);
            }
        }
      delete params;
    }
  else if (name == "location")
    {
      createDummyRegion (formatterLayout);
      params = split (trim (value), ",");
      if (params->size () == 4)
        {
          if (region != NULL)
            {
              value = cvtPercentual ((*params)[0], &isPercentual);

              region->setLeft (::ginga::util::stof (value), isPercentual);

              value = cvtPercentual ((*params)[1], &isPercentual);

              region->setTop (::ginga::util::stof (value), isPercentual);
            }
        }
      delete params;
    }
  else if (name == "size")
    {
      createDummyRegion (formatterLayout);
      params = split (trim (value), ",");
      if (params->size () == 4)
        {
          if (region != NULL)
            {
              value = cvtPercentual ((*params)[0], &isPercentual);

              region->setWidth (::ginga::util::stof (value), isPercentual);

              value = cvtPercentual ((*params)[1], &isPercentual);

              region->setHeight (::ginga::util::stof (value), isPercentual);
            }
        }
      delete params;
    }
}

void
NclCascadingDescriptor::createDummyRegion (void *formatterLayout,
                                           void *executionObject)
{

  NclExecutionObject *object;
  string name, value;
  map<string, string>::iterator i;

  NodeEntity *dataObject;
  PropertyAnchor *property;
  vector<Anchor *> *anchors;
  vector<Anchor *>::iterator j;

  i = parameters.begin ();
  while (i != parameters.end ())
    {
      name = i->first;
      value = i->second;

      updateRegion (formatterLayout, name, value);

      ++i;
    }

  object = (NclExecutionObject *)executionObject;
  dataObject = (NodeEntity *)(object->getDataObject ());
  if (dataObject != NULL
      && (dataObject->instanceOf ("ContentNode")
          || (dataObject->instanceOf ("ReferNode")
              && ((ReferNode *)dataObject)->getInstanceType () == "new")))
    {

      anchors = ((ContentNode *)dataObject)->getAnchors ();
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

                  updateRegion (formatterLayout, name, value);
                }
              ++j;
            }
        }
    }
}

void
NclCascadingDescriptor::createDummyRegion (void *formatterLayout)
{
  NclFormatterLayout *layout;
  LayoutRegion *deviceRegion;

  if (region == NULL)
    {
      layout = (NclFormatterLayout *)formatterLayout;
      deviceRegion = layout->getDeviceRegion ();

      if (deviceRegion != NULL)
        {
          clog << "NclCascadingDescriptor::createDummyRegion with device ";
          clog << "class = '" << deviceRegion->getDeviceClass () << "' ";
          clog << "inside deviceRegion '" << deviceRegion->getId ();
          clog << "'" << endl;

          region = new LayoutRegion ("dummyRegion" + itos (dummyCount));
          dummyCount++;

          deviceRegion->addRegion (region);
          region->setParent (deviceRegion);
        }
      else
        {
          clog << "NclCascadingDescriptor::createDummyRegion Warning!";
          clog << "can't create dummy region: deviceRegion is NULL";
          clog << endl;
        }
    }
}

void
NclCascadingDescriptor::setFormatterLayout (void *formatterLayout)
{
  if (region == NULL)
    {
      clog << "NclCascadingDescriptor::setFormatterRegion Warning!";
      clog << " region == NULL";
      clog << endl;
      return;
    }

  if (this->formatterRegion != NULL)
    {
      /*
       * occurs only for DescriptorSwitch or after an object "restart"
       */
      delete this->formatterRegion;
    }

  formatterRegion = new NclFormatterRegion (id, this, formatterLayout);
}

long
NclCascadingDescriptor::getRepetitions ()
{
  return repetitions;
}

vector<Parameter *> *
NclCascadingDescriptor::getParameters ()
{
  vector<Parameter *> *params;
  params = new vector<Parameter *>;
  Parameter *parameter;

  map<string, string>::iterator it;

  for (it = parameters.begin (); it != parameters.end (); ++it)
    {
      parameter = new Parameter (it->first, it->second);
      params->push_back (parameter);
    }
  return params;
}

string
NclCascadingDescriptor::getParameterValue (string paramName)
{
  string::size_type pos;
  string paramValue;
  double value;

  if (parameters.count (paramName) == 0)
    {
      return "";
    }

  paramValue = parameters[paramName];

  if (paramValue == "")
    {
      return paramValue;
    }

  pos = paramValue.find_last_of ("%");
  if (pos != std::string::npos && pos == paramValue.length () - 1)
    {
      paramValue = paramValue.substr (0, paramValue.length () - 1);
      if (isNumeric ((void *)(paramValue.c_str ())))
        {
          value = ::ginga::util::stof (paramValue) / 100;
          paramValue = itos (value);
        }
    }

  return paramValue;
}

vector<GenericDescriptor *> *
NclCascadingDescriptor::getNcmDescriptors ()
{
  return &descriptors;
}

Color *
NclCascadingDescriptor::getFocusBorderColor ()
{
  return focusBorderColor;
}

double
NclCascadingDescriptor::getFocusBorderTransparency ()
{
  return focusBorderTransparency;
}

int
NclCascadingDescriptor::getFocusBorderWidth ()
{
  return focusBorderWidth;
}

string
NclCascadingDescriptor::getFocusIndex ()
{
  return focusIndex;
}

string
NclCascadingDescriptor::getFocusSrc ()
{
  return focusSrc;
}

string
NclCascadingDescriptor::getSelectionSrc ()
{
  return selectionSrc;
}

string
NclCascadingDescriptor::getMoveDown ()
{
  return moveDown;
}

string
NclCascadingDescriptor::getMoveLeft ()
{
  return moveLeft;
}

string
NclCascadingDescriptor::getMoveRight ()
{
  return moveRight;
}

string
NclCascadingDescriptor::getMoveUp ()
{
  return moveUp;
}

Color *
NclCascadingDescriptor::getSelBorderColor ()
{
  return selBorderColor;
}

int
NclCascadingDescriptor::getSelBorderWidth ()
{
  return selBorderWidth;
}

vector<Transition *> *
NclCascadingDescriptor::getInputTransitions ()
{
  return this->inputTransitions;
}

vector<Transition *> *
NclCascadingDescriptor::getOutputTransitions ()
{
  return this->outputTransitions;
}

GINGA_FORMATTER_END
