/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef TERMVIDEOPROVIDER_H_
#define TERMVIDEOPROVIDER_H_

#include "mb/interface/IContinuousMediaProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class TermVideoProvider : public IContinuousMediaProvider {
		private:
			GingaScreenID myScreen;

		public:
			TermVideoProvider(GingaScreenID screenId, const char* mrl);
			virtual ~TermVideoProvider();

			void setLoadSymbol(string symbol);
			string getLoadSymbol();

			bool getHasVisual(){return true;};

			void setAVPid(int aPid, int vPid);
			void feedBuffers();
			bool checkVideoResizeEvent(ISurface* frame);
			double getTotalMediaTime();
			int64_t getVPts();
			double getMediaTime();
			void setMediaTime(double pos);
			void playOver(
					ISurface* surface,
					bool hasVisual=true, IProviderListener* listener=NULL);

			void resume(ISurface* surface, bool hasVisual);
			void pause();
			void stop();
			void setSoundLevel(float level);
			void* getProviderContent();
			void setProviderContent(void* content){};
			void getOriginalResolution(int* width, int* height);
			bool releaseAll();
			void refreshDR(void* data);
	};
}
}
}
}
}
}

#endif /*TERMVIDEOPROVIDER_H_*/
