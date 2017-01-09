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

#ifndef DOWNLOADSERVERINITIATE_H_
#define DOWNLOADSERVERINITIATE_H_

extern "C" {
	#include <stdio.h>
}

#include "Module.h"
#include "../Ior.h"
#include "DsmccMessageHeader.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	class DownloadServerInitiate {
		private:
			DsmccMessageHeader* header;
			Ior* srgIor;
			char* data;
			unsigned int objectKey;

			// Payload offset.
			unsigned int idx;

		public:
			DownloadServerInitiate();
			virtual ~DownloadServerInitiate();

			int processMessage(DsmccMessageHeader* message);

			Ior* getServiceGatewayIor();

		private:
			int processIor();
	};
}
}
}
}
}
}
}

#endif /*DOWNLOADSERVERINITIATE_H_*/
