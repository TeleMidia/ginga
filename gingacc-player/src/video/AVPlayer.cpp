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

#include "player/AVPlayer.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <sys/types.h>
#include <stdio.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	AVPlayer::AVPlayer(GingaScreenID screenId, string mrl) :
			Thread(), Player(screenId, mrl) {

		string::size_type pos;

		this->provider    = 0;
		this->mainAV      = false;
		this->buffered    = false;
		this->isRemote    = false;

		this->status      = STOP;
		this->running     = false;
		this->soundLevel  = 1.0;
		this->win         = 0;
		this->pSym        = "";

		Thread::mutexInit(&pMutex, true);
		Thread::mutexInit(&tMutex, true);

#if HAVE_ICRTP & !HAVE_XINEPROVIDER
		this->icListener  = NULL;
#endif

		if (mrl.length() > 11 && mrl.substr(0, 11) == "sbtvd-ts://") {
			this->mainAV = true;
			pos = mrl.find("#");
			if (pos != std::string::npos) {
				this->mrl = mrl.substr(pos + 1, mrl.length() - (pos + 1));

			} else {
				this->mrl = mrl.substr(11, mrl.length() - 11);
			}
			clog << "AVPlayer::AVPlayer MAINAV CREATED MRL = '";
			clog << this->mrl << "'" << endl;

#if HAVE_ICRTP & !HAVE_XINEPROVIDER
		} else if (mrl.length() > 6 && mrl.substr(0, 6) == "rtp://") {
			clog << "AVPlayer::AVPlayer creating RTP IC " << endl;

			icListener = new RTPListener(mrl);
			this->mrl  = icListener->getUrl();

			Thread::startThread();

			pthread_t _tId;
			pthread_create(&_tId, NULL, createProviderT, this);
			pthread_detach(_tId);
#endif
		} else {
			createProvider();
			this->scopeEndTime = getTotalMediaTime();
		}
	}

	AVPlayer::~AVPlayer() {
		if (status != STOP) {
			stop();
		}

		unlockConditionSatisfied();

		if (running) {
			wakeUp();
		}

		Thread::mutexLock(&tMutex);
		if (surface != 0 && mainAV) {
			dm->setSurfaceParentWindow(myScreen, surface, 0);
		}

		if (mainAV) {
			if (win != 0) {
				dm->disposeWindow (myScreen, win);
				win = 0;
			}
		}

		Thread::mutexLock(&pMutex);
		if (this->provider != 0) {
			release();
		}

		Thread::mutexUnlock(&pMutex);
		Thread::mutexDestroy(&pMutex);

		Thread::mutexUnlock(&tMutex);
		Thread::mutexDestroy(&tMutex);
	}

	GingaSurfaceID AVPlayer::getSurface() {
		if (provider == 0) {
			createProvider();
			if (provider == 0) {
				clog << "AVPlayer::getSurface() can't create provider" << endl;
			}
		}

		return Player::getSurface();
	}

	void AVPlayer::createProvider(void) {
		Thread::mutexLock(&pMutex);
		clog << "AVPlayer::createProvider '" << mrl << "'" << endl;

		if (mrl.substr(0, 7) == "rtsp://" ||
				mrl.substr(0, 6) == "rtp://" ||
				mrl.substr(0, 7) == "http://" ||
				mrl.substr(0, 8) == "https://") {

			isRemote = true;
		}

		if (provider == 0 && (fileExists(mrl) || isRemote)) {
			provider = dm->createContinuousMediaProvider(
					myScreen, mrl.c_str(), isRemote);

			surface = createFrame();
		}

		Thread::mutexUnlock(&(pMutex));
		clog << "AVPlayer::createProvider '" << mrl << "' all done" << endl;
	}

	GingaProviderID AVPlayer::createProviderT(void* ptr) {
		AVPlayer* p = (AVPlayer*)ptr;

		p->createProvider();

		return p->provider;
	}

	void AVPlayer::finished() {
		clog << "AVPlayer::finished" << endl;
	}

	double AVPlayer::getEndTime() {
		return scopeEndTime;
	}

	void AVPlayer::initializeAudio(int numArgs, char* args[]) {

	}

	void AVPlayer::releaseAudio() {

	}

	void AVPlayer::setSoundLevel(float level) {
		//clog << "AVPlayer::setSoundLevel()" << endl;
		//lock()();
		this->soundLevel = level;
		if (provider != 0) {
			dm->setProviderSoundLevel(provider, soundLevel);
		}
		//unlock();
	}

	GingaSurfaceID AVPlayer::createFrame() {
		//clog << "AVPlayer::createFrame()" << endl;

		Thread::mutexLock(&tMutex);
		if (surface != 0) {
			clog << "AVPlayer::createFrame Warning! surface != NULL";
			clog << endl;
			if (mainAV) {
				dm->setSurfaceParentWindow(myScreen, surface, 0);
			}
			dm->deleteSurface(surface);
		}

		surface = dm->createSurface(myScreen);
		if (win != 0 && mainAV) {
			dm->setSurfaceParentWindow(myScreen, surface, win);
		}

		Thread::mutexUnlock(&tMutex);

		return surface;
	}

	void AVPlayer::getOriginalResolution(int* width, int* height) {
		if (provider != 0) {
			dm->getProviderOriginalResolution(provider, height, width);
		}
	}

	double AVPlayer::getTotalMediaTime() {
		if (provider != 0) {
			return dm->getProviderTotalMediaTime(provider);
		}

		this->wakeUp();
		return 0;
	}

	int64_t AVPlayer::getVPts() {
		if (provider == 0) {
			return 0;
		}

		return dm->getProviderVPts(provider);
	}

	void AVPlayer::timeShift(string direction) {
		if (provider != 0) {
			if (direction == "forward") {
				dm->setProviderMediaTime(provider,
				                         dm->getProviderMediaTime(provider) + 10);

			} else if (direction == "backward") {
				dm->setProviderMediaTime(provider,
				                         dm->getProviderMediaTime(provider) - 10);
			}
		}
	}

	double AVPlayer::getCurrentMediaTime() {
		if (provider == 0) {
			clog << "AVPlayer::getCurrentMediaTime returning -1";
			clog << " cause provider is NULL";
			clog << endl;
			this->wakeUp();
			return -1;
		}

		return dm->getProviderMediaTime(provider);
	}

	double AVPlayer::getMediaTime() {
		return getCurrentMediaTime();
	}

	void AVPlayer::setMediaTime(double pos) {
		if (status == PLAY) {
			status = PAUSE;
			this->wakeUp();
			dm->setProviderMediaTime(provider, pos);
			status = PLAY;
			running = true;
			Thread::startThread();

		} else if (provider != 0) {
			dm->setProviderMediaTime(provider, pos);
		}
	}

	void AVPlayer::setStopTime(double pos) {
		if (status == PLAY) {
			status = PAUSE;
			this->wakeUp();
			scopeEndTime = pos;
			status = PLAY;
			running = true;
			Thread::startThread();

		} else {
			scopeEndTime = pos;
		}
	}

	double AVPlayer::getStopTime() {
	     return scopeEndTime;
	}

	void AVPlayer::setScope(
			string scope,
			short type,
			double begin, double end, double outTransDur) {

		Player::setScope(scope, type, begin, end, outTransDur);
		if (type == TYPE_PRESENTATION) {
			if (scopeInitTime > 0) {
				setMediaTime(scopeInitTime);
			}

			if (scopeEndTime > 0) {
				setStopTime(scopeEndTime);
			}
		}
	}

	bool AVPlayer::play() {
		if (provider == 0) {
			clog << "AVPlayer::play() can't play, provider is NULL" << endl;
			this->wakeUp();
			return false;
		}

		Player::play();
		clog << "AVPlayer::play() calling provider play over" << endl;
		dm->playProviderOver(provider, surface);

		if (!running) {
			running = true;
			Thread::startThread();
		}

		clog << "AVPlayer::play("<< mrl << ") all done!" << endl;
		return true;
	}

	void AVPlayer::pause() {
		status = PAUSE;
		if (provider == 0) {
			this->wakeUp();
			return;
		}

		if (outputWindow != 0) {
			dm->validateWindow(myScreen, outputWindow);
		}
		dm->pauseProvider(provider);
		this->wakeUp();

		clog << "AVPlayer::pause("<< mrl << ") all done!" << endl;
	}

	void AVPlayer::stop() {
		short previousStatus = status;

		Player::stop();
#if HAVE_ICRTP & !HAVE_XINEPROVIDER
		if (icListener != NULL) {
			icListener->releaseIC();
		}
#endif
		if (provider == 0) {
			this->wakeUp();
			return;
		}

		if (previousStatus != STOP) {
			dm->stopProvider(provider);
			this->wakeUp();
		}

		clog << "AVPlayer::stop("<< mrl << ") all done!" << endl;
	}

	void AVPlayer::resume() {
		setSoundLevel(soundLevel);

		Player::play();
		dm->resumeProvider(provider, surface);

		if (!running) {
			running = true;
			Thread::startThread();
		}
	}

	void AVPlayer::setPropertyValue(string name, string value) {
		float fValue = 1.0;
		vector<string>* vals;

		/*clog << "AVPlayer::setPropertyValue '" << name << "' ";
		clog << "= '" << value << "'" << endl;*/

		//TODO: animation, set volume, brightness, ...
		if (name == "soundLevel") {

			if (isPercentualValue(value)) {
				value = itos(getPercentualValue(value) / 100);
			}

			if (value != "") {
				fValue = util::stof(value);
			}
			setSoundLevel(fValue);

		} else if (mainAV) {
			if (name == "createWindow") {
				vals = split(value, ",");
				if (vals->size() == 4) {
					win = dm->createWindow(
							myScreen,
							util::stof((*vals)[0]),
							util::stof((*vals)[1]),
							util::stof((*vals)[2]),
							util::stof((*vals)[3]),
							1.0);

					int caps = dm->getWindowCap (myScreen, win, "NOSTRUCTURE") |
										dm->getWindowCap (myScreen, win, "DOUBLEBUFFER");
					dm->setWindowCaps (myScreen, win, caps);
					dm->drawWindow (myScreen, win);
				}

				delete vals;

				if (!running) {
					Thread::startThread();
				}

			} else if (name == "bounds" && win != 0) {
				vals = split(value, ",");
				if (vals->size() == 4) {
					dm->setWindowBounds (myScreen, win,
															util::stof((*vals)[0]),
															util::stof((*vals)[1]),
															util::stof((*vals)[2]),
															util::stof((*vals)[3]));
				}
				delete vals;

			} else if (name == "show" && win != 0) {
				dm->showWindow(myScreen, win);

			} else if (name == "hide" && win != 0) {
				dm->hideWindow(myScreen, win);
			}
		}

		Player::setPropertyValue(name, value);
	}

	void AVPlayer::addListener(IPlayerListener* listener) {
		Player::addListener(listener);
	}

	void AVPlayer::release() {
		dm->releaseContinuousMediaProvider(myScreen, provider);
		provider = 0;

#if HAVE_ICRTP & !HAVE_XINEPROVIDER
		if (icListener != NULL) {
			delete icListener;
			icListener = NULL;
		}
#endif
	}

	void AVPlayer::setMrl(const char* mrl) {
		this->mrl = mrl;
	}

	string AVPlayer::getMrl() {
		return mrl;
	}

	bool AVPlayer::isPlaying() {
		if (provider == 0) {
			return false;
		}

		if ((getCurrentMediaTime() <= 0 && status != PAUSE) ||
			    (getCurrentMediaTime() >= getStopTime() && getStopTime() > 0)) {

			return false;

		} else {
			return true;
		}
	}

	bool AVPlayer::setOutWindow(GingaWindowID windowId) {
		if (mainAV && win == 0) {
			win = windowId;

			if (!running) {
				Thread::startThread();
			}
			return true;

		} else {
			Player::setOutWindow(windowId);
		}
		return false;
	}

	void AVPlayer::setAVPid(int aPid, int vPid) {
		while (!buffered) {
			SystemCompat::uSleep(150000);
		}
		dm->setProviderAVPid(provider, aPid, vPid);
	}

	void AVPlayer::setAlphaBlend(int x, int y, int w, int h) {
		// only for geode
	}

	void AVPlayer::checkResize() {
		// only for geode
	}

	bool AVPlayer::isRunning() {
		return running;
	}

	bool AVPlayer::checkVideoResizeEvent() {
		bool hasEvent = false;

		if (mainAV) {
			dm->feedProviderBuffers(provider);
		} else {
			SystemCompat::uSleep(150000);
		}
		hasEvent = dm->checkProviderVideoResizeEvent(provider, surface);
		setSoundLevel(this->soundLevel);
		if (hasEvent) {
			dm->playProviderOver(provider, surface);
		}

		return hasEvent;
	}

	void AVPlayer::run() {
		double dur;
		double currentTime;
		double lastCurrentTime = -1;
		double timeRemain;
		double totalTime;

		clog << "AVPlayer::run" << endl;

		Thread::mutexLock(&tMutex);

#if HAVE_ICRTP & !HAVE_XINEPROVIDER
		if (icListener != NULL) {
			running = true;

			Thread::mutexUnlock(&tMutex);

			clog << "AVPlayer::run call performIC" << endl;
			icListener->performIC();
			clog << "AVPlayer::run call performIC done" << endl;
			if (status != STOP && status != PAUSE) {
				status  = STOP;
				running = false;
				notifyPlayerListeners(PL_NOTIFY_STOP, "");
			}

			return;
		}
#endif

		if (mainAV) {
			running = true;

			this->provider = dm->createContinuousMediaProvider(
					myScreen, mrl.c_str(), true);

			this->surface = createFrame();

			if (this->win != 0 && dm->getSurfaceParentWindow(surface) == 0) {
				dm->setSurfaceParentWindow(myScreen, surface, win);
			}
			dm->playProviderOver(provider, surface);
			checkVideoResizeEvent();
			buffered = true;

			//notifying GingaGUI

			// cmd means a command to an embeddor process
			// 0 means successful status message
			// 
			cout << "cmd::0::tuned::?mAV?" << endl;
			waitForUnlockCondition();

		} else if (isRemote) {
			waitForUnlockCondition();

		} else {
			totalTime = getTotalMediaTime();
			if (!isInfinity(scopeEndTime) &&
					scopeEndTime > 0 && scopeEndTime < totalTime) {

				dur = getStopTime();

			} else {
				dur = totalTime;
			}

			clog << "AVPlayer::run total media time that will be considered ";
			clog << " is '" << dur << "'";
			clog << endl;

			if (isInfinity(dur)) {
				clog << "AVPlayer::run duration is INF";
				clog << " => returning" << endl;

				Thread::mutexUnlock(&tMutex);
				return;
			}

			SystemCompat::uSleep(850000);
			currentTime = getCurrentMediaTime();
			if (currentTime > dur) {
				currentTime = 0;
			}
			if (currentTime >= 0) {
				while (dur > (currentTime + 0.1)) {
					clog << "AVPlayer::run dur = '" << dur;
					clog << "' curMediaTime = '";
					clog << currentTime << "' total = '";
					clog << getTotalMediaTime();
					clog << "' for '" << mrl << "'" << endl;

					if (status != PLAY) {
						clog << "AVPlayer::run status != play => exiting";
						clog << endl;
						break;
					}

					timeRemain = (dur - currentTime) * 1000;
					if (outTransTime > 0.0) {
						timeRemain = outTransTime - (currentTime * 1000);
					}

					if (notifyContentUpdate) {
						if (timeRemain > 250) {
							notifyPlayerListeners(
									PL_NOTIFY_UPDATECONTENT,
									"",
									TYPE_PASSIVEDEVICE,
									"");
						}

						if (!this->mSleep(65)) { // 15 fps
							break;
						}

					} else if (status != PLAY || !this->mSleep(timeRemain)) {
						clog << "AVPlayer::run can't sleep '" << timeRemain;
						clog << "' => exiting" << endl;

						if (status == PLAY && outTransTime > 0.0) {
							outTransTime = 0;
							notifyPlayerListeners(PL_NOTIFY_OUTTRANS, "");

						} else {
							break;
						}

					} else if (outTransTime > 0.0) {
						clog << "AVPlayer::run notify transition at '";
						clog << currentTime << "' (out transition time is '";
						clog << outTransTime << "')" << endl;

						outTransTime = 0;
						notifyPlayerListeners(PL_NOTIFY_OUTTRANS, "");
					}

					currentTime = getCurrentMediaTime();
					if (currentTime <= 0) {
						clog << "AVPlayer::run currentTime = '" << currentTime;
						clog << "' => exiting" << endl;
						break;
					}

					if (lastCurrentTime == currentTime && status != PAUSE) {
						break;
					}
					lastCurrentTime = currentTime;
				}

				clog << "AVPlayer::run has exited from loop cause dur = '";
				clog << dur << "' and current time = '" << currentTime;
				clog << "'";
				clog << endl;

			} else {
				clog << "AVPlayer::run can't do nothing cause current time is";
				clog << " '" << currentTime << "'";
				clog << endl;
			}
		}

		clog << "AVPlayer::run(" << mrl << ") notifying ... " << endl;
		if (status != PAUSE) {
			presented = true;
		}

		if (status != STOP && status != PAUSE) {
			status  = STOP;
			running = false;

			if (provider != 0) {
				dm->stopProvider(provider);
			}

			clog << "AVPlayer::run(" << mrl << ") NOTIFY STOP" << endl;
			Thread::mutexUnlock(&tMutex);
			notifyPlayerListeners(PL_NOTIFY_STOP, "");

		} else {
			status = STOP;
			running = false;
			Thread::mutexUnlock(&tMutex);
		}

		clog << "AVPlayer::run(" << mrl << ") ALL DONE" << endl;
	}
}
}
}
}
}
}

using namespace ::br::pucrio::telemidia::ginga::core::player;

static pthread_mutex_t avpm;
static bool avpmInit = false;

extern "C" IPlayer* createAVPlayer(
		GingaScreenID screenId, const char* mrl) {

	AVPlayer* player;
	if (!avpmInit) {
		avpmInit = true;
		Thread::mutexInit(&avpm, NULL);
	}

	Thread::mutexLock(&avpm);
	player = new AVPlayer(screenId, mrl);
	Thread::mutexUnlock(&avpm);

	return player;
}

extern "C" void destroyAVPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
