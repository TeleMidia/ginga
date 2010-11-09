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

#include "player/ImagePlayer.h"

#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	ImagePlayer::ImagePlayer(string mrl) : Player(mrl) {
		io::IGingaLocatorFactory* glf = NULL;
		string path, name, clientPath, newMrl;
		bool resolved = false;

		if (fileExists(mrl)) {
#if HAVE_COMPSUPPORT
			provider = ((ImageProviderCreator*)(cm->getObject(
					"ImageProvider")))(mrl.c_str());
#else
			provider = new DFBImageProvider(mrl.c_str());
#endif
		} else {
			if (!fileExists(mrl) && !isAbsolutePath(mrl)) {
				newMrl = getDocumentPath() + mrl;
				if (fileExists(newMrl)) {
					resolved = true;
					mrl = newMrl;
					newMrl = "";
				}
			}

			if (!resolved) {
				if (mrl.find("/") != std::string::npos) {
					path = mrl.substr(0, mrl.find_last_of("/"));
					name = mrl.substr(
							mrl.find_last_of("/") + 1,
							mrl.length() - mrl.find_last_of("/"));

				} else if (mrl.find("\\") != std::string::npos) {
					path = mrl.substr(0, mrl.find_last_of("\\"));
					name = mrl.substr(
							mrl.find_last_of("\\") + 1,
							mrl.length() - mrl.find_last_of("\\"));
				}

#if HAVE_COMPSUPPORT
				glf = ((GingaLocatorFactoryCreator*)(cm->getObject(
						"GingaLocatorFactory")))();
#else
				glf = GingaLocatorFactory::getInstance();
#endif

				if (glf != NULL) {
					cout << "ImagePlayer trying to find '" << mrl << "'";
					cout << endl;
					clientPath = glf->getLocation(path);
					newMrl = clientPath + name;
					cout << "ImagePlayer found newMrl = '" << newMrl;
					cout << "'" << endl;

					if (fileExists(newMrl)) {
#if HAVE_COMPSUPPORT
						provider = ((ImageProviderCreator*)(cm->getObject(
								"ImageProvider")))(newMrl.c_str());
#else
						provider = new DFBImageProvider(newMrl.c_str());
#endif
					} else {
						provider = NULL;
						cout << "ImagePlayer::ImagePlayer Warning! File ";
						cout << " Not Found: '" << newMrl.c_str();
						cout << "'" << endl;
					}

				} else {
					provider = NULL;
					cout << "ImagePlayer::ImagePlayer Warning! ";
					cout << "GLF Component ";
					cout << " Found!" << endl;
				}
			}
		}

		if (provider != NULL) {
			surface = ImagePlayer::prepareSurface(provider, mrl);
		}
	}

	ImagePlayer::~ImagePlayer() {
		if (provider != NULL) {
			delete provider;
			provider = NULL;

#if HAVE_COMPSUPPORT
			cm->releaseComponentFromObject("ImageProvider");
#endif
		}
	}

	void ImagePlayer::play() {
		if (provider == NULL ||
				surface == NULL || surface->getContent() == NULL) {

			return;
		}

		provider->playOver(surface);
		Player::play();
	}

	void ImagePlayer::stop() {
		Player::stop();
	}

	void ImagePlayer::resume() {
		ImagePlayer::play();
	}

	void ImagePlayer::setPropertyValue(string name, string value) {

		//TODO: set brightness, rotate...
		//refresh changes
		IWindow* win;
		win = (IWindow*)(surface->getParent());
		if (win != NULL) {
			win->renderFrom(surface);
		}

		Player::setPropertyValue(name, value);
	}

	ISurface* ImagePlayer::prepareSurface(
			IImageProvider* provider, string mrl) {

		ISurface* renderedSurface = NULL;

		if (mrl.length() > 4 && mrl.substr(mrl.length() - 4, 4) == ".gif") {
			renderedSurface = provider->prepare(true);

		} else {
			renderedSurface = provider->prepare(false);
		}

		return renderedSurface;
	}

	ISurface* ImagePlayer::renderImage(string mrl) {
		IImageProvider* imgProvider = NULL;
		ISurface* renderedSurface = NULL;
		string newMrl;

		if (fileExists(mrl)) {
#if HAVE_COMPSUPPORT
			imgProvider = ((ImageProviderCreator*)(cm->getObject(
					"ImageProvider")))(mrl.c_str());
#else
			imgProvider = new DFBImageProvider(mrl.c_str());
#endif
		} else {
			if (!isAbsolutePath(mrl)) {
				newMrl = updatePath(getDocumentPath() + "/" + mrl);
				if (fileExists(newMrl)) {
#if HAVE_COMPSUPPORT
					imgProvider = ((ImageProviderCreator*)(cm->getObject(
							"ImageProvider")))(newMrl.c_str());
#else
					imgProvider = new DFBImageProvider(newMrl.c_str());
#endif
				} else {
					cout << "ImagePlayer::renderImage Warning! Can't render '";
					cout << mrl << "': file not found '" << mrl << "'";
					cout << " neither '" << newMrl << "'" << endl;
				}

			} else {
				cout << "ImagePlayer::renderImage Warning! Can't render '";
				cout << mrl << "', file not found!" << endl;
			}
		}

		if (imgProvider != NULL) {
			renderedSurface = ImagePlayer::prepareSurface(imgProvider, mrl);
			delete imgProvider;
			imgProvider = NULL;

#if HAVE_COMPSUPPORT
			cm->releaseComponentFromObject("ImageProvider");
#endif
		}

		return renderedSurface;
	}
}
}
}
}
}
}

extern "C" ISurface* prepareSurface(io::IImageProvider* provider, string mrl) {
	return (::br::pucrio::telemidia::ginga::core::player::
			ImagePlayer::prepareSurface(provider, mrl));
}

extern "C" ISurface* renderImage(string mrl) {
	return (::br::pucrio::telemidia::ginga::core::player::
			ImagePlayer::renderImage(mrl));
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createImagePlayer(const char* mrl) {

	return new ::br::pucrio::telemidia::ginga::core::player::ImagePlayer(mrl);
}

extern "C" void destroyImagePlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
