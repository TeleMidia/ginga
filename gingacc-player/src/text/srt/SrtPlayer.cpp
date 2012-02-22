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

#include "player/SrtPlayer.h"

#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	SrtPlayer::SrtPlayer(GingaScreenID screenId, string mrl) :
			TextPlayer(screenId), Thread() {

		this->mrl = mrl;
		this->textEvents = NULL;
		this->textLines = NULL;
		this->player = NULL;
		this->isPlaying = false;
		this->controlParentVisibility = false;
		this->running = false;
	}

	SrtPlayer::~SrtPlayer() {
		clog << "SrtPlayer::~SrtPlayer" << endl;
		lock();
		/*
		 * TODO: is some class deleting this
		 * if (renderSurface != NULL) {
			delete renderSurface;
			renderSurface = NULL;
		}*/

		if (textEvents != NULL) {
			delete textEvents;
			textEvents = NULL;
		}

		if (textLines != NULL) {
			delete textLines;
			textLines = NULL;
		}
   	}

	void SrtPlayer::loadFile(string mrl) {
		this->mrl = mrl;

		if (this->mrl == "" || !fileExists(this->mrl)) {
			clog << "SrtPlayer::loadFile Warning! File not found: '";
			clog << this->mrl.c_str() << "'" << endl;
			return;
		}

		if (this->mrl.length() > 4) {
			string fileType;
			fileType = this->mrl.substr(this->mrl.length() - 4, 4);
			if (fileType == ".srt") {
				loadSrt();

			} else {
				clog << "SrtPlayer::loadFile Warning! Unknown file type: '";
				clog << this->mrl.c_str() << "'" << endl;
			}

		} else {
			clog << "SrtPlayer::loadFile Warning! Unknown extension type: '";
			clog << this->mrl.c_str() << "'" << endl;
		}
	}

	void SrtPlayer::loadSrt() {
		clog << "SrtPlayer::loadSrt " << endl;
		ifstream fisSub;
		string line, key, value, text;

		fisSub.open((this->mrl).c_str(), ifstream::in);
		if (!fisSub.is_open()) {
			clog << "SrtPlayer::loadFile Warning! can't open input file: '";
			clog << this->mrl.c_str() << "'" << endl;
			return;
		}

		if (textEvents != NULL) {
			textEvents->clear();
			delete textEvents;
			textEvents = NULL;
		}
		textEvents = new map<float, float>;

		if (textLines != NULL) {
			textLines->clear();
			delete textLines;
			textLines = NULL;
		}
		textLines = new vector<string>;

		int i = 1;
		while (!fisSub.eof()) {
			getline(fisSub, line);

			if (line.find('\r') != std::string::npos) {
				line = line.substr(0, line.find('\r'));
			}
//			clog << "line = '" << line << "'" << endl;
			if (trim(line) == itos(i)) {
				while (line.find("-->") == std::string::npos &&
					    !fisSub.eof()) {

					getline(fisSub, line);
				}

				if (line.find('\r') != std::string::npos) {
					line = line.substr(0, line.find('\r'));
				}

				key = trim(line.substr(0, line.find("-->")));

				value = trim(line.substr((line.find("-->") + 3),
					    line.length()));

//				clog << "key = '" << key << "'" << endl;
//				clog << "value = '" << value << "'" << endl;

				(*textEvents)[strTimeToFloat(key)] = strTimeToFloat(value);

				text = "";
				while (trim(line) != "" && !fisSub.eof()) {
					getline(fisSub, line);
					if (line.find('\r') != std::string::npos) {
						line = line.substr(0, line.find('\r'));
					}
					if (line != "") {
						if (text != "") {
							text = text + "||";
						}
						text = text + line;
					}
				}

//				clog << " text = '" << text << "'" << endl;
				textLines->push_back(text);
				i++;
			}
		}

		fisSub.close();
	}

	float SrtPlayer::strTimeToFloat(string time) {
		if (time == "" || time.find(":") == std::string::npos) {
			return 0;
		}

		float hours, minutes, seconds, milliseconds;

		hours = util::stof(time.substr(0, time.find_first_of(":")));
		time = time.substr(time.find_first_of(":") + 1, time.length());
		minutes = util::stof(time.substr(0, time.find_first_of(":")));
		time = time.substr(time.find_first_of(":") + 1, time.length());
		seconds = util::stof(time.substr(0, time.find_first_of(",")));
		time = time.substr(time.find_first_of(",") + 1, time.length());
		milliseconds = util::stof(time);

		return (float)(
			    (hours * 3600) +
			    (minutes * 60) +
			    (seconds) +
			    (milliseconds / 1000));
	}

	void SrtPlayer::printSrt() {
		if (textEvents == NULL || textLines == NULL) {
			clog << "SrtPlayer::printSrt nothing to print" << endl;
			return;
		}

		map<float, float>::iterator i;
		vector<string>::iterator j;

		j = textLines->begin();
		for (i = textEvents->begin(); i != textEvents->end(); ++i) {
			j++;
			if (j == textLines->end()) {
				return;
			}
		}
	}

	void SrtPlayer::setParentVisibilityControl(bool control) {
		this->controlParentVisibility = control;
	}

	void SrtPlayer::setReferenceTimePlayer(IPlayer* player) {
		lock();
		this->player = player;
		unlock();
	}

	double SrtPlayer::getMediaTotalTime() {
		if (player != NULL && textEvents != NULL && !textEvents->empty()) {
			map<float, float>::iterator i;
			i = textEvents->end();
			--i;
			return (double)i->second;
		}
		return 0;
	}

	double SrtPlayer::getMediaTime() {
		if (this->player != NULL) {
			return this->player->getMediaTime();
		}
		return 0;
	}

	void SrtPlayer::play() {
		Player::play();
		if (!isPlaying) {
			if (player == NULL) {
				clog << "SrtPlayer::play ";
				clog << "warning! referenceTimePlayer is NULL!!" << endl;
			}
			isPlaying = true;
			Thread::start();
		}
	}

	void SrtPlayer::stop() {
		clog << "SrtPlayer::stop()" << endl;
		Player::stop();
		isPlaying = false;
		wakeUp();
	}

	void SrtPlayer::pause() {
		Player::pause();
		stop();
	}

	void SrtPlayer::resume() {
		Player::resume();
		if (!isPlaying) {
			if (player == NULL) {
				clog << " warning! referenceTimePlayer is NULL!!" << endl;
			}
			isPlaying = true;
			Thread::start();
		}
	}

	void SrtPlayer::setPropertyValue(string name, string value) {
		if (name == "x-setFontUri") {
		    setFont(value);

		} else if (name == "x-setFontSize") {
		    setFontSize((int)util::stof(value));

		} else if (name == "x-controlVisibility") {
			if (value == "true") {
				setParentVisibilityControl(true);
			} else {
				setParentVisibilityControl(false);
			}

		} else if (name == "x-setFontColor") {
			Color* fontColor = new Color(value);
			setColor(fontColor->getR(),
	    		    fontColor->getG(),
	    		    fontColor->getB(),
	    		    255);

			delete fontColor;

		} else if (name == "x-setRGBFontColor") {
			setColor(191, 191, 0, 255);

		} else if (name == "loadFile") {
			loadFile(value);
		}

		//TODO: set font url, font size, font color, ...
		Player::setPropertyValue(name, value);
	}

	void SrtPlayer::busy() {
		clog << "SrtPlayer::busy()" << endl;
		if (running) {
			this->waitForUnlockCondition();
		}
	}

	bool SrtPlayer::isPlayingSrt() {
		if (isPlaying) {
			return true;
		}
		lock();
		if (surface != NULL) {
			surface->clearContent();
		}
		unlock();
		return false;
	}

	void SrtPlayer::run() {
		clog << "SrtPlayer::run" << endl;
		running = true;
		lock();
		IWindow* parent = NULL;
		if (surface != NULL) {
			parent = (IWindow*)(surface->getParent());

		} else {
			clog << "SrtPlayer::run warning! surface == NULL" << endl;
		}
		unlock();

		if (player != NULL && parent != NULL) {
			float mediaTime, hide, show;
			int sleepTime;
			map<float, float>* events;
			map<float, float>::iterator i, j;

			vector<string>* text;
			vector<string>::iterator k;
			string line;

			events = new map<float,float>(*textEvents);
			text = new vector<string>(*textLines);

			mediaTime = 0;
			hide = 0;
			show = 0;
			sleepTime = 0;
			line = "";

			while (isPlayingSrt() && !events->empty() && !text->empty()) {

				i = events->begin();
				k = text->begin();

				while (i->first < mediaTime) {
					mediaTime = (float)(player->getMediaTime());
					events->erase(i);
					text->erase(k);
					if (events->empty() || text->empty()) {
						break;
					}

					i = events->begin();
					k = text->begin();
				}

				if (events->empty() || text->empty()) {
					break;
				}

				if (!isPlayingSrt()) {
					break;
				}

				show = i->first;
				hide = i->second;
				line = *k;

				events->erase(i);
				text->erase(k);

				if (player == NULL) {
					isPlaying = false;
					break;
				}

				mediaTime = (float)(player->getMediaTime());
				sleepTime = (int)(((show - mediaTime) * 1000000) - 70000);
//				clog << "show = '" << show << "' mediaTime = '" << mediaTime;
//				clog << "' sleepTime = '" << sleepTime << "'" << endl;
				if (sleepTime > 0) {
					Thread::usleep(sleepTime / 1000);
				}

				if (!isPlayingSrt()) {
					break;
				}

				while (line.find("||") != std::string::npos) {
					drawTextLn(
						    line.substr(0, line.find("||")),
						    1);

					line = line.substr(line.find("||") + 2, line.length());
				}

				if (line != "") {
					drawText(
							line.substr(0, line.find("||")),
							1);
				}

				if (!isPlayingSrt()) {
					break;
				}

				lock();
				if (surface != NULL) {
					parent = (IWindow*)(surface->getParent());
					if (parent != NULL && isPlayingSrt()) {
						if (controlParentVisibility) {
							clog << "SrtPlayer::run show";
							clog << "LINE: '" << line.c_str() << "'" << endl;
							parent->show();
						}

					} else {
						clog << "SrtPlayer::run warning! ";
						clog << "cant render, NULL parent" << endl;
					}

				} else {
					clog << "SrtPlayer::run warning! ";
					clog << "cant render, NULL surface" << endl;
				}
				unlock();

				if (player == NULL) {
					isPlaying = false;
					break;
				}

				if (notifyContentUpdate) {
					notifyPlayerListeners(
							PL_NOTIFY_UPDATECONTENT,
							"",
							TYPE_PASSIVEDEVICE,
							"");
				}

				mediaTime = (float)(player->getMediaTime());
				sleepTime = (int)(((hide - mediaTime) * 1000000) - 100000);
				if (sleepTime > 0) {
					Thread::usleep(sleepTime / 1000);
				}

				lock();
				if (surface != NULL) {
					surface->clearSurface();
					parent = (IWindow*)(surface->getParent());
					if (parent != NULL && controlParentVisibility) {
						parent->hide();
					}
				}
				unlock();

				if (notifyContentUpdate) {
					notifyPlayerListeners(
							PL_NOTIFY_UPDATECONTENT,
							"",
							TYPE_PASSIVEDEVICE,
							"");
				}

				this->currentLine = 0;
				this->currentColumn = 0;
			}

			events->clear();
			delete events;
			events = NULL;

			text->clear();
			delete text;
			text = NULL;
		}

		if (isPlayingSrt()) { //natural end
			clog << "SrtPlayer::run natural end" << endl;
			isPlaying = false;
			notifyPlayerListeners(
					PL_NOTIFY_STOP,
					"",
					IPlayer::TYPE_PRESENTATION,
					"");

		} else {
			clog << "SrtPlayer::run stoped" << endl;
		}

		clog << "SrtPlayer::run subtitle done!" << endl;
		running = false;
		this->unlockConditionSatisfied();
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createSrtPlayer(
				GingaScreenID screenId, const char* mrl, bool hasVisual) {

	return (new ::br::pucrio::telemidia::ginga::core::player::
			SrtPlayer(screenId, (string)mrl));
}

extern "C" void destroySrtPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
