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

#include "ncl/transition/TransitionUtil.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace transition {
   	int TransitionUtil::getTypeCode(string type) {
		if (type == "barWipe") {
			return Transition::TYPE_BARWIPE;

		} else if (type == "irisWipe") {
			return Transition::TYPE_IRISWIPE;

		} else if (type == "clockWipe") {
			return Transition::TYPE_CLOCKWIPE;

		} else if (type == "snakeWipe") {
			return Transition::TYPE_SNAKEWIPE;

		} else if (type == "fade") {
			return Transition::TYPE_FADE;

		} else {
			return -1;
		}
	}

	string TransitionUtil::getTypeName(int type) {
		switch (type) {
			case Transition::TYPE_BARWIPE:
				return "barWipe";

			case Transition::TYPE_IRISWIPE:
				return "irisWipe";

			case Transition::TYPE_CLOCKWIPE:
				return "clockWipe";

			case Transition::TYPE_SNAKEWIPE:
				return "snakeWipe";

			case Transition::TYPE_FADE:
				return "fade";

			default:
				return "";
		}
	}

	int TransitionUtil::getSubtypeCode(int type, string subtype) {
		switch (type) {
			case Transition::TYPE_BARWIPE:
				if (subtype == "leftToRight") {
					return Transition::SUBTYPE_BARWIPE_LEFTTORIGHT;

				} else if (subtype == "topToBottom") {
					return Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM;
				}

				break;

			case Transition::TYPE_IRISWIPE:
				if (subtype == "rectangle") {
					return Transition::SUBTYPE_IRISWIPE_RECTANGLE;

				} else if (subtype == "diamond") {
					return Transition::SUBTYPE_IRISWIPE_DIAMOND;
				}

				break;

			case Transition::TYPE_CLOCKWIPE:
				if (subtype == "clockwiseTwelve") {
					return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE;

				} else if (subtype == "clockwiseThree") {
					return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETHREE;

				} else if (subtype == "clockwiseSix") {
					return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISESIX;

				} else if (subtype == "clockwiseNine") {
					return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE;
				}

				break;

			case Transition::TYPE_SNAKEWIPE:
				if (subtype == "topLeftHorizontal") {
					return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL;

				} else if (subtype == "topLeftVertical") {
					return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTVERTICAL;

				} else if (subtype == "topLeftDiagonal") {
					return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTDIAGONAL;

				} else if (subtype == "topRightDiagonal") {
					return Transition::SUBTYPE_SNAKEWIPE_TOPRIGHTDIAGONAL;

				} else if (subtype == "bottomRightDiagonal") {
					return Transition::SUBTYPE_SNAKEWIPE_BOTTOMRIGHTDIAGONAL;

				} else if (subtype == "bottomLeftDiagonal") {
					return Transition::SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL;
				}

				break;

			case Transition::TYPE_FADE:
				if (subtype == "crossfade") {
					return Transition::SUBTYPE_FADE_CROSSFADE;

				} else if (subtype == "fadeToColor") {
					return Transition::SUBTYPE_FADE_FADETOCOLOR;

				} else if (subtype == "fadeFromColor") {
					return Transition::SUBTYPE_FADE_FADEFROMCOLOR;
				}

				break;
		}

		return -1;
	}

	string TransitionUtil::getSubtypeName(int type, int subtype) {
		string subtypeName = "";

		switch (type) {
			case Transition::TYPE_BARWIPE:
				switch (subtype) {
					case Transition::SUBTYPE_BARWIPE_LEFTTORIGHT:
						subtypeName = "leftToRight";
						break;

					case Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM:
						subtypeName = "topToBottom";
						break;

					default:
						break;
				}
				break;

			case Transition::TYPE_IRISWIPE:
				switch (subtype) {
					case Transition::SUBTYPE_IRISWIPE_RECTANGLE:
						subtypeName = "rectangle";
						break;

					case Transition::SUBTYPE_IRISWIPE_DIAMOND:
						subtypeName = "diamond";
						break;

					default:
						break;
				}
				break;

			case Transition::TYPE_CLOCKWIPE:
				switch (subtype) {
					case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE:
						subtypeName = "clockwiseTwelve";
						break;

					case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETHREE:
						subtypeName = "clockwiseThree";
						break;

					case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISESIX:
						subtypeName = "clockwiseSix";
						break;

					case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE:
						subtypeName = "clockwiseNine";
						break;

					default:
						break;
				}
				break;

			case Transition::TYPE_SNAKEWIPE:
				switch (subtype) {
					case Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL:
						subtypeName = "topLeftHorizontal";
						break;

					case Transition::SUBTYPE_SNAKEWIPE_TOPLEFTVERTICAL:
						subtypeName = "topLeftVertical";
						break;

					case Transition::SUBTYPE_SNAKEWIPE_TOPLEFTDIAGONAL:
						subtypeName = "topLeftDiagonal";
						break;

					case Transition::SUBTYPE_SNAKEWIPE_TOPRIGHTDIAGONAL:
						subtypeName = "topRightDiagonal";
						break;

					case Transition::SUBTYPE_SNAKEWIPE_BOTTOMRIGHTDIAGONAL:
						subtypeName = "bottomRigthDiagonal";
						break;

					case Transition::SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL:
						subtypeName = "bottomLeftDiagonal";
						break;

					default:
						break;
				}
				break;

			case Transition::TYPE_FADE:
				switch (subtype) {
					case Transition::SUBTYPE_FADE_CROSSFADE:
						subtypeName = "crossfade";
						break;

					case Transition::SUBTYPE_FADE_FADETOCOLOR:
						subtypeName = "fadeToColor";
						break;

					case Transition::SUBTYPE_FADE_FADEFROMCOLOR:
						subtypeName = "fadeFromColor";
						break;

					default:
						break;
				}
				break;

			default:
				break;
		}

		return (subtypeName);
	}

	short TransitionUtil::getDirectionCode(string direction) {
		if (direction == "forward") {
			return Transition::DIRECTION_FORWARD;

		} else if (direction == "reverse") {
			return Transition::DIRECTION_REVERSE;

		} else {
  			return -1;
		}
	}

	string TransitionUtil::getDirectionName(short direction) {
		switch (direction) {
			case Transition::DIRECTION_FORWARD:
				return "forward";

			case Transition::DIRECTION_REVERSE:
				return "reverse";

			default:
				return "";
		}
	}

	int TransitionUtil::getDefaultSubtype(int type) {
		switch (type) {
			case Transition::TYPE_BARWIPE:
				return Transition::SUBTYPE_BARWIPE_LEFTTORIGHT;

			case Transition::TYPE_IRISWIPE:
				return Transition::SUBTYPE_IRISWIPE_RECTANGLE;

			case Transition::TYPE_CLOCKWIPE:
				return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE;

			case Transition::TYPE_SNAKEWIPE:
				return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL;

			case Transition::TYPE_FADE:
				return Transition::SUBTYPE_FADE_CROSSFADE;

			default:
				return Transition::SUBTYPE_FADE_CROSSFADE;
		}
	}
}
}
}
}
}
