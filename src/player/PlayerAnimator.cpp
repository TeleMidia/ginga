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

#include "PlayerAnimator.h"

#include "ginga-color-table.h"

#include "util/functions.h"
using namespace ::ginga::util;

#include "mb/Display.h"
using namespace ::ginga::mb;

PlayerAnimator::PlayerAnimator ()
{
  this->properties = NULL;
}

PlayerAnimator::~PlayerAnimator ()
{

}

void
PlayerAnimator::addProperty(const string &dur,
                            const string &name,
                            const string &value)
{
  if(name == "bounds")
    {
      vector<string> params = split(value, ',');
      if(params.size () == 4)
        {
          updateList(dur, "left", params[0]);
          updateList(dur, "top", params[1]);
          updateList(dur, "width", params[2]);
          updateList(dur, "height", params[3]);
        }
    }
   else if(name == "background" || name == "bgColor")
    {
      SDL_Color color;
      ginga_color_input_to_sdl_color(value, &color);
      updateList(dur,"red", to_string(color.r));
      updateList(dur,"green", to_string(color.g));
      updateList(dur,"blue", to_string(color.b));
    }
  else
    updateList(dur, name, value);

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
      pr->targetValue = xstrtodorpercent (value) * 255.;
    else
      pr->targetValue = xstrtodorpercent (value) * 100.;
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
PlayerAnimator::calculeVelocity(gint32 * value, ANIM_PROPERTY* pr)
{
  pr->velocity = getAnimationVelocity( (gdouble)*value,
                                       pr->targetValue,
                                       pr->duration );
  pr->curValue = (gdouble)*value;

  if((guint32)pr->velocity == 0)
    {
      *value = (guint32)pr->targetValue;
      this->properties = g_list_remove(this->properties, pr);
      delete pr;
      return false;
    }
  return true;
}

bool
PlayerAnimator::calculeVelocity(guint8 * value, ANIM_PROPERTY* pr)
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
PlayerAnimator::calculePosition(gint32 * value, ANIM_PROPERTY* pr, gint32 dir)
{ //S = So + vt
  pr->curValue = pr->curValue +
      (dir * (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps())));
  *value = (guint32)pr->curValue;

  if( (dir > 0 && *value >= pr->targetValue) ||
      (dir < 0 && *value <= pr->targetValue) )
    {
      *value = (guint32)pr->targetValue;
      this->properties = g_list_remove(this->properties, pr);
      free(pr);
    }
}

void
PlayerAnimator::calculeColor(guint8* value, ANIM_PROPERTY* pr, gint32 dir)
{ //S = So + vt
  pr->curValue = pr->curValue +
      (dir * (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps())));
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
        if(!calculeVelocity(alpha, pr))
          return;

      if( (gdouble)(*alpha) < pr->targetValue)
        calculeColor(alpha, pr, 1);
      else if( (gdouble)(*alpha) > pr->targetValue)
        calculeColor(alpha, pr, -1);
    }
  else if(pr->name == "red")
    {
      if(pr->velocity <=0)
        if(!calculeVelocity(red, pr))
          return;

      if( (gdouble)(*red) < pr->targetValue)
        calculeColor(red, pr, 1);
      else if( (gdouble)(*red) > pr->targetValue)
        calculeColor(red, pr, -1);
    }
  else if(pr->name == "green")
    {
      if(pr->velocity <=0)
        if(!calculeVelocity(green, pr))
          return;

      if( (gdouble)(*green) < pr->targetValue)
        calculeColor(green, pr, 1);
      else if( (gdouble)(*green) > pr->targetValue)
        calculeColor(green, pr, -1);
    }
  else if(pr->name == "blue")
    {
      if(pr->velocity <=0)
        if(!calculeVelocity(blue, pr))
          return;

      if( (gdouble)(*blue) < pr->targetValue)
        calculeColor(blue, pr, 1);
      else if( (gdouble)(*blue) > pr->targetValue)
        calculeColor(blue, pr, -1);
    }
}

void
PlayerAnimator::updatePosition(SDL_Rect* rect, ANIM_PROPERTY* pr)
{
  if(pr == NULL || rect == NULL)
    return;

  if(pr->name == "top")
    {
      if(pr->velocity <=0)
        if(!calculeVelocity(&rect->y, pr))
          return;

      if(rect->y < pr->targetValue)
        calculePosition(&rect->y, pr, 1);
      else if(rect->y > pr->targetValue)
        calculePosition(&rect->y, pr, -1);
    }
  else if(pr->name == "left")
    {
      if(pr->velocity <= 0)
        if(!calculeVelocity(&rect->x, pr))
          return;

      if(rect->x < pr->targetValue)
        calculePosition(&rect->x, pr, 1);
      else if(rect->x > pr->targetValue)
        calculePosition(&rect->x, pr, -1);
    }
  else if(pr->name == "width")
    {
      if(pr->velocity <=0)
        if(!calculeVelocity(&rect->w, pr))
          return;

      if(rect->w < pr->targetValue)
        calculePosition(&rect->w, pr, 1);
      else if(rect->w > pr->targetValue)
        calculePosition(&rect->w, pr, -1);
    }
  else if(pr->name == "height")
    {
      if(pr->velocity <=0)
        if(!calculeVelocity(&rect->h, pr))
          return;

      if(rect->h < pr->targetValue)
        calculePosition(&rect->h, pr, 1);
      else if(rect->h > pr->targetValue)
        calculePosition(&rect->h, pr, -1);
    }

  // g_debug("\n\n inside::: %f %s %f \n\n",
  //          pr->duration,
  //          pr->name.c_str(),
  //          pr->targetValue);
}
