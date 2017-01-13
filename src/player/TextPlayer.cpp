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

#include "config.h"
#include "util/Color.h"
#include "mb/IFontProvider.h"
#include "mb/SDLSurface.h"

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
		if (outputWindow != 0 && dm->hasWindow(myScreen, outputWindow)) {
			dm->revertWindowContent(myScreen, outputWindow);
			dm->deleteWindow(myScreen, outputWindow);

			outputWindow = 0;
		}

		if (surface != 0 && dm->hasSurface(myScreen, surface)) {
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
		if (this->surface != 0) {
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
			int fontSize, Color* fontColor) {

		if (fontSize < 1 || s == 0 || text == "") {
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

		fontHeight = dm->getProviderHeight(font);
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
		if (this->surface != 0) {
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
		string aux, splited;

		aux = text;
		uri = SystemCompat::appendGingaFilesPrefix("font" + SystemCompat::getIUriD() + "vera.ttf");
		if (font == 0 && fileExists(uri)) {
			setFont(uri);
		}

		if (font == 0) {
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

		if (font != 0 && surface != 0) {
			dm->getSurfaceSize(surface, &surWidth, &surHeight);
			textWidth = dm->getProviderStringWidth(
					font, aux.c_str(), strlen((const char*)(aux.c_str())));

			if (textWidth > surWidth && aux.length() > 1) {
				space = false;

				widthAverage = (int)(textWidth / aux.length());
				maxToDraw = (int)(((surWidth) / widthAverage) * 0.85);

				len      = aux.length();
				splited  = aux.substr(0, maxToDraw);
				splitPos = splited.find_last_of(" ");

				if (splitPos == std::string::npos) {
					splitPos = maxToDraw;
					splited = aux.substr(splitPos, len - splitPos);

				} else {
					splitPos++;
					splited = aux.substr(splitPos, len - splitPos);
					space = true;
				}

				aux = aux.substr(0, splitPos);

				textWidth = dm->getProviderStringWidth(font, aux.c_str());

				while (textWidth > surWidth) {
					if (space) {
						splited = " " + splited;
					}

					len      = aux.length();
					splitPos = aux.find_last_of(" ");

					if (splitPos == std::string::npos) {
						splited = aux[len] + splited;
						aux     = aux.substr(0, len - 1);
						space   = false;

					} else {
						splitPos++;
						splited = aux.substr(
								splitPos, len - splitPos) + splited;

						aux   = aux.substr(0, splitPos);
						space = true;
					}

					oldTextWidth = textWidth;
					textWidth = dm->getProviderStringWidth(font, aux.c_str());

					if (oldTextWidth == textWidth) {
						break;
					}
				}

				dm->playProviderOver(
						font, surface, aux.c_str(), currentColumn, currentLine, align);

				breakLine();
				if (splited != text) {
					drawText(splited, align);
				}

			} else {
				dm->playProviderOver(
						font, surface, aux.c_str(), currentColumn, currentLine, align);

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
