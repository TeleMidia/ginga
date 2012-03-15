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
*******************************************************************************
The contents of file were based on the Cairo Graphics Library, originally
developed by University of Southern California.
*******************************************************************************/

extern "C" {
	#include <sys/types.h>
	#include <time.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
}

#include "mb/interface/Matrix.h"

#ifndef s32
typedef int32_t s32;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	void Matrix::multiply(
			matrix_t* result, const matrix_t* a, const matrix_t* b) {

		matrix_t r;

		r.xx = a->xx * b->xx + a->yx * b->xy;
		r.yx = a->xx * b->yx + a->yx * b->yy;

		r.xy = a->xy * b->xx + a->yy * b->xy;
		r.yy = a->xy * b->yx + a->yy * b->yy;

		r.x0 = a->x0 * b->xx + a->y0 * b->xy + b->x0;
		r.y0 = a->x0 * b->yx + a->y0 * b->yy + b->y0;

		*result = r;
	}

	void Matrix::init(
			matrix_t* matrix,
			double xx, double yx,
			double xy, double yy,
			double x0, double y0) {

		matrix->xx = xx; matrix->yx = yx;
		matrix->xy = xy; matrix->yy = yy;
		matrix->x0 = x0; matrix->y0 = y0;
	}

	void Matrix::initIdentity(matrix_t* matrix) {
		init(
				matrix,
				1, 0,
				0, 1,
				0, 0);
	}

	void Matrix::initTranslate(matrix_t* matrix, double tx, double ty) {
		init(
				matrix,
				1, 0,
				0, 1,
				tx, ty);
	}

	void Matrix::translate(matrix_t* matrix, double tx, double ty) {
		matrix_t tmp;

		initTranslate(&tmp, tx, ty);

		multiply(matrix, &tmp, matrix);
	}

	void Matrix::initScale(matrix_t* matrix, double sx, double sy) {
	    init(
	    		matrix,
	    		sx, 0,
	    		0, sy,
	    		0, 0);
	}

	void Matrix::scale(matrix_t* matrix, double sx, double sy) {
		matrix_t tmp;

		initScale (&tmp, sx, sy);

		multiply (matrix, &tmp, matrix);
	}

	void Matrix::initRotate(matrix_t* matrix, double radians) {
		double  s;
		double  c;

		s = sin(radians);
		c = cos(radians);

		init(
				matrix,
				c, s,
				-s, c,
				0, 0);
	}

	void Matrix::rotate(matrix_t* matrix, double radians) {
		matrix_t tmp;

		initRotate(&tmp, radians);
		multiply(matrix, &tmp, matrix);
	}

	void Matrix::setMatrix(const matrix_t* cairo, ISurface* primary) {
		s32 matrix[9];

		matrix[0] = (s32)(cairo->xx * 0x10000);
		matrix[1] = (s32)(cairo->xy * 0x10000);
		matrix[2] = (s32)(cairo->x0 * 0x10000);
		matrix[3] = (s32)(cairo->yx * 0x10000);
		matrix[4] = (s32)(cairo->yy * 0x10000);
		matrix[5] = (s32)(cairo->y0 * 0x10000);
		matrix[6] = 0x00000;
		matrix[7] = 0x00000;
		matrix[8] = 0x10000;

		primary->setMatrix((void*)matrix);
	}
}
}
}
}
}
}
