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

		if (object->getKind() == "srg" ||
			    object->getKind() == "DSM::ServiceGateway") {

			bindings = object->getBindings();
			for (i = bindings->begin(); i != bindings->end(); ++i) {
				objectId = itos((*i)->getIor()->getCarouselId()) +
					    itos((*i)->getIor()->getModuleId()) +
					    itos((*i)->getIor()->getObjectKey());

				objectNames[objectId] = (*i)->getId();
				objectPaths[objectId] = SystemCompat::getTemporaryDir() + "ginga" +
										SystemCompat::getIUriD() + "carousel" +
										SystemCompat::getIUriD() + itos(pid) + "." +
										itos(object->getCarouselId()) +
										SystemCompat::getIUriD();
			}

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

				SystemCompat::makeDir(path.c_str(), 0777);
			}

			bindings = object->getBindings();
			for (i = bindings->begin(); i != bindings->end(); ++i) {
				objectId = itos((*i)->getIor()->getCarouselId()) +
					    itos((*i)->getIor()->getModuleId()) +
					    itos((*i)->getIor()->getObjectKey());

				objectNames[objectId] = (*i)->getId();
				objectPaths[objectId] = path;
			}
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

				} else {
					clog << "Warning! Cannot mount ";
					clog << path.c_str() << endl;
					return false;
				}
			}
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
