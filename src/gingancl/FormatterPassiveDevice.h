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

#ifndef _FormatterPassiveDevice_H_
#define _FormatterPassiveDevice_H_

#include "FormatterMultiDevice.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MULTIDEVICE_BEGIN

	class FormatterPassiveDevice : public FormatterMultiDevice {
		public:
			FormatterPassiveDevice(
					GingaScreenID screenId,
					IDeviceLayout* deviceLayout,
					int x, int y, int w, int h, bool useMulticast, int srvPort);

			virtual ~FormatterPassiveDevice();

		protected:
			void postMediaContent(int destDevClass){};
			bool newDeviceConnected(int newDevClass, int w, int h) {
				return false;
			};

			void connectedToBaseDevice(unsigned int domainAddr);

			bool receiveRemoteEvent(
					int remoteDevClass,
					int eventType,
					string eventContent) {

				return false;
			};

			bool receiveRemoteContent(
					int remoteDevClass,
					char *stream, int streamSize) {

				return false;
			};

			bool receiveRemoteContent(int remoteDevClass, string contentUri);
			bool userEventReceived(SDLInputEvent* ev);
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MULTIDEVICE_END
#endif /* _FormatterPassiveDevice_H_ */
