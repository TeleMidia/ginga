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

#ifndef TRANSITION_H_
#define TRANSITION_H_

#include "util/Color.h"
using namespace ::ginga::util;

#include "../Entity.h"
using namespace ::br::pucrio::telemidia::ncl;

BR_PUCRIO_TELEMIDIA_NCL_TRANSITION_BEGIN

	class Transition : public Entity {
		public:
			static const int TYPE_BARWIPE = 0;
			static const int TYPE_IRISWIPE = 1;
			static const int TYPE_CLOCKWIPE = 2;
			static const int TYPE_SNAKEWIPE = 3;
			static const int TYPE_FADE = 4;

			static const int SUBTYPE_BARWIPE_LEFTTORIGHT = 0;
			static const int SUBTYPE_BARWIPE_TOPTOBOTTOM = 1;

			static const int SUBTYPE_IRISWIPE_RECTANGLE = 20;
			static const int SUBTYPE_IRISWIPE_DIAMOND = 21;

			static const int SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE = 40;
			static const int SUBTYPE_CLOCKWIPE_CLOCKWISETHREE = 41;
			static const int SUBTYPE_CLOCKWIPE_CLOCKWISESIX = 42;
			static const int SUBTYPE_CLOCKWIPE_CLOCKWISENINE = 43;

			static const int SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL = 60;
			static const int SUBTYPE_SNAKEWIPE_TOPLEFTVERTICAL = 61;
			static const int SUBTYPE_SNAKEWIPE_TOPLEFTDIAGONAL = 62;
			static const int SUBTYPE_SNAKEWIPE_TOPRIGHTDIAGONAL = 63;
			static const int SUBTYPE_SNAKEWIPE_BOTTOMRIGHTDIAGONAL = 64;
			static const int SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL = 65;

			static const int SUBTYPE_FADE_CROSSFADE = 80;
			static const int SUBTYPE_FADE_FADETOCOLOR = 81;
			static const int SUBTYPE_FADE_FADEFROMCOLOR = 82;

			static const short DIRECTION_FORWARD = 0;
			static const short DIRECTION_REVERSE = 1;

		private:
			int type;
			int subtype;
			double dur;
			double startProgress;
			double endProgress;
			short direction;
			Color* fadeColor;
			int horzRepeat;
			int vertRepeat;
			Color* borderColor;
			int borderWidth;

		public:
			Transition(string id, int type);
			virtual ~Transition();
			Color* getBorderColor();
			int getBorderWidth();
			short getDirection();
			double getDur();
			double getEndProgress();
			Color* getFadeColor();
			int getHorzRepeat();
			double getStartProgress();
			int getSubtype();
			int getType();
			int getVertRepeat();
			void setBorderColor(Color* color);
			void setBorderWidth(int width);
			void setDirection(short dir);
			void setDur(double dur);
			void setEndProgress(double ep);
			void setFadeColor(Color* color);
			void setHorzRepeat(int num);
			void setStartProgress(double sp);
			void setSubtype(int subtype);
			void setType(int type);
			void setVertRepeat(int num);
	};

BR_PUCRIO_TELEMIDIA_NCL_TRANSITION_END
#endif /*TRANSITION_H_*/
