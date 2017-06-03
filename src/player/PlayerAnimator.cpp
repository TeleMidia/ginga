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
#include "PlayerAnimator.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

PlayerAnimator::PlayerAnimator ()
{
  this->properties = NULL;
}

PlayerAnimator::~PlayerAnimator ()
{
  g_list_free(properties);
}

void
PlayerAnimator::addProperty(const string &dur,
                            const string &name,
                            const string &value)
{
  if(name == "bounds")
    {
      vector<string> params = xstrsplit (value, ',');
      if(params.size () == 4)
        {
          updateList (dur, "left", params[0]);
          updateList (dur, "top", params[1]);
          updateList (dur, "width", params[2]);
          updateList (dur, "height", params[3]);
        }
    }
   else if(name == "background" || name == "bgColor")
    {
      SDL_Color color = {0, 0, 0, 255};
      g_assert (ginga_color_parse (value, &color));
      updateList (dur, "red", xstrbuild ("%d", color.r));
      updateList (dur, "green", xstrbuild ("%d", color.g));
      updateList (dur, "blue", xstrbuild ("%d", color.b));
    }
  else
    {
      updateList (dur, name, value);
    }

}

void
PlayerAnimator::updateList(const string &dur,
                           const string &name,
                           const string &value)
{
  ANIM_PROPERTY* pr = new ANIM_PROPERTY;
  pr->name = name;
  pr->duration = xstrtod(dur);
  pr->curValue = 0;
  pr->velocity = 0;
  if (xstrispercent (value))
    if(name == "transparency")
      pr->targetValue = xstrtodorpercent (value, NULL) * 255.;
    else
      pr->targetValue = xstrtodorpercent (value, NULL) * 100.;
  else
    if(name == "transparency"){
        pr->targetValue = xstrtod (value) * 255.;
      }
    else
      pr->targetValue = xstrtod (value);

  this->properties = g_list_insert (this->properties, pr,-1);
}

void
PlayerAnimator::update(SDL_Rect* rect,
                       guint8* red, guint8* green, guint8* blue, guint8* alpha)
{
  GList* l =  this->properties;
  while (l != NULL)
    {
      GList *next = l->next;
      ANIM_PROPERTY* pr = (ANIM_PROPERTY*)l->data;

      if(!pr)
        this->properties = g_list_remove_link (this->properties, l);
      else
        {
          if(pr->name == "top" ||
             pr->name == "left" ||
             pr->name == "width" ||
             pr->name == "height")
            {
              updatePosition(rect, pr);
            }
          else if(pr->name == "transparency" ||
                  pr->name == "red" ||
                  pr->name == "blue" ||
                  pr->name == "green" ){
              updateColor(red, green, blue, alpha, pr);
            }
        }

      l = next;
    }
}

gdouble
PlayerAnimator::cvtTimeIntToDouble(guint32 value)
{
  return ((gdouble)value)/1000;
}

gdouble
PlayerAnimator::getAnimationVelocity( gdouble initPos,
                                      gdouble finalPos,
                                      gdouble duration )
{
  if(duration <= 0)
    return 0;

  // g_debug("iPos: %f fPos: %f \n", initPos, finalPos);

  gdouble distance = finalPos - initPos;

  if(distance < 0)
    distance*=-1;

  g_debug("distance: %f velocity: %f \n", distance, (distance / duration) );

  return distance / duration;
}

bool
PlayerAnimator::calculateVelocity(gint32 * value, ANIM_PROPERTY* pr)
{
  pr->velocity = getAnimationVelocity( (gdouble)*value,
                                       pr->targetValue,
                                       pr->duration );
  pr->curValue = (gdouble)*value;

  if((guint32)pr->velocity == 0)
    {
      *value = (gint32)pr->targetValue;
      this->properties = g_list_remove(this->properties, pr);
      delete pr;
      return false;
    }
  return true;
}

bool
PlayerAnimator::calculateVelocity(guint8 * value, ANIM_PROPERTY* pr)
{
  pr->velocity = getAnimationVelocity( (gdouble)*value,
                                       pr->targetValue,
                                       pr->duration );
  pr->curValue = (gdouble)*value;

  if((guint32)pr->velocity == 0)
    {
      *value = (guint8)pr->targetValue;
      this->properties = g_list_remove(this->properties, pr);
      delete pr;
      return false;
    }
  return true;
}

void
PlayerAnimator::calculatePosition(gint32 * value, ANIM_PROPERTY* pr, gint32 dir)
{ //S = So + vt
  pr->curValue = pr->curValue +
      (dir * (pr->velocity * (1.0/(gdouble)Ginga_Display->getFPS())));
  *value = (gint32)pr->curValue;

  if( (dir > 0 && *value >= pr->targetValue) ||
      (dir < 0 && *value <= pr->targetValue) )
    {
      *value = (gint32)pr->targetValue;
      this->properties = g_list_remove(this->properties, pr);
      free(pr);
    }
}

void
PlayerAnimator::calculateColor(guint8* value, ANIM_PROPERTY* pr, gint32 dir)
{ //S = So + vt
  pr->curValue = pr->curValue +
      (dir * (pr->velocity * (1.0/(gdouble)Ginga_Display->getFPS())));
  *value = (guint8)pr->curValue;

  if( (dir > 0 && *value >= pr->targetValue) ||
      (dir < 0 && *value <= pr->targetValue) )
    {
      *value = (guint8)pr->targetValue;
      this->properties = g_list_remove(this->properties, pr);
      free(pr);
    }
}

void
PlayerAnimator::updateColor(guint8* red, guint8* green,
                            guint8* blue, guint8* alpha,
                            ANIM_PROPERTY* pr)
{
  if(alpha == NULL)
    return;

  if(pr->name == "transparency")
    {
      if(pr->velocity <=0)
        if(!calculateVelocity(alpha, pr))
          return;

      if( (gdouble)(*alpha) < pr->targetValue)
        calculateColor(alpha, pr, 1);
      else if( (gdouble)(*alpha) > pr->targetValue)
        calculateColor(alpha, pr, -1);
    }
  else if(pr->name == "red")
    {
      if(pr->velocity <=0)
        if(!calculateVelocity(red, pr))
          return;

      if( (gdouble)(*red) < pr->targetValue)
        calculateColor(red, pr, 1);
      else if( (gdouble)(*red) > pr->targetValue)
        calculateColor(red, pr, -1);
    }
  else if(pr->name == "green")
    {
      if(pr->velocity <=0)
        if(!calculateVelocity(green, pr))
          return;

      if( (gdouble)(*green) < pr->targetValue)
        calculateColor(green, pr, 1);
      else if( (gdouble)(*green) > pr->targetValue)
        calculateColor(green, pr, -1);
    }
  else if(pr->name == "blue")
    {
      if(pr->velocity <=0)
        if(!calculateVelocity(blue, pr))
          return;

      if( (gdouble)(*blue) < pr->targetValue)
        calculateColor(blue, pr, 1);
      else if( (gdouble)(*blue) > pr->targetValue)
        calculateColor(blue, pr, -1);
    }
}

void
PlayerAnimator::updatePosition(SDL_Rect* rect, ANIM_PROPERTY* pr)
{
  if(pr == NULL || rect == NULL)
    return;

  if(pr->name == "top")
    {
      if(pr->velocity <= 0)
        if(!calculateVelocity(&rect->y, pr))
          return;

      if(rect->y < pr->targetValue)
        calculatePosition(&rect->y, pr, 1);
      else if(rect->y > pr->targetValue)
        calculatePosition(&rect->y, pr, -1);
    }
  else if(pr->name == "left")
    {
      if(pr->velocity <= 0)
        if(!calculateVelocity(&rect->x, pr))
          return;

      if(rect->x < pr->targetValue)
        calculatePosition(&rect->x, pr, 1);
      else if(rect->x > pr->targetValue)
        calculatePosition(&rect->x, pr, -1);
    }
  else if(pr->name == "width")
    {
      if(pr->velocity <=0)
        if(!calculateVelocity(&rect->w, pr))
          return;

      if(rect->w < pr->targetValue)
        calculatePosition(&rect->w, pr, 1);
      else if(rect->w > pr->targetValue)
        calculatePosition(&rect->w, pr, -1);
    }
  else if(pr->name == "height")
    {
      if(pr->velocity <=0)
        if(!calculateVelocity(&rect->h, pr))
          return;

      if(rect->h < pr->targetValue)
        calculatePosition(&rect->h, pr, 1);
      else if(rect->h > pr->targetValue)
        calculatePosition(&rect->h, pr, -1);
    }

  // g_debug("\n\n inside::: %f %s %f \n\n",
  //          pr->duration,
  //          pr->name.c_str(),
  //          pr->targetValue);
}
