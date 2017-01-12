/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef GINGAMATRIX_H_
#define GINGAMATRIX_H_

#include "ISurface.h"

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	typedef struct matrix {
		double xx; double yx;
		double xy; double yy;
		double x0; double y0;
	} matrix_t;

	class Matrix {
		public:			
			static void multiply(
					matrix_t* result, const matrix_t* a, const matrix_t* b);

			static void init(
					matrix_t* matrix,
					double xx, double yx,
					double xy, double yy,
					double x0, double y0);

			static void initIdentity(matrix_t* matrix);
			static void initTranslate(matrix_t* matrix, double tx, double ty);
			static void translate(matrix_t* matrix, double tx, double ty);
			static void initScale(matrix_t* matrix, double sx, double sy);
			static void scale(matrix_t* matrix, double sx, double sy);
			static void initRotate(matrix_t* matrix, double radians);
			static void rotate(matrix_t* matrix, double radians);
			static void setMatrix(const matrix_t* cairo,  ISurface* primary);
	};
}
}
}
}
}
}

#endif //GINGAMATRIX_H_
