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

#ifndef DX2DSURFACE_H_
#define DX2DSURFACE_H_


#include <pthread.h>
//#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#define SURFACEFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct{
	int x;
	int y;
	FLOAT z;
	int width;
	int height;
	DWORD color;
}DX2DSurfaceProp;

struct SURFACEVERTEX{
	FLOAT x, y, z, rhw;	// from the D3DFVF_XYZRHW flag
	DWORD color;		// from the D3DFVF_DIFFUSE flag
	FLOAT tu, tv;		// Texture coordinates
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {

	class DX2DSurface {
		public:
			DX2DSurface(int x=0, int y=0, int w=10, int h=10, IDirect3DDevice9 *dev=NULL);
			virtual ~DX2DSurface();
			int getX();
			int getY();
			int getWidth();
			int getHeight();
			LPDIRECT3DTEXTURE9 getTexture();
			void setTexture(LPDIRECT3DTEXTURE9 tex);
			void draw2DSurface();
			void setProperties(DX2DSurfaceProp* surProp);
			void blit(int x, int y, DX2DSurface* src, int srcX, int srcY, int srcW, int srcH);
			void resize(int width, int height);
			void moveTo(int posX, int posY);
			void show();
			void hide();
			void update();
			void fill(int x, int y, int w, int h);
			void drawString(int x, int y, const char* txt);

		private:
			void buildVertex();
			void initGraphics();

		protected:
			SURFACEVERTEX vertex[4];
			FLOAT	x, y, z;
			INT		alpha;
			INT		width;
			INT		height;
			DWORD color;
			LPDIRECT3DDEVICE9		pD3ddev;
			LPDIRECT3DVERTEXBUFFER9 pVbuffer;
			LPDIRECT3DINDEXBUFFER9	pIndexBuffer;
			LPDIRECT3DTEXTURE9		pTex;
			LPDIRECT3DTEXTURE9		pTexCanvas;
			LPDIRECT3DTEXTURE9		pTexFont;
			LPD3DXSPRITE            pSprite;

			bool	blitEnable;
			bool	visible;

			pthread_mutex_t visible_lock;
			pthread_mutex_t tex_lock;
	};

}
}
}
}
}
}
}

#endif // DX2DSURFACE_H_