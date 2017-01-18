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

#ifndef IAPPLICATIONPLAYER_H_
#define IAPPLICATIONPLAYER_H_


GINGA_PLAYER_BEGIN

	//debug purpose
	static const string EC_DBG_START_PRESENTATION        = "0x255";

	//nclEdit base
	static const string EC_OPEN_BASE                     = "0x00";
	static const string EC_ACTIVATE_BASE                 = "0x01";
	static const string EC_DEACTIVATE_BASE               = "0x02";
	static const string EC_SAVE_BASE                     = "0x03";
	static const string EC_CLOSE_BASE                    = "0x04";

	//nclEdit document
	static const string EC_ADD_DOCUMENT                  = "0x05";
	static const string EC_REMOVE_DOCUMENT               = "0x06";
	static const string EC_START_DOCUMENT                = "0x07";
	static const string EC_STOP_DOCUMENT                 = "0x08";
	static const string EC_PAUSE_DOCUMENT                = "0x09";
	static const string EC_RESUME_DOCUMENT               = "0x0A";

	//nclEdit region
	static const string EC_ADD_REGION                    = "0x0B";
	static const string EC_REMOVE_REGION                 = "0x0C";
	static const string EC_ADD_REGION_BASE               = "0x0D";
	static const string EC_REMOVE_REGION_BASE            = "0x0E";

	//nclEdit rule
	static const string EC_ADD_RULE                      = "0x0F";
	static const string EC_REMOVE_RULE                   = "0x10";
	static const string EC_ADD_RULE_BASE                 = "0x11";
	static const string EC_REMOVE_RULE_BASE              = "0x12";

	//nclEdit connector
	static const string EC_ADD_CONNECTOR                 = "0x13";
	static const string EC_REMOVE_CONNECTOR              = "0x14";
	static const string EC_ADD_CONNECTOR_BASE            = "0x15";
	static const string EC_REMOVE_CONNECTOR_BASE         = "0x16";

	//nclEdit descriptor
	static const string EC_ADD_DESCRIPTOR                = "0x17";
	static const string EC_REMOVE_DESCRIPTOR             = "0x18";
	static const string EC_ADD_DESCRIPTOR_SWITCH         = "0x19";
	static const string EC_REMOVE_DESCRIPTOR_SWITCH      = "0x1A";
	static const string EC_ADD_DESCRIPTOR_BASE           = "0x1B";
	static const string EC_REMOVE_DESCRIPTOR_BASE        = "0x1C";

	//nclEdit transition
	static const string EC_ADD_TRANSITION                = "0x1D";
	static const string EC_REMOVE_TRANSITION             = "0x1E";
	static const string EC_ADD_TRANSITION_BASE           = "0x1F";
	static const string EC_REMOVE_TRANSITION_BASE        = "0x20";

	//nclEdit reuse
	static const string EC_ADD_IMPORT_BASE               = "0x21";
	static const string EC_REMOVE_IMPORT_BASE            = "0x22";
	static const string EC_ADD_IMPORTED_DOCUMENT_BASE    = "0x23";
	static const string EC_REMOVE_IMPORTED_DOCUMENT_BASE = "0x24";
	static const string EC_ADD_IMPORT_NCL                = "0x25";
	static const string EC_REMOVE_IMPORT_NCL             = "0x26";

	//nclEdit components
	static const string EC_ADD_NODE                      = "0x27";
	static const string EC_REMOVE_NODE                   = "0x28";

	//nclEdit interface
	static const string EC_ADD_INTERFACE                 = "0x29";
	static const string EC_REMOVE_INTERFACE              = "0x2A";

	//nclEdit link
	static const string EC_ADD_LINK                      = "0x2B";
	static const string EC_REMOVE_LINK                   = "0x2C";

	static const string EC_SET_PROPERTY_VALUE            = "0x2D";

GINGA_PLAYER_END
#endif /*IAPPLICATIONPLAYER_H_*/
