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

#include "tsparser/MosaicDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files
	MosaicDescriptor::MosaicDescriptor() {
		descriptorLength      = 0;
		descriptorTag         = 0x51;
		mosaicEntryPoint      = 0;
		numberHorizontalCells = 0;
		numberVerticalCells   = 0;


	}

	MosaicDescriptor::~MosaicDescriptor() {

		if (cells != NULL) {
			vector<Cell*>::iterator i;
			for (i = cells->begin(); i!= cells->end(); ++i) {
				delete((*i)->elemCellsId);
				delete((*i));
				(*i) == NULL;
			}
		}
		delete cells;
		cells == NULL;
	}

	unsigned int MosaicDescriptor::getDescriptorLength() {
		return descriptorLength;
	}

	unsigned char MosaicDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	void MosaicDescriptor::print() {
		clog << "MosaicDescriptor::print printing..." << endl;
	}

	size_t MosaicDescriptor::process(char* data, size_t pos) {
		size_t remainingBytes;
		struct Cell* cell;

		descriptorLength = data[pos+1];
		pos += 2;

		mosaicEntryPoint = ((data[pos] & 0x80) >> 7);
		numberHorizontalCells = ((data[pos] & 0x70) >> 4);
		//jumping reserved_future_use
		numberVerticalCells = (data[pos] & 0x07);

		remainingBytes = descriptorLength - 1;
		cells = new vector<Cell*>;
		while(remainingBytes) {
			pos++;
			cell = new struct Cell;

			cell->logicalCellId = ((data[pos] & 0xFC) >> 2);
			//jumping reserved_future_use
			pos++;

			cell->logicalCellPresInfo = (data[pos] &0x3);
			pos++;

			cell->elemCellFieldLength = data[pos];
			pos++;

			remainingBytes -= 3;
			cell->elemCellsId = new unsigned char[cell->elemCellFieldLength];

			for (int i = 0; i < cell->elemCellFieldLength; ++i){
				//jumping reserved future use
				cell->elemCellsId[i] = (data[pos] & 0x2F);
				pos++;
				remainingBytes--;
			}

			cell->cellLinkageInfo = data[pos];
			pos++;
			remainingBytes--;

			if (cell->cellLinkageInfo == 0x01) {
				cell->bouquetId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));
				remainingBytes -= 2;
			}
			else if(cell->cellLinkageInfo == 0x02 ||
					cell->cellLinkageInfo == 0x03 ||
					cell->cellLinkageInfo == 0x04) {

				cell->originalNetworkId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));

				pos += 2;
				cell->transportStreamId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));

				pos+=2;

				cell->serviceId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));

				remainingBytes -= 6;
				pos++;

				if (cell->cellLinkageInfo == 0x04){
					pos ++;
					cell->eventId = (((data[pos] << 8) & 0xFF00) |
							(data[pos+1] & 0xFF));
					remainingBytes -= 2;
				}
			}
			cells->push_back(cell);
		}
		return pos;

	}
}
}
}
}
}
}
}
}
