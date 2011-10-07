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

#include "dataprocessing/dsmcc/carousel/data/DownloadServerInitiate.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	DownloadServerInitiate::DownloadServerInitiate(DsmccMessageHeader* message) {
		header = message;
		srgIor = NULL;
		data = NULL;
		processMessage();
	}

	Ior* DownloadServerInitiate::getServiceGatewayIor() {
		return this->srgIor;
	}

	void DownloadServerInitiate::processMessage() {
		int fd, rval;
		unsigned int privateDataLength;

		// dsmccmessageheader = 12
		idx = header->getAdaptationLength() + 12;

		data = (char*)malloc((header->getMessageLength() + idx));

		fd = open(header->getFileName().c_str(), O_RDONLY|O_LARGEFILE);
		if (fd >= 0) {
			rval = read(
					fd, (void*)&(data[0]), header->getMessageLength() + idx);

			//skip serverId
			idx = idx + 20;

			//compatibilityDescriptor?


			//privateDataLength
			privateDataLength = ((data[idx] & 0xFF) << 8) |
					(data[idx+1] & 0xFF);

			idx = idx + 2;

			if (privateDataLength == 0x00) {
				//skiped bad alignment
				privateDataLength = ((data[idx] & 0xFF) << 8) |
						(data[idx+1] & 0xFF);

				idx = idx + 2;
			}

			DownloadServerInitiate::processIor();

			close(fd);
//			remove(header->getFileName().c_str());

		} else {
			clog << "Message header error: could not open file ";
			clog << header->getFileName().c_str() << endl;
		}
	}


	void DownloadServerInitiate::processIor() {
		unsigned int len;
		char* field;

		//type_id
		len = ((data[idx] & 0xFF) << 24) | ((data[idx+1] & 0xFF) << 16) |
			    ((data[idx+2] & 0xFF) << 8)  | (data[idx+3] & 0xFF);

		idx = idx + 4;

		field = (char*)malloc(len);
		memcpy(field, (void*)&(data[idx]), len);
		srgIor = new Ior();
		srgIor->setTypeId((string)field);
		free(field);
		idx = idx + len;

		//check Number of TaggedProfiles
		len = ((data[idx] & 0xFF) << 24) | ((data[idx+1] & 0xFF) << 16) |
			    ((data[idx+2] & 0xFF) << 8) | (data[idx+3] & 0xFF);

		idx = idx + 4;
		if (len > 1) {
			clog << "Warning: DSI::TaggedProfiles, never reach here!!! TP = ";
			clog << len << endl;
		}

		//get ior profile tag and check it
		len = ((data[idx] & 0xFF) << 24) | ((data[idx+1] & 0xFF) << 16) |
			    ((data[idx+2] & 0xFF) << 8) | (data[idx+3] & 0xFF);

		idx = idx + 4;
		if (len != TAG_BIOP) {
			clog << "Warning: DSI::TAG_BIOP, never reach here!!!";
			clog << " TAG = " << len << endl;

		} else {
			// BIOP Profile Body
			// skip size of body
			len = ((data[idx] & 0xFF) << 24) | ((data[idx+1] & 0xFF) << 16) |
				    ((data[idx+2] & 0xFF) << 8) | (data[idx+3] & 0xFF);

			idx = idx + 4;

			// skip byte_order field
			idx++;

			/*
			 * check number of liteComponents
			 * shall have objectLocation and connBinder
			 */
			len = (data[idx] & 0xFF);
			if (len != 2) {
				clog << "Warning: liteComponents, never reach here!!! LC = ";
				clog << len << endl;
			}

			idx++;

			// objectLocation
			// check if tag == "TAG_ObjectLocation"
			len = ((data[idx] & 0xFF) << 24) | ((data[idx+1] & 0xFF) << 16) |
				    ((data[idx+2] & 0xFF) << 8) | (data[idx+3] & 0xFF);

			if (len != TAG_BIOP_OBJECT_LOCATION) {
				clog << "Warning: TAG_ObjectLocation, never reach here!!!";
				clog << " TAG = " << len << endl;

			} else {
				idx = idx + 4;

				// skip size of object data, we don't need it.
				idx++;

				//carousel_id
				len = ((data[idx] & 0xFF) << 24) |
						((data[idx+1] & 0xFF) << 16) |
						((data[idx+2] & 0xFF) << 8)  | (data[idx+3] & 0xFF);

				srgIor->setCarouselId(len);
				idx = idx + 4;

				//module_id
				len = ((data[idx] & 0xFF) << 8) | (data[idx+1] & 0xFF);
				srgIor->setModuleId(len);
				idx = idx + 2;

				//check version: major == 1 && minor == 0
				if (((data[idx] & 0xFF) != 0x01) ||
						((data[idx+1] & 0xFF) != 0x00)) {

					clog << "DSI Warning: Object version, never reach here!!!";
					clog << endl;
				}

				idx = idx + 2;

				//objectKey
				len = (data[idx] & 0xFF);
				idx++;

				// if len > 4 then the unsigned int is not larger enough
				if (len > 4) {
					clog << "Warning! Size of";
					clog << " objectKey > 4, never reaches here!";
					clog << endl;

					idx = idx + len;

				} else if (len > 0) {
					if (len == 4) {
						objectKey = ((data[idx] & 0xFF) << 24) |
							    ((data[idx+1] & 0xFF) << 16) |
							    ((data[idx+2] & 0xFF) << 8) |
							    (data[idx+3] & 0xFF);

					} else if (len == 3) {
						objectKey = ((data[idx] & 0xFF) << 16) |
					    	    ((data[idx+1] & 0xFF) << 8) |
					    	    (data[idx+2] & 0xFF);

					} else if (len == 2) {
						objectKey = ((data[idx] & 0xFF) << 8) |
								(data[idx+1] & 0xFF);

					} else {
						objectKey = (data[idx] & 0xFF);
					}

					srgIor->setObjectKey(objectKey);
					idx = idx + len;
				}
			}

			// connBinder
			// TODO: it will be Usefull?
			//skip connBinder TAG
			idx = idx + 4;

			//get size of connBinder to skip the rest of it
			len = (data[idx] & 0xFF);
			idx++;

			idx = idx + len;

			free(data);
		}
	}
}
}
}
}
}
}
}
