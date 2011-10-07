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

#include "cm/parser/ComponentParser.h"

#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>

typedef struct {
	::br::pucrio::telemidia::ginga::core::cm::IComponentParser* parser;
	::br::pucrio::telemidia::ginga::core::cm::IComponent* currentComponent;
	bool isParsing;
} ComponentAndParser;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace cm {
	ComponentParser::ComponentParser() {
		components = NULL;
		symbols = NULL;
		parentObjects = NULL;

		dependenciesToAdd = NULL;
	}

	ComponentParser::~ComponentParser() {
		releaseMaps();
	}

	void ComponentParser::parse(string xmlDocument) {
		ComponentAndParser* cp;
		int fd;
		int bytes = 1;
		int fileSize;

		releaseMaps();

		fd = open(xmlDocument.c_str(), O_RDONLY|O_LARGEFILE);
		if (fd < 0) {
			clog << "ComponentParser::parse: can't open file:" << xmlDocument;
			clog << endl;
			return;
		}

		fileSize = lseek(fd, 0, SEEK_END);
		if (fileSize < 0) {
			clog << "ComponentParser::parse: file '" << xmlDocument;
			clog << "' is empty" << endl;
			close(fd);
			return;
		}

		close(fd);
		fd = open(xmlDocument.c_str(), O_RDONLY|O_LARGEFILE);
		if (fd < 0) {
			clog << "ComponentParser::parse2: can't open file:" << xmlDocument;
			clog << endl;
			return;
		}

		char content[fileSize];

		components = new map<string, IComponent*>;
		symbols = new map<string, IComponent*>;
		parentObjects = new map<string, set<string>*>;
		dependenciesToAdd = new map<string, set<string>*>;

		XML_Parser parser = XML_ParserCreate(NULL);

		cp = new ComponentAndParser();
		cp->parser = this;
		cp->currentComponent = NULL;
		cp->isParsing = true;

		XML_SetUserData(parser, cp);
		XML_SetElementHandler(
				parser,
				ComponentParser::startElementHandler,
				ComponentParser::stopElementHandler);

		do {
			bytes = read(fd, content, fileSize);
			if (bytes <= 0) {
				break;
			}

			if (XML_Parse(parser, content, bytes, (bytes != fileSize)) ==
					XML_STATUS_ERROR) {

				cerr << "ComponentParser::parse() error '";
				cerr << XML_ErrorString(XML_GetErrorCode(parser));
				cerr << "' at line '" << XML_GetCurrentLineNumber(parser);
				cerr << "'" << endl;
				close(fd);
				delete cp;
				XML_ParserFree(parser);
				return;
			}

		} while (cp->isParsing);

		close(fd);
		delete cp;
		XML_ParserFree(parser);
	}

	void ComponentParser::releaseMaps() {
		if (components != NULL) {
			delete components;
			components = NULL;
		}

		if (symbols != NULL) {
			delete symbols;
			symbols = NULL;
		}
	}

	void ComponentParser::startElementHandler(
			void* data, const XML_Char* element, const XML_Char** attrs) {

		clog << "ComponentParser::startElementHandler element '";
		clog << element << "'" << endl;

		if (strcmp(element, "component") == 0) {
			parseComponent(data, attrs);

		} else if (strcmp(element, "dependency") == 0) {
			parseDependency(data, attrs);

		} else if (strcmp(element, "symbol") == 0) {
			parseSymbol(data, attrs);

		} else if (strcmp(element, "location") == 0) {
			parseLocation(data, attrs);

		} else if (strcmp(element, "repository") == 0) {
			parseRepository(data, attrs);
		}
	}

	void ComponentParser::parseComponent(void* data, const XML_Char** attrs) {
		IComponentParser* cp = ((ComponentAndParser*)data)->parser;
		IComponent* component;
		string name = "";
		string version = "";
		string type = "";

		for (int i = 0; i < 8; i = i + 2) {
			if (strcmp(attrs[i], "package") == 0) {
				//TODO: package control

			} else if (strcmp(attrs[i], "name") == 0) {
					name = attrs[i + 1];

			} else if (strcmp(attrs[i], "version") == 0) {
				version = attrs[i + 1];

			} else if (strcmp(attrs[i], "type") == 0) {
				type = attrs[i + 1];
			}
		}

		void* comp = dlopen("libgingacccmcomponent.so", RTLD_LAZY);
		if (comp == NULL) {
			cerr << "ComponentParser warning: cant load component '";
			cerr << "libgingacccmcomponent.so' => " << dlerror() << endl;
			return;
		}

		dlerror();
		if (comp != NULL) {
			if (name != "" && version != "" && type != "") {
				component = ((ComponentCreator*)(
						dlsym(comp, "createComponent")))(name, version, type);

				const char* dlsym_error = dlerror();

				if (dlsym_error != NULL) {
					cerr << "ComponentManager warning: cant load symbol '";
					cerr << "createComponent' => " << dlsym_error << endl;
					return;
				}

				dlerror();
				clog << "ComponentParser::parseComponent create component '";
				clog << name.c_str() << "' version '";
				clog << version.c_str() << "' type '";
				clog << type.c_str() << "'" << endl;

				((ComponentAndParser*)data)->currentComponent = component;
				((ComponentParser*)cp)->addComponent(name, component);
			}
		}
	}

	void ComponentParser::addComponent(string name, IComponent* component) {
		(*components)[name] = component;
	}

	void ComponentParser::parseDependency(void* data, const XML_Char** attrs) {
		string name = "";
		string version = "";
		map<string, set<string>*>* depsToSolve;
		map<string, IComponent*>::iterator i;
		set<string>* comps;
		IComponent* d = NULL;
		IComponent* c = ((ComponentAndParser*)data)->currentComponent;
		IComponentParser* cp = ((ComponentAndParser*)data)->parser;
		depsToSolve = cp->getUnsolvedDependencies();

		if (c == NULL) {
			return;
		}

		for (int i = 0; i < 4; i = i + 2) {
			if (strcmp(attrs[i], "componentName") == 0) {
				name = attrs[i + 1];

			} else if (strcmp(attrs[i], "version") == 0) {
				version = attrs[i + 1];
			}
		}

		if (name != "" && version != "") {
			d = ((ComponentParser*)cp)->getComponent(name);
			if (d != NULL) {
				c->addDependency(d);

			} else {
				if (depsToSolve->count(c->getName()) != 0) {
					comps = (*depsToSolve)[c->getName()];

				} else {
					comps = new set<string>;
					(*depsToSolve)[c->getName()] = comps;
				}

				comps->insert(name);
			}
			clog << "ComponentParser::parseDependency added '";
			clog << name.c_str();
			clog << "' version '" << version.c_str() << "'" << endl;
		}
	}

	void ComponentParser::parseSymbol(void* data, const XML_Char** attrs) {
		string parent = "";
		string object = "";
		string creator = "";
		string destroyer = "";
		map<string, IComponent*>::iterator i;
		IComponentParser* cp = ((ComponentAndParser*)data)->parser;
		IComponent* c = ((ComponentAndParser*)data)->currentComponent;

		if (c == NULL) {
			return;
		}

		for (int i = 0; i < 8; i = i + 2) {
			if (strcmp(attrs[i], "object") == 0) {
				object = attrs[i + 1];

			} else if (strcmp(attrs[i], "creator") == 0) {
				creator = attrs[i + 1];

			} else if (strcmp(attrs[i], "destroyer") == 0) {
				destroyer = attrs[i + 1];

			} else if (strcmp(attrs[i], "interface") == 0) {
				parent = attrs[i + 1];
			}
		}

		if (object != "" && creator != "") {
			((ComponentParser*)cp)->addObject(object, c);

			if (parent != "" && parent.substr(0, 1) == "I") {
				((ComponentParser*)cp)->addChild(parent, object);
			}

			c->addCreatorSymbol(object, creator);
			c->addDestroyerSymbol(object, destroyer);

			clog << "ComponentParser::parseSymbol added object '";
			clog << object.c_str();
			clog << "' creator '" << creator.c_str() << "' destroyer '";
			clog << destroyer.c_str() << "'" << endl;
		}
	}

	void ComponentParser::addObject(string symbol, IComponent* component) {
		(*symbols)[symbol] = component;
	}

	void ComponentParser::addChild(string parent, string child) {
		set<string>* childs = NULL;

		if (parentObjects->count(parent) != 0) {
			childs = (*parentObjects)[parent];
		}

		if (childs == NULL) {
			childs = new set<string>;
		}

		childs->insert(child);
		(*parentObjects)[parent] = childs;
	}

	void ComponentParser::parseLocation(void* data, const XML_Char** attrs) {
		string type = "";
		string uri = "";
		map<string, IComponent*>::iterator i;
		IComponent* c = ((ComponentAndParser*)data)->currentComponent;

		if (c == NULL) {
			return;
		}

		for (int i = 0; i < 4; i = i + 2) {
			if (strcmp(attrs[i], "type") == 0) {
				type = attrs[i + 1];

			} else if (strcmp(attrs[i], "uri") == 0) {
				uri = attrs[i + 1];
			}
		}

		if (uri != "") {
			c->setLocation(uri, type);

			clog << "ComponentParser::parseLocation added uri '";
			clog << uri.c_str();
			clog << "' type '" << type.c_str() << "'" << endl;
		}
	}

	void ComponentParser::parseRepository(void* data, const XML_Char** attrs) {
		string uri = "";
		IComponent* c = ((ComponentAndParser*)data)->currentComponent;

		if (c == NULL) {
			return;
		}

		for (int i = 0; i < 2; i = i + 2) {
			if (strcmp(attrs[i], "uri") == 0) {
				uri = attrs[i + 1];
			}
		}

		if (uri != "") {
			c->addUri(uri);

			clog << "ComponentParser::parseRepository added uri '";
			clog << uri.c_str();
			clog << "'" << endl;
		}
	}

	void ComponentParser::stopElementHandler(
			void* data, const XML_Char* element) {

		IComponentParser* cp = ((ComponentAndParser*)data)->parser;
		clog << "ComponentParser::stopElementHandler element '";
		clog << element << "'" << endl;

		if (strcmp(element, "middleware") == 0) {
			((ComponentAndParser*)data)->isParsing = false;
			cp->solveDependencies();
		}
	}

	IComponent* ComponentParser::getComponent(string name) {
		if (components->count(name) != 0) {
			return (*components)[name];
		}

		return NULL;
	}

	map<string, IComponent*>* ComponentParser::getSymbols() {
		map<string, IComponent*>* symbs;

		symbs = new map<string, IComponent*>(*symbols);
		return symbs;
	}

	map<string, IComponent*>* ComponentParser::getComponents() {
		map<string, IComponent*>* comps;

		comps = new map<string, IComponent*>(*components);
		return comps;
	}

	map<string, set<string>*>* ComponentParser::getParentObjects() {
		return parentObjects;
	}

	map<string, set<string>*>* ComponentParser::getUnsolvedDependencies() {
		return dependenciesToAdd;
	}

	void ComponentParser::solveDependencies() {
		map<string, set<string>*>::iterator i;
		set<string>::iterator j;

		set<string>* deps;
		IComponent* c, *d;
		string name;

		i = dependenciesToAdd->begin();
		while (i != dependenciesToAdd->end()) {
			name = i->first;
			if (components->count(name) != 0) {
				c = (*components)[name];
				deps = i->second;
				j = deps->begin();
				while (j != deps->end()) {
					if (components->count(*j) != 0) {
						d = (*components)[*j];
						c->addDependency(d);
					}
					++j;
				}
			}
			++i;
		}
		dependenciesToAdd->clear();
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::cm::IComponentParser*
		createComponentParser() {

	return new ::br::pucrio::telemidia::ginga::core::cm::ComponentParser();
}

extern "C" void destroyComponentParser(
		::br::pucrio::telemidia::ginga::core::cm::IComponentParser* cp) {

	delete cp;
}
