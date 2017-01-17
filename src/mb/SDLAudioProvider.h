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

#ifndef SDLAUDIOPROVIDER_H_
#define SDLAUDIOPROVIDER_H_

#include "IContinuousMediaProvider.h"

#include "SDL2ffmpeg.h"

#include <set>
using namespace std;

GINGA_MB_BEGIN

	class SDLAudioProvider : public IContinuousMediaProvider {
		protected:
			GingaScreenID myScreen;
			string symbol;
			string mrl;

			SDL2ffmpeg* decoder;

		public:
			SDLAudioProvider(GingaScreenID screenId, const char* mrl);
			virtual ~SDLAudioProvider();

			virtual void setLoadSymbol(string symbol);
			virtual string getLoadSymbol();

			virtual bool getHasVisual(){return false;};

			virtual void setAVPid(int aPid, int vPid){};
			virtual void feedBuffers(){};
			void* getProviderContent();
			virtual void setProviderContent(void* content){};
			virtual bool checkVideoResizeEvent(GingaSurfaceID frame){return false;};
			double getTotalMediaTime();
			float getSoundLevel();
			virtual int64_t getVPts();
			double getMediaTime();
			void setMediaTime(double pos);

		public:
			void playOver(GingaSurfaceID surface);
			void resume(GingaSurfaceID surface);
			void pause();
			void stop();
			void setSoundLevel(float level);
			bool releaseAll();
			void getOriginalResolution(int* width, int* height);

			virtual void refreshDR(void* data);
	};

GINGA_MB_END
#endif /*SDLAUDIOPROVIDER_H_*/
