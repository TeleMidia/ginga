/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
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

#include "../../../include/Contents.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
#if HAVE_DIRECTFB
	set<IDirectFBVideoProvider*>* Contents::videoProviderPool = (
			new set<IDirectFBVideoProvider*>);

	set<IDirectFBImageProvider*>* Contents::imageProviderPool = (
			new set<IDirectFBImageProvider*>);

	set<IDirectFBFont*>* Contents::fontPool = (
			new set<IDirectFBFont*>);
#endif

	void* Contents::createVideoProvider(char* mrl) {
#if HAVE_DIRECTFB
		IDirectFB* dfb = (IDirectFB*)Graphics::getRoot();
		IDirectFBVideoProvider* provider = NULL;
		DFBCHECK(dfb->CreateVideoProvider(dfb, mrl, &provider));
		videoProviderPool->insert(provider);
		return (void*)provider;
#else
		return NULL;
#endif
	}

	void Contents::releaseVideoProvider(void* decoder) {
#if HAVE_DIRECTFB
		set<IDirectFBVideoProvider*>::iterator i;
		IDirectFBVideoProvider* vp;
		vp = (IDirectFBVideoProvider*)decoder;

		i = videoProviderPool->find(vp);
		if (i != videoProviderPool->end()) {
			videoProviderPool->erase(i);

		} else {
			return;
		}
#if DFB_VER == 120
		vp->Release(vp);
#else
		DFBCHECK(vp->Release(vp));
#endif /*DFB_VER*/
#endif /*HAVE_DIRECTFB*/
	}

	void* Contents::createImageProvider(char* mrl) {
#if HAVE_DIRECTFB
		IDirectFB* dfb = (IDirectFB*)Graphics::getRoot();
		IDirectFBImageProvider* provider;

		DFBCHECK(dfb->CreateImageProvider(dfb, mrl, &provider));
		imageProviderPool->insert(provider);
		return (void*)provider;
#else
		return NULL;
#endif
	}

	void Contents::releaseImageProvider(void* provider) {
#if HAVE_DIRECTFB
		set<IDirectFBImageProvider*>::iterator i;
		IDirectFBImageProvider* ip;
		ip = (IDirectFBImageProvider*)provider;

		i = imageProviderPool->find(ip);
		if (i != imageProviderPool->end()) {
			imageProviderPool->erase(i);

		} else {
			return;
		}

#if DFB_VER == 120
		ip->Release(ip);
#else
		DFBCHECK(ip->Release(ip));
#endif /*DFB_VER*/
#endif /*HAVE_DIRECTFB*/
	}

	void* Contents::createFont(char* fontUri, int heightInPixel) {
#if HAVE_DIRECTFB
		IDirectFB* dfb = (IDirectFB*)Graphics::getRoot();
		IDirectFBFont* font = NULL;
		DFBFontDescription desc;

		desc.flags = (DFBFontDescriptionFlags)(
			    DFDESC_HEIGHT | DFDESC_ATTRIBUTES);

		desc.height = heightInPixel;
		desc.attributes = (DFBFontAttributes)0;

		DFBCHECK(dfb->CreateFont(dfb, fontUri, &desc, &font));
		fontPool->insert(font);
		return (void*)font;
#else
		return NULL;
#endif
	}

	void Contents::releaseFont(void* font) {
#if HAVE_DIRECTFB
		set<IDirectFBFont*>::iterator i;
		IDirectFBFont* f;
		f = (IDirectFBFont*)font;

		i = fontPool->find(f);
		if (i != fontPool->end()) {
			fontPool->erase(i);

		} else {
			return;
		}

#if DFB_VER == 120
		f->Release(f);
#else
		DFBCHECK(f->Release(f));
#endif /*DFB_VER*/
#endif /*HAVE_DIRECTFB*/
	}
}
}
}
}
}
}
