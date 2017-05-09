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

#ifndef _CONTEXTBASE_H_
#define _CONTEXTBASE_H_

GINGA_FORMATTER_BEGIN

const string SYSTEM_LANGUAGE ("system.language");
const string SYSTEM_CAPTION ("system.caption");
const string SYSTEM_SUBTITLE ("system.subtitle");
const string SYSTEM_RETURN_BIT_RATE ("system.returnBitRate");
const string SYSTEM_SCREEN_SIZE ("system.screenSize");
const string SYSTEM_SCREEN_GRAPHIC_SIZE ("system.screenGraphicSize");

const string SYSTEM_AUDIO_TYPE ("system.audioType");
const string SYSTEM_CPU ("system.CPU");
const string SYSTEM_MEMORY ("system.memory");
const string SYSTEM_OPERATING_SYSTEM ("system.operatingSystem");
const string SYSTEM_JAVA_CONFIG ("system.javaConfiguration");
const string SYSTEM_JAVA_PROFILE ("system.javaProfile");
const string SYSTEM_LUA_VERSION ("system.luaVersion");

const string SYSTEM_DEVNUMBER ("system.devNumber");
const string SYSTEM_CLASSTYPE ("system.classType");
const string SYSTEM_INFO ("system.info");
const string SYSTEM_CLASS_NUMBER ("system.classNumber");

const string USER_AGE ("user.age");
const string USER_LOCATION ("user.location");
const string USER_GENRE ("user.genre");

const string DEFAULT_FOCUS_BORDER_COLOR ("default.focusBorderColor");
const string DEFAULT_SEL_BORDER_COLOR ("default.selBorderColor");
const string DEFAULT_FOCUS_BORDER_WIDTH ("default.focusBorderWidth");
const string DEFAULT_FOCUS_BORDER_TRANSPARENCY ("default.focusBorderTransparency");

GINGA_FORMATTER_END

#endif //_CONTEXTBASE_H_
