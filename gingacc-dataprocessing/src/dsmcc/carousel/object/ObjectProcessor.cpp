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

#include "dataprocessing/dsmcc/carousel/object/ObjectProcessor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	ObjectProcessor::ObjectProcessor(unsigned short pid) {
		this->pid = pid;
	}

	ObjectProcessor::~ObjectProcessor() {
		map<string, Object*>::iterator i;

		i = objects.begin();
		while (i != objects.end()) {
			delete i->second;
			++i;
		}

		objects.clear();

		objectNames.clear();
		objectPaths.clear();
		listeners.clear();
	}

	void ObjectProcessor::setObjectsListeners(set<IObjectListener*>* l) {
		listeners.clear();
		listeners.insert(l->begin(), l->end());
	}

	void ObjectProcessor::pushObject(Object* object) {
		map<string, Object*>::iterator i;

		objects[object->getObjectId()] = object;

		i = objects.begin();
		while (i != objects.end()) {
			object = i->second;

			if (mountObject(object)) {
				clog << "ObjectProcessor::pushObject call notifyLists" << endl;
				notifyObjectListeners(object);

				objects.erase(i);
				i = objects.begin();

				delete object;
				object = NULL;

			} else {
				++i;
			}
		}
	}

	bool ObjectProcessor::hasObjects() {
		if (objects.empty()) {
			return false;
		}

		return true;
	}

	map<string, string>* ObjectProcessor::getSDNames() {
		return &objectNames;
	}

	map<string, string>* ObjectProcessor::getSDPaths() {
		return &objectPaths;
	}

	bool ObjectProcessor::mountObject(Object* object) {
		vector<Binding*>* bindings;
		vector<Binding*>::iterator i;
		string objectId, path, strToken;
		FILE* fd;
		char* data;
		char token[6];
		unsigned int j, size;

		/*clog << "ObjectProcessor::mountObject of kind '" << object->getKind();
		clog << "'" << endl;*/
		if (object->getKind() == "srg" ||
			    object->getKind() == "DSM::ServiceGateway") {

			bindings = object->getBindings();
			for (i = bindings->begin(); i != bindings->end(); ++i) {
				objectId = itos((*i)->getIor()->getCarouselId()) +
					    itos((*i)->getIor()->getModuleId()) +
					    itos((*i)->getIor()->getObjectKey());

				/*clog << "ObjectProcessor::mountObject srg adding objId '";
				clog << objectId << "'" << endl;*/
				objectNames[objectId] = (*i)->getId();
				objectPaths[objectId] = SystemCompat::getTemporaryDir() + "ginga" +
										SystemCompat::getIUriD() + "carousel" +
										SystemCompat::getIUriD() + itos(pid) + "." +
										itos(object->getCarouselId()) +
										SystemCompat::getIUriD();
			}

			//clog << "ObjectProcessor::mountObject srg done" << endl;
			return true;

		} else if (object->getKind() == "dir" ||
			    object->getKind() == "DSM::Directory") {

			if (objectPaths.count(object->getObjectId()) == 0) {
				/*clog << "ObjectProcessor::mountObject Warning!";
				clog << "cant find object id '" << object->getObjectId();
				clog << endl;*/
				return false;

			} else {
				path = (objectPaths.find(object->getObjectId()))->second +
					    (objectNames.find(object->getObjectId()))->second +
					    SystemCompat::getIUriD();

				/*clog << "ObjectProcessor::mountObject create dir '" << path;
				clog << endl;*/
				SystemCompat::makeDir(path.c_str(), 0777);
			}

			bindings = object->getBindings();
			for (i = bindings->begin(); i != bindings->end(); ++i) {
				objectId = itos((*i)->getIor()->getCarouselId()) +
					    itos((*i)->getIor()->getModuleId()) +
					    itos((*i)->getIor()->getObjectKey());

				/*clog << "ObjectProcessor::mountObject dir adding objId '";
				clog << objectId << "'" << endl;*/

				objectNames[objectId] = (*i)->getId();
				objectPaths[objectId] = path;
			}
			//clog << "ObjectProcessor::mountObject dir done" << endl;
			return true;

		} else if (object->getKind() == "fil" ||
			    object->getKind() == "DSM::File") {

			if (objectPaths.count(object->getObjectId()) == 0) {
				clog << "ObjectProcessor::mountObject Warning! ";
				clog << "cant find object id '" << object->getObjectId();
				clog << "" << endl;
				return false;

			} else {
				path = (objectPaths.find(object->getObjectId()))->second +
					    (objectNames.find(object->getObjectId()))->second;

				fd = fopen(path.c_str(), "w+b");
				size = object->getDataSize();
				if (fd > 0) {
					//TODO: correct BUG in content provider
					if (path.find(".ncl") != std::string::npos) {
						clog << "ObjectProcessor::mount NCL FILE" << endl;
						data = object->getData();
						j = 0;
						while (j < size) {
							memcpy((void*)&(token[0]), (void*)&(data[j]), 6);
							strToken = (string)(char*)token;
							if (strToken.find("</ncl>") != std::string::npos) {
								size = j + 6;
								break;
							}
							j++;
						}
					}

					fwrite((void*)(object->getData()), 1, size, fd);
					fclose(fd);

					clog << "ObjectProcessor::mountObject create fil '";
					clog << path << "'" << endl;

				} else {
					clog << "Warning! Cannot mount ";
					clog << path.c_str() << endl;
					return false;
				}
			}
			//clog << "ObjectProcessor::mountObject fil done" << endl;
			return true;
		}

		clog << "ObjectProcessor::mountObject Warning! unrecognized type";
		clog << endl;
		return false;
	}


	void ObjectProcessor::notifyObjectListeners(Object* obj) {
		set<IObjectListener*>::iterator i;
		string clientUri = "";
		string name = "";
		string objectId;

		objectId = obj->getObjectId();
		if (objectPaths.count(objectId) != 0) {
			clientUri = objectPaths[objectId];
		}

		if (objectNames.count(objectId) != 0) {
			name = objectNames[objectId];
		}

		i = listeners.begin();
		while (i != listeners.end()) {
			clog << "ObjectProcessor::notifyListeners call objectmounted";
			clog << " for '" << objectId << "'" << endl;
			(*i)->objectMounted(objectId, clientUri, name);
			++i;
		}
	}
}
}
}
}
}
}
}
