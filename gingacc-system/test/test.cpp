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

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "config.h"

#include "system/io/LocalDeviceManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#if HAVE_XINEPROVIDER
#include "system/io/interface/content/video/XineVideoProvider.h"
#endif

#if HAVE_FFMPEGPROVIDER
#include "system/io/interface/content/video/FFmpegVideoProvider.h"
#endif

#include "system/io/interface/input/CodeMap.h"

#include "system/io/interface/output/ISurface.h"
#include "system/io/interface/output/dfb/DFBWindow.h"
#include "system/io/interface/content/image/DFBImageProvider.h"

#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	IImageProvider* img;
	IWindow* w;
	ISurface* s;

#if HAVE_XINEPROVIDER
	XineVideoProvider* xineProvider;
#endif

#if HAVE_FFMPEGPROVIDER
	FFmpegVideoProvider* ffmpegProvider;
#endif

	setLogToNullDev();
	LocalDeviceManager::getInstance()->createDevice("systemScreen(0)");
	w = new DFBWindow(10, 10, 300, 300);
	w->draw();

	if (argc == 1) {
		img = new DFBImageProvider("/root/bg_initializing.png");

		s = img->prepare(false);

		w->renderFrom(s);
		w->show();

		clog << CodeMap::KEY_ASTERISK << endl;
		clog << "System test has shown image. press enter to continue" << endl;

	} else if (argc == 3) {
		if (strcmp(argv[1], "xine") == 0) {
			clog << "System test will test xine video provider" << endl;
#if HAVE_XINEPROVIDER
			xineProvider = new XineVideoProvider(argv[2]);
			s = xineProvider->getPerfectSurface();
			s->setParent(w);
			xineProvider->playOver(s, true);
			xineProvider->feedBuffers();
			xineProvider->checkVideoResizeEvent(s);
			xineProvider->setSoundLevel(100);
			xineProvider->playOver(s, true);
			w->show();
#endif
		} else if (strcmp(argv[1], "ffmpeg") == 0) {
			clog << "System test will test ffmpeg video provider" << endl;
#if HAVE_FFMPEGPROVIDER
			ffmpegProvider = new FFmpegVideoProvider(argv[2]);
			s = ffmpegProvider->getPerfectSurface();
			s->setParent(w);
			w->show();
			ffmpegProvider->feedBuffers();
			ffmpegProvider->checkVideoResizeEvent(s);
			ffmpegProvider->setSoundLevel(100);
			ffmpegProvider->playOver(s, true);
#endif
		}
	}

	//TODO: tests

	getchar();
	return 0;
}
