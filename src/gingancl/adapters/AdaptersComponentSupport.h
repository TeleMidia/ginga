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

#ifndef ADAPTERSCOMPONENTSUPPORT_H_
#define ADAPTERSCOMPONENTSUPPORT_H_

#include "config.h"

#include "mb/IInputManager.h"
#include "mb/LocalScreenManager.h"

#include "player/ProgramAV.h"
#include "player/ChannelPlayer.h"
#include "player/AVPlayer.h"
#ifndef __ANDROID__
#include "player/LuaPlayer.h"
#endif
#include "player/ImagePlayer.h"
#include "player/PlainTxtPlayer.h"
#include "player/SrtPlayer.h"
#include "player/NTPPlayer.h"

#include "AVPlayerAdapter.h"
#include "ChannelPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::av;

#include "ProgramAVPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::av::tv;

#include "ImagePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::image;

#include "MirrorPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::mirror;

#include "PlainTxtPlayerAdapter.h"
#include "SubtitlePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::text;

#include "TimePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::time;

#if HAVE_BERKELIUM
#include "player/BerkeliumPlayer.h"
#include "application/declarative/xhtml/berkelium/BerkeliumPlayerAdapter.h"
using namespace br::pucrio::telemidia::ginga::ncl::adapters::application::xhtml;
#endif

#if HAVE_ESPEAK
#include "player/SsmlPlayer.h"
#include "SsmlPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::tts;
#endif

#include "LuaPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application::lua;

#include "NCLPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application::ncl;

#include "FormatterPlayerAdapter.h"

#include "gingancl/FormatterMediator.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#endif /* ADAPTERSCOMPONENTSUPPORT_H_ */
