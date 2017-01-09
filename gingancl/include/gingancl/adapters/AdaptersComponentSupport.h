/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

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

#include "av/AVPlayerAdapter.h"
#include "av/ChannelPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::av;

#include "av/tv/ProgramAVPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::av::tv;

#include "image/ImagePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::image;

#include "mirror/MirrorPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::mirror;

#include "text/PlainTxtPlayerAdapter.h"
#include "text/SubtitlePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::text;

#ifndef __ANDROID__
#include "application/imperative/lua/LuaPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application::lua;
#endif

#include "time/TimePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::time;

#if HAVE_BERKELIUM
#include "player/BerkeliumPlayer.h"

#include "application/declarative/xhtml/berkelium/BerkeliumPlayerAdapter.h"
using namespace br::pucrio::telemidia::ginga::ncl::adapters::application::xhtml;
#endif

#if HAVE_SSML
#include "player/SsmlPlayer.h"

#include "tts/SsmlPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::tts;
#endif

#include "application/declarative/ncl/NCLPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application::ncl;

#include "FormatterPlayerAdapter.h"

#include "gingancl/FormatterMediator.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
}
}
}
}
}
}

#endif /* ADAPTERSCOMPONENTSUPPORT_H_ */
