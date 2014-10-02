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

#include "util/Color.h"
#include "mb/interface/IFontProvider.h"
#include "mb/interface/ISurface.h"

#include "player/TextPlayer.h"

#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	TextPlayer::TextPlayer(GingaScreenID screenId) : Player(screenId, "") {
   		initializePlayer(screenId);
   	}

	TextPlayer::~TextPlayer() {
   		if (fontColor != NULL) {
   			delete fontColor;
   			fontColor = NULL;
   		}

   		if (bgColor != NULL) {
   			delete bgColor;
   			bgColor = NULL;
   		}

   		/* release window and surface first; then, release font */
		if (outputWindow != NULL && dm->hasWindow(myScreen, outputWindow)) {
			dm->revertWindowContent(myScreen, outputWindow);
			dm->deleteWindow(myScreen, outputWindow);

			outputWindow = 0;
		}

		if (surface != NULL && dm->hasSurface(myScreen, surface)) {
			dm->deleteSurface(surface);
			surface = 0;
		}

   		if (font != 0) {
   			dm->releaseFontProvider(myScreen, font);
   			font = 0;
   		}
   	}

	void TextPlayer::initializePlayer(GingaScreenID screenId) {
		this->fontHeight    = 0;
		this->currentAlign  = IFontProvider::FP_TA_LEFT;
		this->currentLine   = 0;
		this->currentColumn = 0;
		this->tabSize       = 0;
		this->font          = 0;
		this->bgColor       = NULL;
		this->fontColor     = NULL;
		this->fontSize      = 12;
		this->fontUri       = SystemCompat::appendGingaFilesPrefix(
				"font/vera.ttf");

		this->surface = dm->createSurface(myScreen);
		if (this->surface != NULL) {
			int cap = dm->getSurfaceCap(surface, "ALPHACHANNEL");
			dm->setSurfaceCaps (surface, cap);
		}
	}

	int TextPlayer::write(
			GingaScreenID screenId,
			GingaSurfaceID s,
			string text,
			short textAlign,
			string fontUri,
			int fontSize, IColor* fontColor) {

		if (fontSize < 1 || s == NULL || text == "") {
			return 0;
		}

		GingaProviderID font = 0;
		int width = 0;

		font = dm->createFontProvider(screenId, fontUri.c_str(), fontSize);

		if (fontColor == NULL) {
			fontColor = new Color("black");
		}

		if (font != 0) {
			dm->setSurfaceColor(s,
													fontColor->getR(),
													fontColor->getG(),
													fontColor->getB(),
													fontColor->getAlpha());

			width = dm->getProviderStringWidth(font, text.c_str(),
			                                   strlen((const char*)(text.c_str())));

			dm->playProviderOver(font, s, text.c_str(), 0, 0, textAlign);

			dm->releaseFontProvider(screenId, font);
			font = 0;
		}

		delete fontColor;
		fontColor = NULL;

		return width;
	}

	bool TextPlayer::setFont(string someUri) {
		if (!fileExists(someUri)) {
			clog << "TextPlayer::setFont Warning! File not found: '";
			clog << someUri.c_str() << "'" << endl;
			return false;
		}

		this->fontUri = someUri;
		if (font != 0) {
			dm->releaseFontProvider(myScreen, font);
			font = 0;
		}

		font = dm->createFontProvider(myScreen, someUri.c_str(), fontSize);
		if (font == 0) {
			clog << "TextPlayer::setFont Warning! Can't create Font '";
			clog << someUri << "': '" << font << "'" << endl;
			return false;
		}

		dm->getProviderHeight(font);
		return true;
	}

	void TextPlayer::setFontSize(int size) {
		fontSize = size;
		setFont(fontUri);
	}

	int TextPlayer::getFontSize() {
		return fontSize;
	}

	int TextPlayer::getFontHeight() {
		return fontHeight;
	}

	void TextPlayer::setBgColor(int red, int green, int blue, int alpha) {
   		if (bgColor != NULL) {
   			delete bgColor;
   			bgColor = NULL;
   		}

   		bgColor = new Color(red, green, blue, alpha);
		if (this->surface != NULL) {
			dm->setSurfaceBgColor(surface, red, green, blue, alpha);
		}
	}

	void TextPlayer::setColor(int red, int green, int blue, int alpha) {
		if (this->fontColor != NULL) {
			delete this->fontColor;
			this->fontColor = NULL;
		}

		this->fontColor = new Color(red, green, blue, alpha);
	}

	void TextPlayer::setTabSize(int size) {
		this->tabSize = size;
	}

	int TextPlayer::getTabSize() {
		return tabSize;
	}

	void TextPlayer::drawText(string text, short align) {
		string uri;
		int textWidth, surWidth, surHeight;

		unsigned int maxToDraw;
		string::size_type splitPos, len;
		unsigned int widthAverage;
		bool space;
		int oldTextWidth;
		string splited;

		uri = SystemCompat::appendGingaFilesPrefix("font" + SystemCompat::getIUriD() + "vera.ttf");
		if (font == NULL && fileExists(uri)) {
			setFont(uri);
		}

		if (font == NULL) {
			clog << "TextPlayer::drawText Warning! can't set font" << endl;
			return;
		}

		if (fontColor == NULL) {
			fontColor = new Color("black");
		}

		dm->setSurfaceColor(surface,
		                      fontColor->getR(),
		                      fontColor->getG(),
		                      fontColor->getB(),
		                      fontColor->getAlpha());

		if (font != 0 && surface != NULL) {
			dm->getSurfaceSize(surface, &surWidth, &surHeight);
			textWidth = dm->getProviderStringWidth(
					font, text.c_str(), strlen((const char*)(text.c_str())));

			if (textWidth > surWidth && text.length() > 1) {
				space = false;

				widthAverage = (int)(textWidth / text.length());
				maxToDraw = (int)(((surWidth) / widthAverage) * 0.85);

				len = text.length();
				splited = text.substr(0, maxToDraw);
				splitPos = splited.find_last_of(" ");

				if (splitPos == std::string::npos) {
					splitPos = maxToDraw;
					splited = text.substr(splitPos, len - splitPos);

				} else {
					splitPos++;
					splited = text.substr(splitPos, len - splitPos);
					space = true;
				}

				text = text.substr(0, splitPos);

				textWidth = dm->getProviderStringWidth(font, text.c_str());

				while (textWidth > surWidth) {
					if (space) {
						splited = " " + splited;
					}

					len = text.length();
					splitPos = text.find_last_of(" ");
					if (splitPos == std::string::npos) {
						splited = text[len] + splited;
						text = text.substr(0, len - 1);
						space = false;

					} else {
						splitPos++;
						splited = text.substr(
								splitPos, len - splitPos) + splited;

						text = text.substr(0, splitPos);
						space = true;
					}

					oldTextWidth = textWidth;
					textWidth = dm->getProviderStringWidth(font, text.c_str());

					if (oldTextWidth == textWidth) {
						break;
					}
				}

				dm->playProviderOver(font, surface, text.c_str(), currentColumn,
				                     currentLine, align);

				/*if (align == A_TOP_CENTER) {
					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    (int)(surWidth / 2),
					    currentLine, (DFBSurfaceTextFlags)(align)));

				} else if (align == A_CENTER) {
					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    (int)(surWidth / 2),
					    (int)((surHeight + currentLine) / 2),
					    (DFBSurfaceTextFlags)(align)));

				} else if (align == A_BOTTOM_CENTER) {
					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    (int)(surWidth / 2),
					    (int)((surHeight - currentLine) / 2),
					    (DFBSurfaceTextFlags)(align)));

				} else {
					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    currentColumn,
					    currentLine, (DFBSurfaceTextFlags)(align)));
				}*/

				breakLine();
				drawText(splited, align);

			} else {
				dm->playProviderOver(font, surface, text.c_str(), currentColumn,
				                     currentLine, align);
/*
				if (align == A_TOP_CENTER) {

					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    (int)(surWidth / 2),
					    currentLine, (DFBSurfaceTextFlags)(align)));

				} else if (align == A_CENTER) {
					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    (int)(surWidth / 2),
					    (int)((surHeight + currentLine) / 2),
					    (DFBSurfaceTextFlags)(align)));

				} else if (align == A_BOTTOM_CENTER) {
					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    (int)(surWidth / 2),
					    (int)((surHeight - currentLine) / 2),
					    (DFBSurfaceTextFlags)(align)));

				} else {
					DFBCHECK(surface->getSurface()->DrawString(
					    surface->getSurface(),
					    text.c_str(),
					    -1,
					    currentColumn,
					    currentLine, (DFBSurfaceTextFlags)(align)));
				}
*/
				currentColumn += textWidth;
			}

		} else {
			clog << "TextPlayer::drawText Warning! FontProvider(" << font;
			clog << ") or Surface(" << surface << ") = NULL";
			clog << endl;
		}
	}

	bool TextPlayer::drawTextLn(string text, short align) {
		drawText(text, align);
		return breakLine();
	}

	void TextPlayer::tab() {
		currentColumn = currentColumn + (tabSize * 12);
	}

	bool TextPlayer::breakLine() {
		int w, h;
		if (font == 0) {
			setFont(SystemCompat::appendGingaFilesPrefix("font/decker.ttf"));
		}

		dm->getSurfaceSize(surface, &w, &h);
		if ((currentLine + fontHeight) > h) {
			clog << "TextPlayer::breakLine() Exceeding surface bounds";
			clog << " currentLine = '" << currentLine << "'";
			clog << " fontHeight = '" << fontHeight << "'";
			clog << " surH = '" << h << "'" << endl;

			currentLine   = currentLine + (int)(1.15 * fontHeight);
			currentColumn = 0;
			return false;

		} else {
			currentLine   = currentLine + (int)(1.15 * fontHeight);
			currentColumn = 0;
			return true;
		}
	}

	int TextPlayer::getCurrentColumn() {
		return this->currentColumn;
	}

	int TextPlayer::getCurrentLine() {
		return this->currentLine;
	}

	void TextPlayer::setPropertyValue(string name, string value) {
		//TODO: set font url, font size, font color, ...
		Player::setPropertyValue(name, value);
	}
}
}
}
}
}
}
