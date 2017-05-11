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

#include "util/functions.h"
using namespace ::ginga::util;

#include "mb/Display.h"
using namespace ::ginga::mb;

PlayerAnimator::PlayerAnimator (){
   this->properties = NULL; 
}

PlayerAnimator::~PlayerAnimator (){

}

void
PlayerAnimator::addProperty(string dur, string name, string value){
    
   GList* l =  this->properties;           
   while (l != NULL){
     GList *next = l->next;
     ANIM_PROPERTY* pr = (ANIM_PROPERTY*)l->data;
     if(!pr)
     this->properties = g_list_remove_link (this->properties, l);
    else {
       if(pr->name == name){
           pr->velocity = 0;
           pr->duration = xstrtod(dur);
           if(isPercentualValue (value))
             pr->targetValue = getPercentualValue (value);
           else 
             pr->targetValue = xstrtod (value);
           return;  
       }
    }      
    l = next;
   }

  ANIM_PROPERTY* pr = (ANIM_PROPERTY*)malloc(sizeof(ANIM_PROPERTY));
  pr->name = name;
  pr->duration = xstrtod(dur);
  pr->curValue = 0;
  pr->velocity = 0;
  if(isPercentualValue (value))
     pr->targetValue = getPercentualValue (value);
  else 
     pr->targetValue = xstrtod (value);

  this->properties = g_list_insert(this->properties, pr,-1);
}

void
PlayerAnimator::update(SDL_Rect* rect){
   GList* l =  this->properties;           
   while (l != NULL){
     GList *next = l->next;
     ANIM_PROPERTY* pr = (ANIM_PROPERTY*)l->data;
     if(!pr)
       this->properties = g_list_remove_link (this->properties, l);
     else {
       if(pr->name == "top" || pr->name == "left" || pr->name == "width" || pr->name == "height")
         updatePosition(rect,pr);
     }
      l = next;
   }
}

void
PlayerAnimator::updatePosition(SDL_Rect* rect, ANIM_PROPERTY* pr){
    if(pr == NULL || rect == NULL)
      return;
  
   if(pr->name == "top"){ 
       if(pr->velocity <=0){
            pr->velocity = getAnimationVelocity((gdouble)rect->y, pr->targetValue, pr->duration);
            pr->curValue = (gdouble)rect->y;
       }
       if(pr->velocity ==0){
          rect->y = (guint32)pr->targetValue; 
          this->properties = g_list_remove(this->properties, pr);
          free(pr);
          return;
       }
       else if(rect->y < pr->targetValue){ 
          pr->curValue = pr->curValue + (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->y = (guint32)pr->curValue; 
          if(rect->y >= pr->targetValue){
               rect->y = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
       else if(rect->y > pr->targetValue){
          pr->curValue = pr->curValue - (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->y = (guint32)pr->curValue; 
          if(rect->y <= pr->targetValue){
               rect->y = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
   }
   else if(pr->name == "left"){
      
       if(pr->velocity <=0){
            pr->velocity = getAnimationVelocity((gdouble)rect->x, pr->targetValue, pr->duration);
            pr->curValue = (gdouble)rect->x;
       }
       if(pr->velocity ==0){
          rect->x = (guint32)pr->targetValue; 
          this->properties = g_list_remove(this->properties, pr);
          free(pr);
          return;
       }
       else if(rect->x < pr->targetValue){ //S = So + vt
          pr->curValue = pr->curValue + (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->x = (guint32)pr->curValue; 
          if(rect->x >= pr->targetValue){
               rect->x = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
       else if(rect->x > pr->targetValue){
          pr->curValue = pr->curValue - (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->x = (guint32)pr->curValue; 
          if(rect->x <= pr->targetValue){
               rect->x = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
   }
   else if(pr->name == "width"){
      
       if(pr->velocity <=0){
            pr->velocity = getAnimationVelocity((gdouble)rect->w, pr->targetValue, pr->duration);
            pr->curValue = (gdouble)rect->w;
       }
       if(pr->velocity ==0){
          rect->w = (guint32)pr->targetValue; 
          this->properties = g_list_remove(this->properties, pr);
          free(pr);
          return;
       }
       else if(rect->w < pr->targetValue){ //S = So + vt
          pr->curValue = pr->curValue + (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->w = (guint32)pr->curValue; 
          if(rect->w >= pr->targetValue){
               rect->w = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
       else if(rect->w > pr->targetValue){
          pr->curValue = pr->curValue - (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->w = (guint32)pr->curValue; 
          if(rect->w <= pr->targetValue){
               rect->w = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
   }
   else if(pr->name == "height"){
      
       if(pr->velocity <=0){
            pr->velocity = getAnimationVelocity((gdouble)rect->h, pr->targetValue, pr->duration);
            pr->curValue = (gdouble)rect->h;
       }
       if(pr->velocity ==0){
          rect->h = (guint32)pr->targetValue; 
          this->properties = g_list_remove(this->properties, pr);
          free(pr);
          return;
       }
       else if(rect->h < pr->targetValue){ //S = So + vt
          pr->curValue = pr->curValue + (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->h = (guint32)pr->curValue; 
          if(rect->h >= pr->targetValue){
               rect->h = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
       else if(rect->h > pr->targetValue){
          pr->curValue = pr->curValue - (pr->velocity * (1.0/(gdouble)Ginga_Display->getFps()));
          rect->h = (guint32)pr->curValue; 
          if(rect->h <= pr->targetValue){
               rect->h = (guint32)pr->targetValue; 
               this->properties = g_list_remove(this->properties, pr);
               free(pr);
          }
       }
   }
   
  

 //   g_debug("\n\n inside::: %f %s %f \n\n",pr->duration,pr->name.c_str(),pr->targetValue);
    
}

