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

#ifndef FMDCOMPONENTSUPPORT_H_
#define FMDCOMPONENTSUPPORT_H_

#include "config.h"

#include "mb/IInputManager.h"
#include "mb/LocalScreenManager.h"

#if HAVE_MULTIDEVICE
# include "multidevice/RemoteDeviceManager.h"
# include "multidevice/ActiveDeviceDomain.h"
# include "multidevice/PassiveDeviceDomain.h"
# include "multidevice/BaseDeviceDomain.h"
#endif
#include "gingancl/FormatterMediator.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include "player/ImagePlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace multidevice {
}
}
}
}
}
}

#endif /* FMDCOMPONENTSUPPORT_H_ */
