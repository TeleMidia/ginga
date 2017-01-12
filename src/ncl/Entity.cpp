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

#include "ncl/Entity.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {

	set<Entity*> Entity::instances;
	pthread_mutex_t Entity::iMutex;
	bool Entity::initMutex = false;

	Entity::Entity(string id) {
		this->id = id;
		typeSet.insert("Entity");

		if (!initMutex) {
			initMutex = true;
			pthread_mutex_init(&iMutex, NULL);
		}

		pthread_mutex_lock(&iMutex);
		instances.insert(this);
		pthread_mutex_unlock(&iMutex);
	}

	Entity::~Entity() {
		set<Entity*>::iterator i;

		pthread_mutex_lock(&iMutex);
		i = instances.find(this);
		if (i != instances.end()) {
			instances.erase(i);
		}
		pthread_mutex_unlock(&iMutex);
	}

	bool Entity::hasInstance(Entity* instance, bool eraseFromList) {
		set<Entity*>::iterator i;
		bool hasEntity = false;

		if (!initMutex) {
			return false;
		}

		pthread_mutex_lock(&iMutex);
		i = instances.find(instance);
		if (i != instances.end()) {
			if (eraseFromList) {
				instances.erase(i);
			}
			hasEntity = true;
		}
		pthread_mutex_unlock(&iMutex);

		return hasEntity;
	}

	void Entity::printHierarchy() {
		set<string>::iterator i;

		i = typeSet.begin();
		while (i != typeSet.end()) {
			clog << *i << " ";
			++i;
		}
	}

	bool Entity::instanceOf(string s) {
		if (!typeSet.empty()) {
			/*
			set<string>::iterator it;
			clog << "Entity instanceOf for '" << s << ";
			clog << "' with the following set:" << endl;
			for(it = typeSet.begin(); it != typeSet.end(); ++it) {
				clog << "[" << *it << "] ";
			}
			clog << ((typeSet.find(s) != typeSet.end()) ? "true" : "false");
			clog << endl;
			 */

			return ( typeSet.find(s) != typeSet.end() );

		} else {
			/*
			clog << "Entity instanceOf for " << s << " has an empty set";
			clog << endl;
			*/
			return false;
		}
	}

	int Entity::compareTo(Entity* otherEntity) {
		string otherId;
		int cmp;

		otherId = (static_cast<Entity*>(otherEntity))->getId();

		if (id == "")
			return -1;

		if (otherId == "")
			return 1;

		cmp = id.compare(otherId);
		switch (cmp) {
			case 0 :
				return 0;
			default :
				if (cmp < 0)
					return -1;
				else
					return 1;
		}
	}

	/*bool Entity::equals(Entity* otherEntity) {
		string otherId;

		otherId = (static_cast<Entity*>(otherEntity))->getId();
		return (id.compare(otherId) == 0);
	}*/

	string Entity::getId() {
		return id;
	}

	void Entity::setId(string someId) {
		id = someId;
	}

	string Entity::toString() {
		return id;
	}

	Entity *Entity::getDataEntity() {
		return this;
	}
}
}
}
}
