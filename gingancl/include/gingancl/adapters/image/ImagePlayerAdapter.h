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

#ifndef IMAGEPLAYERADAPTER_H_
#define IMAGEPLAYERADAPTER_H_

#include "../FormatterPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "../../model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "../../model/FormatterEvent.h"
#include "../../model/PresentationEvent.h"
#include "../../model/SelectionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace image {
	class ImagePlayerAdapter : public FormatterPlayerAdapter {
		public:
			ImagePlayerAdapter();
			virtual ~ImagePlayerAdapter(){};

		private:
			void initializeInstance(std::string& data, short scenario);
			void testInstance(std::string& data, short scenario);

		protected:
			void createPlayer();
   };
}
}
}
}
}
}
}

#endif /*IMAGEPLAYERADAPTER_H_*/
