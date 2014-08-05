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

#include "converter/framework/DocumentParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
	DocumentParser::DocumentParser() {
		genericTable = new map<string, void*>;
		//initialize();
		//setDependencies();
		initializeUserCurrentPath();
		documentTree = NULL;

		iUriD = "";
		fUriD = "";
	}

	DocumentParser::~DocumentParser() {
		map<string, void*>* table;
		map<string, void*>::iterator i;

		if (genericTable != NULL) {
			i = genericTable->begin();
			while (i != genericTable->end()) {
				table = (map<string, void*>*)(i->second);
				/*
				TODO: fix it
				j = table->begin();
				while (j != table->end()) {
					delete *j;
					++j;
				}*/

				delete table;
				table = NULL;
				++i;
			}

			delete genericTable;
			genericTable = NULL;
		}

		if (documentTree != NULL) {
			delete documentTree;
			documentTree = NULL;
		}
	}

	void* DocumentParser::parse(string uri, string iUriD, string fUriD) {
		DOMElement* rootElement;

		this->iUriD = iUriD;
		this->fUriD = fUriD;

		if (!isXmlStr(uri)) {
			string::size_type pos;
			while (true) {
				pos = uri.find_first_of(fUriD);
				if (pos == string::npos)
					break;
				uri.replace(pos,1,iUriD);
			}

			uri = absoluteFile(getDocumentPath(), uri);
		}

		documentTree = (DOMDocument*)XMLParsing::parse(uri);

		if (documentTree == NULL) {
			clog << "DocumentParser::parse ";
			clog << "Error when parsing '" << uri.c_str();
			clog << "' (file not found)" << endl;
			return NULL;
		}

		//elemento raiz
		rootElement = (DOMElement*)documentTree->getDocumentElement();

		return parse(rootElement, uri);
	}

	void* DocumentParser::parse(DOMElement* rootElement, string uri) {
		if (!isXmlStr(uri)) {
			string::size_type pos;
			while (true) {
				pos = uri.find_first_of(fUriD);
				if (pos == string::npos)
					break;
				uri.replace(pos,1,iUriD);
			}
		}

		documentUri = uri;

		if (!isXmlStr(uri)) {
			setDocumentPath(getPath(uri) + iUriD );
			clog << "PATH = [" << documentPath.c_str() << "]" << endl;

			//TODO: generalizar isso para qualquer numero de NCLs
			setDocumentPath(documentPath);

		} else {
			//TODO: set path of xml string
		}

		return parseRootElement(rootElement);
	}

	void DocumentParser::setDependencies() {

	}

	string DocumentParser::absoluteFile(string basePath, string fileName) {
		if (isXmlStr(fileName)) {
			return fileName;
		}

		if (isAbsolutePath(fileName)) {
			return fileName;
		}

		fileName = basePath + iUriD + fileName;

		return fileName;
	}

	void DocumentParser::initializeUserCurrentPath() {
		char path[PATH_MAX] = "";
		char* buf = getcwd(path, PATH_MAX);

		if (buf == NULL) {
			return;
		}

		userCurrentPath.assign(path, strlen(path));

		if (userCurrentPath.find_last_of(iUriD) !=
				userCurrentPath.length() - 1) {

			userCurrentPath = userCurrentPath + iUriD;
		}
	}

	string DocumentParser::getPath(string filename) {
		string path;
		string::size_type i;

		i = filename.find_last_of(iUriD);
		if (i != string::npos) {
			path = filename.substr(0, i);
		} else {
			path = "";
		}

		return path;
	}

	string DocumentParser::getIUriD() {
		return iUriD;
	}

	string DocumentParser::getFUriD() {
		return fUriD;
	}

	string DocumentParser::getUserCurrentPath() {
		return userCurrentPath;
	}

	bool DocumentParser::checkUriPrefix(string uri) {
		string::size_type len;

		len = uri.length();
		if ((len >= 10 && uri.substr(0,10) == "x-sbtvdts:")        ||
				(len >= 9 && uri.substr(0,9) == "sbtvd-ts:")       ||
				(len >= 7 && uri.substr(0,7) == "http://")         ||
				(len >= 6 && uri.substr(0,6) == "ftp://")          ||
				(len >= 7 && uri.substr(0,7) == "file://")         ||
				(len >= 6 && uri.substr(0,6) == "tcp://")          ||
				(len >= 6 && uri.substr(0,6) == "udp://")          ||
				(len >= 6 && uri.substr(0,6) == "rtp://")          ||
				(len >= 13 && uri.substr(0,13) == "ncl-mirror://") ||
				(len >= 7 && uri.substr(0,7) == "rtsp://")) {

			return true;
		}

		return false;
	}

	bool DocumentParser::isAbsolutePath(string path) {
		string::size_type i, len;

		if (isXmlStr(path)) {
			return true;
		}

		len = path.length();
		if (checkUriPrefix(path)) {
			return true;
		}

		i = path.find_first_of(fUriD);
		while (i != string::npos) {
			path.replace(i,1,iUriD);
			i = path.find_first_of(fUriD);
		}

	    if ((len >= 1 && path.substr(0, 1) == iUriD) ||
	    		(len >= 2 && path.substr(1, 2) == ":" + iUriD)) {

	    	return true;
	    }

		return false;
	}

	bool DocumentParser::isXmlStr(string location) {
		if (location.find("<") != std::string::npos ||
				location.find("?xml") != std::string::npos ||
				location.find("|") != std::string::npos) {

			return true;
		}

		return false;
	}

	string DocumentParser::getAbsolutePath(string path) {
		//uri_t* newUrl = NULL;
		string newPath = path;

		string::size_type pos;
		while (true) {
			pos = newPath.find_first_of(fUriD);
			if (pos == string::npos) {
				break;
			}
			newPath.replace(pos, 1, iUriD);
		}

		if (!isAbsolutePath(path)) {
			newPath = documentPath + newPath;
			if (!isAbsolutePath(newPath)) {
				newPath = getUserCurrentPath() + newPath;
			}
		}

		return newPath.substr(0, newPath.find_last_of(iUriD));
	}

	string DocumentParser::getDocumentPath() {
		return documentPath;
	}

	void DocumentParser::setDocumentPath(string path) {
		string::size_type pos;
		while (true) {
			pos = path.find_first_of(fUriD);
			if (pos == string::npos) {
				break;
			}

			path.replace(pos,1,iUriD);
		}

		documentPath = path;
	}

	string DocumentParser::getDocumentUri() {
	    return documentUri;
	}

	DOMDocument *DocumentParser::getDocumentTree() {
		return documentTree;
	}

	void DocumentParser::addObject(
		    string tableName, string key, void* value) {

		map<string,void*>* table;
		map<string,void*>::iterator i;

		i = genericTable->find(tableName);
		if (i != genericTable->end()) {
			table = (map<string, void*>*)i->second;
			(*table)[key] = value;
			return;
		}

		table = new map<string, void*>;
		(*table)[key] = value;
		(*genericTable)[tableName] = table;
	}

	void* DocumentParser::getObject(string tableName, string key) {
		map<string, void*>* table = NULL;
		map<string, void*>::iterator i;

		i = genericTable->find(tableName);
		if (i != genericTable->end()) {
			table = (map<string, void*>*)i->second;
		}

		if (table != NULL) {
			i = table->find(key);
			if (i != table->end()) {
				return i->second;
			}
		}

		return NULL;
	}

	void DocumentParser::removeObject(string tableName, string key) {
		map<string, void*>* table = NULL;
		map<string, void*>::iterator i;

		i = genericTable->find(tableName);
		if (i != genericTable->end()) {
			table = (map<string, void*>*)i->second;
		}

		if (table == NULL) {
			clog << "DocumentParser::removeObject Warning! name '";
			clog << tableName << "' not found "<< endl;

		} else {
			i = table->find(key);
			if (i != table->end()) {
				//delete i->second;
				table->erase(i);
				return;
			}
		}

		clog << "DocumentParser::removeObject Warning! key '" << endl;
		clog << key << "' not found" << endl;
	}

	void DocumentParser::addObjectGrouped(
		    string tableName, string key, void* value) {

		map<string, void*>* table = NULL;
		map<string, void*>::iterator i;

		vector<void*>* vec = NULL;

		i = genericTable->find(tableName);
		if (i != genericTable->end()) {
			table = (map<string, void*>*)i->second;
		}

		if (table != NULL) {
			i = table->find(tableName);
			if (i != table->end()) {
				vec = (vector<void*>*)i->second;
			}

			if (vec != NULL) {
				vec->push_back(value);

			} else {
				vec = new vector<void*>;
				vec->push_back(value);
				(*table)[key] = vec;
			}

		} else {
			vec   = new vector<void*>;
			table = new map<string, void*>;

			vec->push_back(value);
			(*table)[key] = vec;
			(*genericTable)[tableName] = table;
		}
	}

	bool DocumentParser::importDocument(
		    DocumentParser* parser, string docLocation) {

		string uri;

		if (!isAbsolutePath(docLocation)) {
			uri = absoluteFile(getDocumentPath(), docLocation);

		} else {
			uri = docLocation;
		}

		clog << "DocumentParser::importDocument from location '";
		clog << docLocation << "' translated uri = '";
		clog << uri << "'" << endl;

		parser->parse(uri, iUriD, fUriD);

		return true;
	}
}
}
}
}
}
