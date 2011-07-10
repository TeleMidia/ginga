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

#include <stdlib.h>
#include "system/io/interface/output/dfb/DFBSurface.h"
#include "system/io/interface/output/dfb/Matrix.h"
#include "system/io/interface/output/dfb/DFBWindow.h"
#include "system/io/interface/content/text/IFontProvider.h"
#include "system/io/LocalDeviceManager.h"

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>: \n\t", __FILE__, __LINE__ );   \
		DirectFBError( #x, err );                                 \
	}                                                             \
}
#endif /*DFBCHECK*/

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DFBSurface::DFBSurface() {
		this->sur           = NULL;
		this->parent        = NULL;
		this->chromaColor   = NULL;
		this->caps          = 0;
		this->hasExtHandler = false;
	}

	DFBSurface::DFBSurface(void* someSurface) {
		this->sur           = (IDirectFBSurface*)someSurface;
		this->parent        = NULL;
		this->chromaColor   = NULL;
		this->caps          = 0;
		this->hasExtHandler = false;
	}

	DFBSurface::DFBSurface(int w, int h) {
		DFBSurfaceDescription surDsc;
		surDsc.width = w;
		surDsc.height = h;
		surDsc.pixelformat = DSPF_LUT8;
		surDsc.caps = (DFBSurfaceCapabilities)(DSCAPS_ALL);
		surDsc.flags = (DFBSurfaceDescriptionFlags)(
				DSDESC_CAPS | DSDESC_WIDTH |
				DSDESC_HEIGHT | DSDESC_PIXELFORMAT);

	     /*desc.flags       = DSDESC_CAPS | DSDESC_PIXELFORMAT;
	     desc.caps        = DSCAPS_PRIMARY | DSCAPS_DOUBLE;
	     desc.pixelformat = DSPF_LUT8;*/

		this->caps = 0;
		this->sur = (IDirectFBSurface*)(
				LocalDeviceManager::getInstance()->createSurface(&surDsc));

		this->parent        = NULL;
		this->chromaColor   = NULL;
		this->hasExtHandler = false;
	}

	DFBSurface::~DFBSurface() {
		if (chromaColor != NULL) {
			delete chromaColor;
			chromaColor = NULL;
		}

		if (sur != NULL) {
			if (parent != NULL) {
				if (parent->removeChildSurface(this)) {
					DFBCHECK(sur->Clear(sur, 0, 0, 0, 0x00));
					LocalDeviceManager::getInstance()->releaseSurface(sur);
					sur = NULL;
				}

			} else {
				DFBCHECK(sur->Clear(sur, 0, 0, 0, 0x00));
				LocalDeviceManager::getInstance()->releaseSurface(sur);
				sur = NULL;
			}
		}
	}

	void DFBSurface::setExternalHandler(bool extHandler) {
		this->hasExtHandler = extHandler;
	}

	bool DFBSurface::hasExternalHandler() {
		return this->hasExtHandler;
	}

	void DFBSurface::addCaps(int caps) {
		this->caps = this->caps | caps;
	}

	void DFBSurface::setCaps(int caps) {
		this->caps = caps;
	}

	int DFBSurface::getCap(string cap) {
		if (cap == "ALL") {
			return DWCAPS_ALL;
		} else if (cap == "NOSTRUCTURE") {
			return DWCAPS_NODECORATION;
		} else if (cap == "ALPHACHANNEL") {
			return DWCAPS_ALPHACHANNEL;
		} else {
			return DWCAPS_NONE;
		}
	}

	int DFBSurface::getCaps() {
		return this->caps;
	}

	void* DFBSurface::getContent() {
		return sur;
	}

	void DFBSurface::setContent(void* surface) {
		if (this->sur != NULL && surface != NULL) {
			if (parent == NULL || (parent)->removeChildSurface(this)) {
				DFBCHECK(sur->Clear(sur, 0, 0, 0, 0x00));
				LocalDeviceManager::getInstance()->releaseSurface(sur);
				sur = NULL;
			}
		}
		this->sur = (IDirectFBSurface*)surface;
	}

	bool DFBSurface::setParent(void* parentWindow) {
		this->parent = (IWindow*)parentWindow;
		if (parent != NULL && chromaColor != NULL) {
			parent->setColorKey(
				    chromaColor->getR(),
				    chromaColor->getG(),
				    chromaColor->getB());
		}

		if (this->sur == NULL && parent != NULL) {
			IDirectFBWindow* wgWin;

			wgWin = (IDirectFBWindow*)(parent->getContent());
			DFBCHECK(wgWin->GetSurface(wgWin, &sur));
			DFBCHECK(sur->Clear(sur, 0, 0, 0, 0x00));
			parent->setReleaseListener(this);
			return false;
		}

		if (parent != NULL) {
			parent->addChildSurface(this);
		}
		return true;
	}

	void* DFBSurface::getParent() {
		return this->parent;
	}

	void DFBSurface::setChromaColor(IColor* color) {
		if (this->chromaColor != NULL) {
			delete this->chromaColor;
			chromaColor = NULL;
		}

		this->chromaColor = color;

		if (sur != NULL) {
			DFBCHECK(sur->SetSrcColorKey(
					sur,
				    chromaColor->getR(),
				    chromaColor->getG(),
				    chromaColor->getB()));

			DFBCHECK(sur->SetBlittingFlags(sur,
					(DFBSurfaceBlittingFlags)(
							DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_COLORKEY)));

		} else {
			cout << "DFBSurface::setChromaColor Warning! ";
			cout << "Can't set chroma color: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	IColor* DFBSurface::getChromaColor() {
		return this->chromaColor;
	}

	void DFBSurface::clearContent() {
		if (sur == NULL) {
			cout << "DFBSurface::clearContent Warning! ";
			cout << "Can't clear content: ";
			cout << "internal surface is NULL" << endl;
			return;
		}

		DFBCHECK(sur->Clear(sur, 0, 0, 0, 0xFF));
		if (parent != NULL) {
			parent->clearContent();
		}
	}

	void DFBSurface::clearSurface() {
		if (sur == NULL) {
			cout << "DFBSurface::clearSurface Warning! ";
			cout << "Can't clear surface: ";
			cout << "internal surface is NULL" << endl;
			return;
		}

		DFBCHECK(sur->Clear(sur, 0, 0, 0, 0xFF));
	}

	ISurface* DFBSurface::getSubSurface(int x, int y, int w, int h) {
		IDirectFBSurface* s = NULL;
		ISurface* subSurface = NULL;
		DFBRectangle rect;

		if (this->sur == NULL) {
			cout << "DFBSurface::getSubSurface Warning! ";
			cout << "Can't get sub surface: ";
			cout << "internal surface is NULL" << endl;
			return NULL;
		}

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		DFBCHECK(sur->GetSubSurface(sur, &rect, &s));

		subSurface = new DFBSurface(s);
		subSurface->setParent(parent);
		return subSurface;
	}

	void DFBSurface::drawLine(int x1, int y1, int x2, int y2) {
		if (sur != NULL) {
			sur->DrawLine(sur, x1, y1, x2, y2);

		} else {
			cout << "DFBSurface::drawLine Warning! ";
			cout << "Can't draw line: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	void DFBSurface::drawRectangle(int x, int y, int w, int h) {
		if (sur != NULL) {
			DFBCHECK( sur->DrawRectangle(sur, x, y, w, h) );

		} else {
			cout << "DFBSurface::drawRectangle Warning! ";
			cout << "Can't draw rectangle: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	void DFBSurface::fillRectangle(int x, int y, int w, int h) {
		if (sur != NULL) {
			DFBCHECK( sur->FillRectangle(sur, x, y, w, h) );

		} else {
			cout << "DFBSurface::fillRectangle Warning! ";
			cout << "Can't fill rectangle: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	void DFBSurface::drawString(int x, int y, const char* txt) {
		if (sur != NULL) {
			DFBCHECK(sur->DrawString(sur,
					txt, -1, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT|DSTF_TOP)));

		} else {
			cout << "DFBSurface::drawString Warning! ";
			cout << "Can't draw string: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	void DFBSurface::setBorder(IColor* borderColor) {
		int w, h;

		if (sur == NULL) {
			cout << "DFBSurface::setBorder Warning! ";
			cout << "Can't set border color: ";
			cout << "internal surface is NULL" << endl;
			return;
		}

		DFBCHECK(sur->GetSize(sur, &w, &h));
		DFBCHECK(sur->SetColor(
				sur,
				borderColor->getR(),
				borderColor->getG(),
				borderColor->getB(),
				0xFF));

		DFBCHECK(sur->DrawRectangle(sur, 0, 0, w, h));

		if (parent != NULL) {
			parent->validate();
		}
	}

	void DFBSurface::setColor(IColor* writeColor) {
		if (sur == NULL) {
			cout << "DFBSurface::setColor Warning! ";
			cout << "Can't set color: ";
			cout << "internal surface is NULL" << endl;
			return;
		}

		DFBCHECK(sur->SetColor(sur,
				writeColor->getR(),
				writeColor->getG(),
				writeColor->getB(),
				writeColor->getAlpha()));
	}

	void DFBSurface::setBgColor(IColor* bgColor) {
		if (sur == NULL) {
			cout << "DFBSurface::setBgColor Warning! ";
			cout << "Can't set background color: ";
			cout << "internal surface is NULL" << endl;
			return;
		}

		DFBCHECK(sur->Clear(
				sur, bgColor->getR(),
				bgColor->getG(), bgColor->getB(), bgColor->getAlpha()));
	}

	void DFBSurface::setFont(void* font) {
		IDirectFBFont* f = NULL;

		if (sur != NULL) {
			f = (IDirectFBFont*)(((IFontProvider*)font)->getContent());
			if (f != NULL) {
				DFBCHECK(sur->SetFont(sur, f));
				return;
			}
		}

		cout << "DFBSurface::setFont Warning! Can't set font '" << f << "'";
		cout << " surface '" << sur << "'" << endl;
	}

	void DFBSurface::flip() {
		if (sur != NULL) {
			DFBCHECK(sur->Flip (sur, NULL, DSFLIP_NONE));

		} else {
			cout << "DFBSurface::flip Warning! ";
			cout << "Can't flip surface: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	void DFBSurface::scale(double x, double y) {
		int width, height;

		if (sur == NULL) {
			cout << "DFBSurface::scale Warning! ";
			cout << "Can't scale surface: ";
			cout << "internal surface is NULL" << endl;
			return;
		}

		sur->GetSize(sur, &width, &height);

		/*Copy to a temporary surface*/
		DFBSurface* sur_temp = new DFBSurface(width, height);		
		IDirectFBSurface* temp = (IDirectFBSurface*)(sur_temp->getContent());		
		temp->SetBlittingFlags(
				temp, (DFBSurfaceBlittingFlags)(
						DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_COLORKEY));

		temp->Blit(temp, sur, NULL, 0, 0);		

		/* Clear the frame. */
		sur->SetDrawingFlags(sur, (DFBSurfaceDrawingFlags)(DSDRAW_NOFX) );
		sur->Clear(sur, 0x00, 0x00, 0x00, 0x00 );
		
		sur->SetRenderOptions(
				sur, (DFBSurfaceRenderOptions)(DSRO_MATRIX | DSRO_ANTIALIAS));

		/*Scale the matrix*/
		matrix_t matrix;
		Matrix::initTranslate(&matrix, width/2, height/2);				
		Matrix::scale(&matrix, x, y);

		Matrix::setMatrix(&matrix, sur);
		
		/*Copy back*/
		sur->SetBlittingFlags(
				sur,
				(DFBSurfaceBlittingFlags)(
						DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_COLORKEY));

		sur->Blit(sur, temp, NULL, -width/2, -height/2);

		/* Flip the output surface. */
 		sur->Flip( sur, NULL, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
		delete sur_temp;
	}

	void DFBSurface::blit(
			int x, int y, ISurface* src,
			int srcX, int srcY, int srcW, int srcH) {

		DFBRectangle rect, *r = NULL;

		if (src != NULL) {
			if (srcX >= 0) {
				rect.x = srcX;
				rect.y = srcY;
				rect.w = srcW;
				rect.h = srcH;
				r = &rect;
			}

			if (sur != NULL) {
				DFBCHECK(sur->SetBlittingFlags(
						sur,
						(DFBSurfaceBlittingFlags)(
								DSBLIT_BLEND_ALPHACHANNEL |
								DSBLIT_SRC_COLORKEY)));

				DFBCHECK( sur->Blit(
						sur, (IDirectFBSurface*)(src->getContent()), r, x, y));

			} else {
				cout << "DFBSurface::blit Warning! ";
				cout << "Can't blit surface: ";
				cout << "internal surface is NULL" << endl;
			}

		} else {
			cout << "DFBSurface::blit Warning! ";
			cout << "Can't blit surface: ";
			cout << "source surface is NULL" << endl;
		}
	}

	void DFBSurface::getStringExtents(const char* text, int* w, int* h) {
		DFBRectangle rect;
		IDirectFBFont* font;

		if (sur != NULL) {
			DFBCHECK( sur->GetFont(sur, &font) );
			DFBCHECK( font->GetStringExtents(font, text, -1, &rect, NULL) );
			*w = rect.w;
			*h = rect.h;

		} else {
			cout << "DFBSurface::getStringExtends Warning! ";
			cout << "Can't get string info: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	void DFBSurface::setClip(int x, int y, int w, int h) {
		DFBRegion rg;

		rg.x1 = x;
		rg.y1 = y;
		rg.x2 = x+w;
		rg.y2 = y+h;

		if (sur != NULL) {
			DFBCHECK( sur->SetClip(sur, &rg) );

		} else {
			cout << "DFBSurface::setClip Warning! ";
			cout << "Can't set surface clip: ";
			cout << "internal surface is NULL" << endl;
		}
	}

	void DFBSurface::getSize(int* w, int* h) {
		if (sur == NULL) {
			cout << "DFBSurface::getSize Warning! ";
			cout << "Can't get surface size: ";
			cout << "internal surface is NULL" << endl;
			return;
		}

		sur->GetSize(sur, w, h);
	}

	string DFBSurface::getDumpFileUri() {
		string uri;

		if (sur == NULL) {
			cout << "DFBSurface::getDumpFileUri Warning! ";
			cout << "Can't dump surface bitmap: ";
			cout << "internal surface is NULL" << endl;

			uri = "";

		} else {
			uri = "/tmp/dump_0000";
			remove((char*)((uri + ".ppm").c_str()));
			remove((char*)((uri + ".pgm").c_str()));
			sur->Dump(sur, "/tmp", "dump");
		}

		return uri + ".ppm";
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::ISurface*
		createDFBSurface(void* sur, int w, int h) {

	if (sur != NULL) {
		return (new ::br::pucrio::telemidia::ginga::core::system::io::
				DFBSurface(sur));

	} else if (w != 0 && h != 0) {
		return (new ::br::pucrio::telemidia::ginga::core::system::io::
				DFBSurface(w, h));

	} else {
		return (new ::br::pucrio::telemidia::ginga::core::system::io::
				DFBSurface());
	}
}

extern "C" void destroyDFBSurface(
		::br::pucrio::telemidia::ginga::core::system::io::ISurface* s) {

	delete s;
}
