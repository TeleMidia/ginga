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

/**
 * @file   AdapterHTMLPlayer.h
 * @author Eduardo Cruz Araújo (edcaraujo@gmail.com)
 * @date   April, 2017
 *
 * @brief  Header file of HTML player adapter.
 *
 * This is the header file for the AdapterHTMLPlayer class.
 */

#ifndef ADAPTER_HTML_PLAYER_H
#define ADAPTER_HTML_PLAYER_H

#include "ginga.h"

#include "AdapterFormatterPlayer.h"

GINGA_FORMATTER_BEGIN

/**
 *  @brief An adapter class for the HTML player.
 * 
 *  This class is responsible for create new instances of 
 *  HTML players.
 */ 
class AdapterHTMLPlayer : public AdapterFormatterPlayer
{
public:
  /**
   * @brief Default constructor for AdapterHTMLPlayer.  
   * 
   * @return Nothing. 
   */  
  AdapterHTMLPlayer ();

  /**
   * @brief Default destructor for AdapterHTMLPlayer.  
   *
   * @return Nothing. 
   */ 
  virtual ~AdapterHTMLPlayer ();

protected:
  /**
   * @brief Create a new instance of HTML player. 
   *
   * @return Nothing. 
   *
   * This method associate the new HTML player instance
   * to the protected property **player** inherited from 
   * AdapterFormatterPlayer class.
   */ 
  void createPlayer ();
};

GINGA_FORMATTER_END

#endif /* ADAPTER_HTML_PLAYER_H */
