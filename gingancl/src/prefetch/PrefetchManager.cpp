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

#include "gingancl/prefetch/PrefetchManager.h"

#include "config.h"

#if HAVE_COMPONENTS
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#if HAVE_IC
#include "ic/InteractiveChannelManager.h"
#endif //HAVE_IC
#endif

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <sys/stat.h>
#include <stdio.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace prefetch {
#if HAVE_COMPONENTS
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	PrefetchManager::PrefetchManager() {
		string iurid = SystemCompat::getIUriD();
		icm          = NULL;

#if HAVE_IC
#if HAVE_COMPONENTS
		icm = ((ICMCreator*)(cm->getObject("InteractiveChannelManager")))();
#else
		icm = InteractiveChannelManager::getInstance();
#endif //HAVE_COMPONENTS
#endif //HAVE_IC

		scheduledRemoteUris = NULL;
		scheduledLocalUris  = NULL;
		localToRemoteUris   = new map<string, string>;
		urisToLocalRoots    = new map<string, string>;
		synch               = true;
		kbytes              = 0;
		filesDown           = 0;
		filesSched          = 0;
		prefetchRoot        = SystemCompat::getTemporaryDir() +
				iurid + "tmp" + iurid + "prefetch";

		createDirectory(prefetchRoot);
	}

	PrefetchManager::~PrefetchManager() {
		if (localToRemoteUris != NULL) {
			delete localToRemoteUris;
			localToRemoteUris = NULL;
		}

		if (urisToLocalRoots != NULL) {
			delete urisToLocalRoots;
			urisToLocalRoots = NULL;
		}
	}

	IPrefetchManager* PrefetchManager::_instance = NULL;

	IPrefetchManager* PrefetchManager::getInstance() {
		if (_instance == NULL) {
			return new PrefetchManager();
		}

		return _instance;
	}

	void PrefetchManager::createDirectory(string newDir) {
		vector<string>* dirs;
		vector<string>::iterator i;
		string dir = "";

		dirs = split(newDir, SystemCompat::getIUriD());
		if (dirs == NULL) {
			return;
		}

		i = dirs->begin();
		while (i != dirs->end()) {
			dir = dir + SystemCompat::getIUriD() + *i;
			SystemCompat::makeDir(dir.c_str(), 0755);
			//clog << "PrefetchManager::createDirectory '";
			//clog << dir << "'" << endl;
			++i;
		}

		delete dirs;
	}

	void PrefetchManager::release() {
		delete _instance;
		_instance = NULL;
	}

	void PrefetchManager::releaseContents() {
		map<string, string>::iterator i;
		string resource;

		if (localToRemoteUris != NULL) {
			i = localToRemoteUris->begin();
			while (i != localToRemoteUris->end()) {
				resource = i->first;
				clog << "PrefetchManager::releaseContents trying to remove '";
				clog << resource << "'" << endl;
				remove((char*)(resource.c_str()));
				++i;
			}
			localToRemoteUris->clear();
			urisToLocalRoots->clear();
		}

		if (icm != NULL) {
#if HAVE_IC
			icm->clearInteractiveChannelManager();
#endif //HAVE_IC
		}
	}

	string PrefetchManager::createDocumentPrefetcher(string remoteDocUri) {
		string localUri = "";

		if (urisToLocalRoots->count(remoteDocUri) != 0) {
			localUri = (*urisToLocalRoots)[remoteDocUri];
			localUri = localUri + remoteDocUri.substr(
					remoteDocUri.find_last_of(SystemCompat::getIUriD()),
					remoteDocUri.length() - remoteDocUri.find_last_of(SystemCompat::getIUriD()));

			return localUri;
		}

		localUri = prefetchRoot + remoteDocUri.substr(
				remoteDocUri.find_last_of(SystemCompat::getIUriD()), (
						remoteDocUri.find_last_of(".") -
						remoteDocUri.find_last_of(SystemCompat::getIUriD())));

		localUri = SystemCompat::updatePath(localUri);
		(*urisToLocalRoots)[remoteDocUri] = localUri;
		createDirectory(localUri);

		//clog << "PrefetchManager::createDocumentPrefetcher root='";
		//clog << localUri;

		localUri = localUri + remoteDocUri.substr(
				remoteDocUri.find_last_of(SystemCompat::getIUriD()),
				remoteDocUri.length() - remoteDocUri.find_last_of(SystemCompat::getIUriD()));

		localUri = SystemCompat::updatePath(localUri);

		//clog << "', URI='" << localUri << "' remoteUri = '";
		//clog << remoteDocUri;
		//clog << "'" << endl;

		getContent(remoteDocUri, localUri);

		(*localToRemoteUris)[localUri] = remoteDocUri;
		return localUri;
	}

	string PrefetchManager::createSourcePrefetcher(
			string localDocUri, string src) {

		string remoteDocUri, newSrc, localRoot;
		string remoteRoot, relativeSrc, remoteUri;

		bool prefetch = false;
		bool isRemoteDoc = hasRemoteLocation(localDocUri);
		relativeSrc = "";

		//remoteDoc => create local src relative to local document uri created
		if (isRemoteDoc) {
			remoteDocUri = getRemoteLocation(localDocUri);
			remoteRoot = remoteDocUri.substr(0, remoteDocUri.find_last_of(SystemCompat::getIUriD()));
			localRoot = getLocalRoot(remoteDocUri);

		} else {//document is local => create local src relative to localDoc
			localRoot = localDocUri.substr(0, localDocUri.find_last_of(SystemCompat::getIUriD()));
		}

		if (src.substr(0, 7) == "http://") {
			if (isRemoteDoc) {
				if (src.find(remoteRoot) == std::string::npos) {
					relativeSrc = src.substr(
							src.find_last_of(SystemCompat::getIUriD()) + 1,
							src.length() - (src.find_last_of(SystemCompat::getIUriD()) + 1));

				} else {
					relativeSrc = src.substr(
							remoteRoot.length() + 1,
							src.length() - (remoteRoot.length() + 1));
				}

			} else {
				relativeSrc = src.substr(
						src.find_last_of(SystemCompat::getIUriD()) + 1,
						src.length() - (src.find_last_of(SystemCompat::getIUriD()) + 1));
			}

			remoteUri = src;
			prefetch = true;

		} else {
			//if document is remote, translate local src to remote
			if (src.find(SystemCompat::getIUriD()) != std::string::npos) {
				src = SystemCompat::updatePath(src);
			}
			if (isRemoteDoc) {
				if (src.find(localRoot) == std::string::npos) {
					if (src.find(SystemCompat::getIUriD()) != std::string::npos) {
						relativeSrc = src.substr(
								src.find_last_of(SystemCompat::getIUriD()) + 1,
								src.length() - (src.find_last_of(SystemCompat::getIUriD()) + 1));
					} else {
						relativeSrc = src;
					}

				} else {
					relativeSrc = src.substr(
							localRoot.length() + 1,
							src.length() - (localRoot.length() + 1));
				}

				remoteUri = remoteRoot + SystemCompat::getIUriD() + relativeSrc;
				prefetch = true;
			}
		}

		if (prefetch) {
			newSrc = localRoot + SystemCompat::getIUriD() + relativeSrc;
			newSrc = SystemCompat::updatePath(newSrc);
			createDirectory(newSrc.substr(0, newSrc.find_last_of(SystemCompat::getIUriD())));

			//clog << "PrefetchManager::createSourcePrefetcher: remoteUri '";
			//clog << remoteUri << "' oldSrc '" << src << "' newSrc '";
			//clog << newSrc << "'" << endl;

			scheduleContent(remoteUri, newSrc);

			(*localToRemoteUris)[newSrc] = remoteUri;
			return newSrc;
		}

		return src;
	}

	bool PrefetchManager::hasIChannel() {
#if HAVE_IC
		if (icm != NULL) {
			return icm->hasInteractiveChannel();
		}
#endif
		return false;
	}

	bool PrefetchManager::hasRemoteLocation(string docUri) {
		if (localToRemoteUris->count(docUri) == 0) {
			return false;
		}
		return true;
	}

	string PrefetchManager::getRemoteLocation(string docUri) {
		if (hasRemoteLocation(docUri)) {
			return (*localToRemoteUris)[docUri];
		}
		return "";
	}

	string PrefetchManager::getLocalRoot(string remoteUri) {
		if (urisToLocalRoots->count(remoteUri) != 0) {
			return (*urisToLocalRoots)[remoteUri];
		}
		return "";
	}

	void PrefetchManager::setSynchPrefetch(bool synch) {
		this->synch = synch;
	}

	void PrefetchManager::getContent(string remoteUri, string localUri) {
		if (icm != NULL) {
#if HAVE_IC
			IInteractiveChannel* ic = icm->createInteractiveChannel(
					remoteUri);

			ic->setSourceTarget(localUri);
			ic->reserveUrl(remoteUri, this, "GingaNCL/0.13.6");
			ic->performUrl();

			icm->releaseInteractiveChannel(ic);
#endif
		}
	}

	void PrefetchManager::scheduleContent(string remoteUri, string localUri) {
		if (scheduledLocalUris == NULL) {
			scheduledLocalUris = new set<string>;
		}

		if (scheduledRemoteUris == NULL) {
			scheduledRemoteUris = new set<string>;
		}

		scheduledRemoteUris->insert(remoteUri);
		scheduledLocalUris->insert(localUri);
	}

	void PrefetchManager::receiveDataPipe(FILE* fd, int size) {
		string txt = "", nFiles;

		if (size == 0) {
			return;
		}

		kbytes = kbytes + (size / 1024);

		if (filesSched == 0) {
			txt = "Searching files.";

		} else {
			if (filesSched < 2) {
				nFiles = " file.";

			} else {
				nFiles = " files.";
			}

			txt = "Downloaded (" + itos(kbytes) + " KB): " + itos(filesDown) +
					" of " + itos(filesSched) + nFiles;
		}
	}

	void PrefetchManager::getScheduledContent(string clientUri) {
		set<string>::iterator i, j;
		string remoteUri = "";

		if (scheduledLocalUris != NULL && scheduledRemoteUris != NULL) {
			if (localToRemoteUris->count(clientUri) != 0) {
				remoteUri = (*localToRemoteUris)[clientUri];
			}

			i = scheduledLocalUris->find(clientUri);
			j = scheduledRemoteUris->find(remoteUri);

			if (i != scheduledLocalUris->end() &&
					j != scheduledRemoteUris->end()) {

				scheduledLocalUris->erase(i);
				scheduledRemoteUris->erase(j);
				getContent(remoteUri, clientUri);

			} else {
				clog << "PrefetchManager::getScheduledContent Warning! ";
				clog << "cant find client '" << clientUri << "' and '";
				clog << remoteUri << "' in scheduled tasks" << endl;
			}

		} else {
			clog << "PrefetchManager::getScheduledContent Warning! ";
			clog << "uris conainter are NULL" << endl;
		}
	}

	void PrefetchManager::getScheduledContents() {
		set<string>::iterator i, j;

		if (scheduledLocalUris != NULL && scheduledRemoteUris != NULL) {
			i = scheduledLocalUris->begin();
			j = scheduledRemoteUris->begin();
			filesSched = scheduledLocalUris->size();
			kbytes = 0;
			filesDown = 0;
			while (i != scheduledLocalUris->end()) {
				getContent(*j, *i);
				++i;
				++j;
				filesDown++;
			}
			receiveDataPipe(NULL, 1); //refresh last file downloaded
		}
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::ncl::prefetch::IPrefetchManager*
         createPrefetchManager() {

	return (::br::pucrio::telemidia::ginga::ncl::prefetch::
			PrefetchManager::getInstance());
}

extern "C" void destroyPrefetchManager(
		::br::pucrio::telemidia::ginga::ncl::prefetch::IPrefetchManager* pm) {

	pm->release();
}
