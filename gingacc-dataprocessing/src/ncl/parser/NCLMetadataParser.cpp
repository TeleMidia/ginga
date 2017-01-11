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

#include "dataprocessing/ncl/parser/NCLMetadataParser.h"

#include "dataprocessing/ncl/DataFile.h"
#include "dataprocessing/ncl/Metadata.h"

#include <string.h>
#include <stdio.h>

typedef struct {
	::br::pucrio::telemidia::ginga::core::dataprocessing::ncl::IMetadata* metadata;
	bool isParsing;
} MetadataAndParser;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace ncl {
	IMetadata* NCLMetadataParser::parse(string xmlDocument) {
		FILE* fd;
		int bytes = 1;
		int fileSize;

		fd = fopen(xmlDocument.c_str(), "rb");
		if (fd < 0) {
			clog << "NCLMetadataParser::parse: can't open file:" << xmlDocument;
			clog << endl;
			return NULL;
		}

		fseek(fd, 0L, SEEK_END);
		fileSize = ftell(fd);
		fclose(fd);

		if (fileSize <= 0) {
			clog << "NCLMetadataParser::parse: file '" << xmlDocument;
			clog << "' is empty" << endl;
			return NULL;
		}

		fd = fopen(xmlDocument.c_str(), "rb");
		if (fd == NULL) {
			clog << "NCLMetadataParser::parse2: can't open file:" << xmlDocument;
			clog << endl;
			return NULL;
		}

		char* content = new char[fileSize];
		bytes = fread(content, 1, fileSize, fd);
		if (bytes != fileSize) {
			clog << "NCLMetadataParser::parse3: can't read '" << bytes;
			clog << "' bytes from '" << xmlDocument << "'";
			clog << endl;
			return NULL;
		}

		fclose(fd);

		return parse(content, fileSize);
	}

	IMetadata* NCLMetadataParser::parse(char* xmlStream, int streamSize) {
		MetadataAndParser* mp;
		IMetadata* metadata;

		XML_Parser parser = XML_ParserCreate(NULL);

		mp            = new MetadataAndParser();
		mp->metadata  = NULL;
		mp->isParsing = true;

		XML_SetUserData(parser, mp);
		XML_SetElementHandler(
				parser,
				NCLMetadataParser::startElementHandler,
				NCLMetadataParser::stopElementHandler);

		if (XML_Parse(parser, xmlStream, streamSize, true) ==
				XML_STATUS_ERROR) {

			clog << "NCLMetadataParser::parse() error '";
			clog << XML_ErrorString(XML_GetErrorCode(parser));
			clog << "' at line '" << XML_GetCurrentLineNumber(parser);
			clog << "'" << endl;
			delete mp;
			XML_ParserFree(parser);
			return NULL;
		}

		if (mp->isParsing) {
			clog << "NCLMetadataParser::parse4 Warning! underflow";
			clog << " XML parser ";
			clog << endl;
		}

		metadata = mp->metadata;
		delete mp;
		XML_ParserFree(parser);

		return metadata;
	}

	void NCLMetadataParser::startElementHandler(
			void* data, const XML_Char* element, const XML_Char** attrs) {

		IMetadata* nm = ((MetadataAndParser*)data)->metadata;
		bool isValid = (nm != NULL);
		clog << "NCLMetadataParser::startElementHandler element '";
		clog << element << "'" << endl;

		if (strcmp(element, "metadata") == 0) {
			parseMetadata(data, attrs);

		} else if (strcmp(element, "baseData") == 0 && isValid) {
			parseBaseData(data, attrs);

		} else if (strcmp(element, "pushedRoot") == 0 && isValid) {
			parseRoot(data, attrs);

		} else if (strcmp(element, "pushedData") == 0 && isValid) {
			parseData(data, attrs);
		}
	}

	void NCLMetadataParser::parseMetadata(void* data, const XML_Char** attrs) {
		string name = "";
		double size = 0;

		for (int i = 0; i < 4; i = i + 2) {
			if (strcmp(attrs[i], "name") == 0) {
				name = attrs[i + 1];

			} else if (strcmp(attrs[i], "size") == 0) {
				size = util::stof((string)attrs[i + 1]);
			}
		}

		if (name != "" && size > 0) {
			((MetadataAndParser*)data)->metadata = new Metadata(name);
			((MetadataAndParser*)data)->metadata->setTargetTotalLength(size);
		}
	}

	void NCLMetadataParser::parseBaseData(void* data, const XML_Char** attrs) {
		IMetadata* nm = ((MetadataAndParser*)data)->metadata;
		string uri = "";

		for (int i = 0; i < 2; i = i + 2) {
			if (strcmp(attrs[i], "uri") == 0) {
				uri = attrs[i + 1];
			}
		}

		if (uri != "") {
			nm->setBaseUri(uri);
		}
	}

	void NCLMetadataParser::parseRoot(void* data, const XML_Char** attrs) {
		IMetadata* nm = ((MetadataAndParser*)data)->metadata;
		IDataFile* pushedRoot = createObject(data, attrs);

		nm->setRootObject(pushedRoot);
	}

	void NCLMetadataParser::parseData(void* data, const XML_Char** attrs) {
		IMetadata* nm = ((MetadataAndParser*)data)->metadata;
		IDataFile* pushedData = createObject(data, attrs);

		nm->addDataFile(pushedData);
	}

	IDataFile* NCLMetadataParser::createObject(
			void* data, const XML_Char** attrs) {

		IDataFile* dataObject = NULL;
		int structureId     = -1;
		double size         = 0;
		string componentTag = "";
		string uri          = "";

		for (int i = 0; i < 8; i = i + 2) {
			if (strcmp(attrs[i], "structureId") == 0) {
				structureId = util::stof((string)attrs[i + 1]);

			} else if (strcmp(attrs[i], "size") == 0) {
				size = util::stof((string)attrs[i + 1]);

			} else if (strcmp(attrs[i], "component_tag") == 0) {
				componentTag = attrs[i + 1];

			} else if (strcmp(attrs[i], "uri") == 0) {
				uri = attrs[i + 1];
			}
		}

		if (structureId != -1 && size != 0 && componentTag != "" && uri != "") {
			dataObject = new DataFile(structureId);
			dataObject->setComponentTag(componentTag);
			dataObject->setUri(uri);
			dataObject->setSize(size);
		}

		return dataObject;
	}

	void NCLMetadataParser::stopElementHandler(
			void* data, const XML_Char* element) {

		if (strcmp(element, "metadata") == 0) {
			((MetadataAndParser*)data)->isParsing = false;
		}
	}
}
}
}
}
}
}
}
