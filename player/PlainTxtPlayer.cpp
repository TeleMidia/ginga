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

#include "util/Color.h"

#include "PlainTxtPlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	PlainTxtPlayer::PlainTxtPlayer(GingaScreenID screenId, string mrl) :
			TextPlayer(screenId) {

		Thread::mutexInit(&mutex, NULL);
		content = "";
	}

	PlainTxtPlayer::~PlainTxtPlayer() {
		Thread::mutexLock(&mutex);
		if (dm->hasSurface(myScreen, surface)) {
			/*
			 * the surface could never be a child of window
			 * (it gets the widget surface)
			 */
			dm->setSurfaceParentWindow(myScreen, surface, 0);
		}
		Thread::mutexUnlock(&mutex);
		Thread::mutexDestroy(&mutex);
   	}

	void PlainTxtPlayer::setFile(string mrl) {
		if (mrl == "" || !fileExists(mrl)) {
			clog << "PlainTxtPlayer::setFile Warning! File not found: '";
			clog << mrl << "'" << endl;
			return;
		}

		if (mrl.length() > 4) {
			string fileType;

			this->mrl = mrl;
			fileType = this->mrl.substr(this->mrl.length() - 4, 4);
			if (fileType != ".txt") {
				clog << "PlainTxtPlayer::loadFile Warning! Unknown file ";
				clog << "type for: '" << this->mrl << "'" << endl;
			}

		} else {
			clog << "PlainTxtPlayer::loadFile Warning! Unknown extension ";
			clog << "type for: '" << mrl << "'" << endl;
		}
	}

	void PlainTxtPlayer::loadTxt() {
		ifstream fis;
		string line, aux;
		int surfaceW, surfaceH;

//		if (surface != NULL) {
//			surface->clearSurface();
//		}

		Thread::mutexLock(&mutex);
		fis.open((this->mrl).c_str(), ifstream::in);
		if (!fis.is_open() && (mrl != "" || content == "")) {
			clog << "PlainTxtPlayer::loadFile Warning! can't open input ";
			clog << "file: '" << this->mrl << "'" << endl;
			Thread::mutexUnlock(&mutex);
			return;
		}

		if (fontColor == NULL) {
			fontColor = new Color(255, 255, 255, 255);
		}

		if (surface != 0 && dm->getSurfaceParentWindow(surface) != 0) {
			if (bgColor != NULL) {
				//this->surface->setCaps(0);
				dm->clearSurfaceContent(surface);
				dm->setWindowBgColor(myScreen, dm->getSurfaceParentWindow(surface),
														bgColor->getR(),
														bgColor->getG(),
														bgColor->getB(),
														bgColor->getAlpha());

			} else {
				GingaWindowID parentWindow = dm->getSurfaceParentWindow(surface);
				dm->clearWindowContent(myScreen, parentWindow);
			}
		}

		this->currentLine = 0;
		this->currentColumn = 0;

		if (mrl != "" && content == "") {
			dm->getSurfaceSize(surface, &surfaceW, &surfaceH);
			while (!fis.eof() && fis.good() &&
					(currentLine + fontHeight) < surfaceH) {

				getline(fis, line);
				if (line != "") {
					if (!drawTextLn(line, currentAlign)) {
						break;
					}
				}
			}

		} else if (content != "") {
			drawTextLn(content, currentAlign);
		}

		if (surface != 0 && dm->getSurfaceParentWindow (surface) != 0) {
			GingaWindowID parentWin = dm->getSurfaceParentWindow(surface);
			dm->validateWindow(myScreen, parentWin);
		}

		fis.close();
		Thread::mutexUnlock(&mutex);
	}

	bool PlainTxtPlayer::play() {
		if (surface != 0) {
			clog << "PlainTxtPlayer::play ok" << endl;
			loadTxt();
			return Player::play();

		} else {
			clog << "PlainTxtPlayer::play warning" << endl;
			return false;
		}
	}

	void PlainTxtPlayer::stop() {
		Player::stop();
	}

	void PlainTxtPlayer::setContent(string content) {
		Thread::mutexLock(&mutex);

		GingaWindowID parentWindow = dm->getSurfaceParentWindow(surface);
		if (surface != 0 &&  parentWindow != 0) {
//			surface->clearSurface();

			if (bgColor != NULL) {
				//this->surface->setCaps(0);
				dm->clearSurfaceContent(surface);
				dm->setWindowBgColor (myScreen, parentWindow,
															bgColor->getR(),
															bgColor->getG(),
															bgColor->getB(),
															bgColor->getAlpha());

			} else {
				dm->clearWindowContent(myScreen, parentWindow);
			}
		}

		this->currentLine = 0;
		this->currentColumn = 0;
		this->content = content;

		if (content != "") {
			drawTextLn(this->content, currentAlign);
			mrl = "";
		}

		if (surface != 0 && parentWindow != 0) {
			dm->validateWindow(myScreen, parentWindow);
		}

		Thread::mutexUnlock(&mutex);
	}

	void PlainTxtPlayer::setTextAlign(string align) {
		if (align == "left") {
			currentAlign = IFontProvider::FP_TA_LEFT;

		} else if (align == "right") {
			currentAlign = IFontProvider::FP_TA_RIGHT;

		} else if (align == "center") {
			currentAlign = IFontProvider::FP_TA_CENTER;
		}
	}

	void PlainTxtPlayer::setPropertyValue(string name, string value) {
		Thread::mutexLock(&mutex);

		vector<string>* params;
		bool refresh = true;

		/*
		clog << "PlainTxtPlayer::setPropertyValue name = '" << name.c_str();
		clog << "' value = '" << value.c_str() << "'" << endl;
		*/

		if (value == "") {
			Thread::mutexUnlock(&mutex);
			return;
		}

		if (surface == 0) {
			refresh = false;
		}

		if (name == "fontColor") {
			if (fontColor != NULL) {
				delete fontColor;
				fontColor = NULL;
			}

			fontColor = new Color(value);

		} else if (name == "fontSize" && isNumeric((void*)(value.c_str()))) {

			setFontSize((int)(util::stof(value)));

		} else if (name == "fontUri") {
			setFont(value);

		} else if (name == "textAlign") {
			setTextAlign(value);

		} else if (name == "fontStyle") {
			string styleName = "text-align";
			size_t pos = value.find(styleName);
			string strAlign = "";
			if (pos != std::string::npos) {
				strAlign = value.substr(pos + styleName.length() + 1);
				setTextAlign(strAlign);
			}

		} else if ((name == "x-bgColor" || name == "bgColor")) {
			if (surface != 0) {
				if (bgColor != NULL) {
					delete bgColor;
					bgColor = NULL;
				}

				bgColor = new Color(value);

				GingaWindowID parentWindow = dm->getSurfaceParentWindow(surface);
				if (parentWindow != 0) {
					//this->surface->setCaps(0);
					dm->clearSurfaceContent(surface);
					dm->setWindowBgColor(myScreen, parentWindow,
															bgColor->getR(),
															bgColor->getG(),
															bgColor->getB(),
															bgColor->getAlpha());
				}
			}

		} else if (name == "x-rgbBgColor" || name == "rgbBgColor") {
			params = split(value, ",");
			if (params->size() == 3) {
				if (surface != 0) {
					if (bgColor != NULL) {
						delete bgColor;
						bgColor = NULL;
					}

					bgColor = new Color(
							(int)util::stof((*params)[0]),
							(int)util::stof((*params)[1]),
							(int)util::stof((*params)[2]));

					GingaWindowID parentWindow = dm->getSurfaceParentWindow(surface);
					if (parentWindow != 0) {
						//this->surface->setCaps(0);
						dm->clearSurfaceContent(surface);
						dm->setWindowBgColor(myScreen, parentWindow,
																bgColor->getR(),
																bgColor->getG(),
																bgColor->getB(),
																bgColor->getAlpha());
					}
				}

			} else {
				refresh = false;
			}

			delete params;
			params = NULL;

		} else if (name == "x-rgbFontColor" || name == "rgbFontColor") {
			params = split(value, ",");
			if (params->size() == 3) {
				if (fontColor != NULL) {
					delete fontColor;
					fontColor = NULL;
				}

				fontColor = new Color(
						(int)util::stof((*params)[0]),
						(int)util::stof((*params)[1]),
						(int)util::stof((*params)[2]));

			} else {
				refresh = false;
			}

			delete params;
			params = NULL;

		} else if (name == "x-content") {
			Thread::mutexUnlock(&mutex);
			setContent(value);
			Thread::mutexLock(&mutex);
			refresh = false;

		} else if (name == "x-setFile") {
			setFile(value);
			refresh = false;
		}

		Player::setPropertyValue(name, value);
		Thread::mutexUnlock(&mutex);

		// refreshing changes
		if (refresh) {
			loadTxt();
			if (notifyContentUpdate) {
				notifyPlayerListeners(
						PL_NOTIFY_UPDATECONTENT,
						"",
						TYPE_PASSIVEDEVICE,
						"");
			}
		}
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createPlainTextPlayer(
				GingaScreenID screenId, const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::player::
			PlainTxtPlayer(screenId, (string)mrl));
}

extern "C" void destroyPlainTextPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
