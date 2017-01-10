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

extern "C" {
	#include <sys/types.h>
	#include <time.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
}

#include "Matrix.h"

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

	void Matrix::setMatrix(const matrix_t* mx, ISurface* primary) {
		s32 matrix[9];

		matrix[0] = (s32)(mx->xx * 0x10000);
		matrix[1] = (s32)(mx->xy * 0x10000);
		matrix[2] = (s32)(mx->x0 * 0x10000);
		matrix[3] = (s32)(mx->yx * 0x10000);
		matrix[4] = (s32)(mx->yy * 0x10000);
		matrix[5] = (s32)(mx->y0 * 0x10000);
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
