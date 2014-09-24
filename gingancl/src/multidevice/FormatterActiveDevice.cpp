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
#include "gingancl/multidevice/FormatterActiveDevice.h"
#include "gingancl/multidevice/FMDComponentSupport.h"


#if HAVE_COMPONENTS
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "mb/interface/CodeMap.h"
#include "mb/IInputManager.h"
#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#endif

//TODO: fix formatter instantiation
#include "gingancl/FormatterMediator.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;


#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "ncl/layout/DeviceLayout.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

extern "C" {
	#include <stdlib.h>
}

#ifdef _MSC_VER
extern "C" {
#include "asprintf.h"
}
#endif


#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <algorithm>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace multidevice {

	FormatterActiveDevice::FormatterActiveDevice(
			GingaScreenID screenId,
			IDeviceLayout* deviceLayout,
			int x, int y, int w, int h, bool useMulticast, int srvPort) : FormatterMultiDevice(
					screenId, deviceLayout, x, y, w, h, useMulticast, srvPort) {

		this->deviceServicePort = srvPort;


		img_dir   = SystemCompat::appendGingaFilesPrefix("md");
		img_dir.append(SystemCompat::getIUriD());
		img_dev   = img_dir + "active-device.png";
		img_reset = img_dir + "active-device-reset.png";
		clog << "FormatterActiveDevice::constructor" << endl;
		set<int>* evs;

		contentsInfo = new map<string, string>;
		deviceClass  = IDeviceDomain::CT_ACTIVE;
		formatter    = NULL;

		defaultWidth  = dm->getDeviceWidth(myScreen);
		defaultHeight = dm->getDeviceHeight(myScreen);

		mainLayout = new FormatterLayout(myScreen, x, y, w, h);
		layoutManager[deviceClass] = mainLayout;

		evs = new set<int>;
		evs->insert(CodeMap::KEY_TAP);
		evs->insert(CodeMap::KEY_F11);
		evs->insert(CodeMap::KEY_F10);
		im->addInputEventListener(this, evs);

		if (fileExists(img_dev)) {
			serialized = dm->createWindow(
					myScreen, 0, 0, DV_QVGA_WIDTH, DV_QVGA_HEIGHT, -1.0);

			s = dm->createRenderedSurfaceFromImageFile(
					myScreen, img_dev.c_str());

			int cap = dm->getWindowCap (myScreen, serialized, "ALPHACHANNEL");
			dm->setWindowCaps (myScreen, serialized, cap);
			dm->drawWindow(myScreen, serialized);

			dm->showWindow (myScreen, serialized);
			dm->renderWindowFrom (myScreen, serialized, s);
			dm->lowerWindowToBottom (myScreen, serialized);

			dm->deleteSurface(s);
			s = NULL;

		} else {
			clog << "FormatterActiveDevice::constructor Warning! File not found: ";
			clog << img_dev << endl;
		}

		TCPServerSocket * servSock;
		bool tryServerSocket=true;
		deviceServicePort = 4444;
		while(tryServerSocket){
			try{
				clog << "FormatterActiveDevice:: trying use port " << deviceServicePort << "..." << endl;
						servSock = new TCPServerSocket(deviceServicePort);
				tryServerSocket=false;
			} catch (SocketException &e) {
				deviceServicePort++;
				clog << "FormatterActiveDevice::deviceServicePort"<< deviceServicePort<< "already in use. Exception error: " << e.what() << endl;
			}
		}
		char* srvPortStr;
		asprintf(&srvPortStr,"%d",deviceServicePort);
		tmp_dir   = SystemCompat::getTemporaryDir() + srvPortStr + SystemCompat::getIUriD();
		SystemCompat::makeDir(tmp_dir.c_str(),0755);

		if (rdm == NULL) {
#if HAVE_COMPONENTS
			rdm = ((IRemoteDeviceManagerCreator*)(cm->getObject(
					"RemoteDeviceManager")))(deviceClass, useMulticast, deviceServicePort);
#else
			rdm = RemoteDeviceManager::getInstance();
			((RemoteDeviceManager*)rdm)->setDeviceDomain(
					new ActiveDeviceDomain(useMulticast, deviceServicePort));

#endif
		}

		rdm->setDeviceInfo(deviceClass, w, h, "");
		rdm->addListener(this);

#if HAVE_COMPONENTS
		privateBaseManager = ((PrivateBaseManagerCreator*)(cm->getObject(
				"PrivateBaseManager")))();

#else
		privateBaseManager = new PrivateBaseManager();
#endif

		ContentTypeManager::getInstance()->setMimeFile(
				SystemCompat::appendGingaFilesPrefix("mimetypes.ini")
		);

		listening = true;
		try {
			//TCPServerSocket servSock(FormatterActiveDevice::COMMAND_SERVER_PORT);
			while (listening) {
				clog << "FormatterActiveDevice::FormatterActiveDevice ";
				clog << "waiting servSock.accept() on port " << srvPortStr << endl;

				tcpSocket = servSock->accept();

				clog << "FormatterActiveDevice::FormatterActiveDevice ";
				clog << "servSock accepted" << endl;

				//handleTCPClient(servSock.accept());
				if (serialized) {
					dm->hideWindow(myScreen, serialized);
				}
				handleTCPClient(tcpSocket);
			}
			servSock->cleanUp();

		} catch (SocketException &e) {
			cerr << e.what() << endl;
			clog << "FormatterActiveDevice::End of Connection";
			clog << " with Base Device" << endl;
			listening = false;
		}

	}

	FormatterActiveDevice::~FormatterActiveDevice() {

		//lock();

		listening = false;
		dm->releaseScreen(myScreen);

		if (privateBaseManager != NULL) {
			delete privateBaseManager;
			privateBaseManager = NULL;
		}
		if (rdm != NULL) {
			delete rdm;
			rdm = NULL;
		}

		if (im != NULL) {
			im->removeInputEventListener(this);
			im = NULL;
		}

		if (formatter != NULL) {
			delete formatter;
			formatter = NULL;
		}

		if (initVars.size()>0) {
			initVars.clear();
		}

		//unlock();
		clog << "FormatterActiveDevice::~FormatterActiveDevice";
		clog << " all done" << endl;
	}

	bool FormatterActiveDevice::socketSend(TCPSocket* sock,string payload) {
		char* buffer;
		int plSize;

		if (sock == NULL) {
			return false;
		}

		buffer = (char*) payload.c_str();
		plSize = (int) payload.size();

		try {
			sock->send(buffer, plSize);

		} catch (SocketException &e) {
			cerr << e.what() << endl;
		}

		return false;
	}

	void FormatterActiveDevice::connectedToBaseDevice(unsigned int domainAddr) {
		clog << "FormatterActiveDevice::connectedToDomainService '";
		clog << domainAddr << "'" << endl;

		hasRemoteDevices = true;
		im->addInputEventListener(this, NULL);
	}

	bool FormatterActiveDevice::receiveRemoteEvent(
			int remoteDevClass,
			int eventType,
			string eventContent) {

		vector<string>* args;
		if (eventType == IDeviceDomain::FT_ATTRIBUTIONEVENT) {
			if (remoteDevClass == -1) {

				//Only sends to parent device vars within the "parent." namespace

				int tokenPos = eventContent.find("parent.");

				if (tokenPos == 0) {
					stringstream sst;
					sst << "EVT ATTR " << eventContent;
					string evtattr = sst.str();

					clog << "FormatterActiveDevice::";
					clog << "post " << evtattr << endl;
					socketSend(tcpSocket,evtattr);
				}
			}
		}

		if (remoteDevClass == IDeviceDomain::CT_BASE) {
			if (eventType == IDeviceDomain::FT_PRESENTATIONEVENT) {
				if (eventContent.find("::") != std::string::npos) {
					args = split(eventContent, "::");
					if (args->size() == 2) {
						if ((*args)[0] == "start") {
							formatter->play();
						} else if ((*args)[0] == "stop") {
							formatter->stop();
							/*
							player->stop();
							delete player;
							player = NULL;
							*/
							//TODO: check
						}
					}
					delete args;
					return true;
				}
			}
		}

		return false;
	}

	bool FormatterActiveDevice::receiveRemoteContent(
			int remoteDevClass,
			string contentUri) {

		map<string, string>::iterator i;
		GingaSurfaceID s;

		clog << "FormatterActiveDevice::receiveRemoteContent from class '";
		clog << remoteDevClass << "' and contentUri '" << contentUri << "'";
		clog << endl;

		i = contentsInfo->find(contentUri);
		if (contentUri.find(".ncl") != std::string::npos &&
				i != contentsInfo->end()) {
/*
			NclPlayerData* data = new NclPlayerData;
			data->screenId      = myScreen;
			data->baseId        = i->second;
			data->playerId      = i->second;
			data->devClass      = deviceClass;
			data->x             = xOffset;
			data->y             = yOffset;
			data->w             = defaultWidth;
			data->h             = defaultHeight;
			data->enableGfx     = false;
			data->parentDocId   = "";
			data->nodeId        = "";
			data->docId         = "";
			data->focusManager  = NULL;
			data->editListener  = NULL;
*/
//			player = new FormatterMediator(NULL);
/*
#if HAVE_COMPONENTS
				player = ((PlayerCreator*)(cm->getObject("Player")))(
						myScreen, contentUri.c_str(), false);
#else
				player = new Player(myScreen, contentUri);
#endif

			player->addListener(this);


			//s = dm->createSurface(myScreen);

			//player->setSurface(s);
			//player->setCurrentDocument(contentUri);
			//player->setParentLayout(mainLayout);
*/
			//TODO: use FormatterMediator (as in PresentationEngineManager)
			return true;
		}

		return false;
	}

	bool FormatterActiveDevice::receiveRemoteContentInfo(
			string contentId, string contentUri) {

		(*contentsInfo)[contentUri] = contentId;
		return true;
	}

	bool FormatterActiveDevice::userEventReceived(IInputEvent* ev) {
		string mnemonicCode;
		int currentX;
		int currentY;
		int code;

		code = ev->getKeyCode(myScreen);
    if(code == CodeMap::KEY_F11 || code == CodeMap::KEY_F10) {
      std::abort();
    }
		if (code == CodeMap::KEY_TAP) {
			ev->getAxisValue(&currentX, &currentY, NULL);
			tapObject(deviceClass, currentX, currentY);
		}

		return true;
	}


	bool FormatterActiveDevice::openDocument(string contentUri) {
		bool open = false;
		//lock();

		if (formatter == NULL) {
			formatter = createNCLPlayer();

		}


		formatter->setCurrentDocument(contentUri);
		((FormatterMediator *)formatter)->getPresentationContext()->
				setRemoteDeviceListener((IRemoteDeviceListener *)this);
		//formatter->getPresentationContext()->setGlobalVarListener(this);
		/*
		if (presContext == NULL)  {
			clog << "FormatterActiveDevice::presContext == NULL"<<endl;
			presContext = ((FormatterMediator *)formatter)->getPresentationContext();
			presContext->setGlobalVarListener(this);
		}
		else {
			clog << "FormatterActiveDevice::presContext != NULL"<<endl;
			//presContext = ((FormatterMediator *)formatter)->getPresentationContext();
			presContext->setGlobalVarListener((IContextListener*)this);
		}
		*/
		//unlock();
		return (formatter != NULL);
	}

	NclPlayerData* FormatterActiveDevice::createNclPlayerData() {
		NclPlayerData* data = NULL;

		data                     = new NclPlayerData;
		data->baseId             = "";
		data->playerId           = "";
		data->devClass           = 0;
		data->screenId           = myScreen;
		data->x                  = xOffset;
		data->y                  = yOffset;
		data->w                  = defaultWidth;
		data->h                  = defaultHeight;
		data->enableGfx          = true;
		data->parentDocId        = "";
		data->nodeId             = "";
		data->docId              = "";
		data->transparency       = 0;
		data->focusManager       = NULL;
		data->privateBaseManager = privateBaseManager;
		data->editListener       = (IPlayerListener*)this;

		return data;
	}

	INCLPlayer* FormatterActiveDevice::createNCLPlayer() {
		INCLPlayer* fmt = NULL;
		NclPlayerData* data = NULL;
		data                     = createNclPlayerData();
		data->baseId             = 1;
		data->playerId           = "active-device";
		data->privateBaseManager = privateBaseManager;
		//TODO: component manager to instantiate formatter (as in PresentationEngineManager)

#if HAVE_COMPONENTS
			fmt = ((NCLPlayerCreator*)(cm->getObject("Formatter")))(data);

#else
			fmt = new FormatterMediator(data);
#endif
		//fmt = new FormatterMediator(data);
		//(NclDocument*)(formatter->setCurrentDocument(fname));

		fmt->addListener(this);

		return fmt;
	}

	/* translates the command code from string to the const int values */
	int FormatterActiveDevice::getCommandCode(string* com) {
		if (com->compare("ADD")==0) {
			return FormatterActiveDevice::ADD_DOCUMENT;

		} else  if (com->compare("REMOVE")==0) {
			return FormatterActiveDevice::REMOVE_DOCUMENT;

		} else if (com->compare("START")==0) {
			return FormatterActiveDevice::START_DOCUMENT;

		} else if (com->compare("STOP")==0) {
			return FormatterActiveDevice::STOP_DOCUMENT;

		} else if (com->compare("PAUSE")==0) {
			return FormatterActiveDevice::PAUSE_DOCUMENT;

		} else if (com->compare("RESUME")==0) {
			return FormatterActiveDevice::RESUME_DOCUMENT;

		} else if (com->compare("SET")==0) {
			return FormatterActiveDevice::SET_VAR;

		} else if (com->compare("SELECT")==0) {
			return FormatterActiveDevice::SELECTION;
		}

		return 0;
	}

	/*
	 * handles a command coming from TCP, which controls the formatter
	 * command syntax
	 * 	  <ID> <NPT> <COMMAND> <PAYLOAD_DESC> <PAYLOAD_SIZE>\n<PAYLOAD>
	 */
	bool FormatterActiveDevice::handleTCPCommand(
			string sid,
			string snpt,
			string scommand,
			string spayload_desc,
			string payload) {

		bool handled   = false;

		string appPath = tmp_dir+spayload_desc;
		appPath = appPath.substr(0, appPath.rfind(".")) + SystemCompat::getIUriD();

		int command_id = getCommandCode(&scommand);
		string zip_dump;

		clog << "FormatterActiveDevice::handleTCPCommand" << endl;
		switch (command_id) {
			case FormatterActiveDevice::ADD_DOCUMENT:{
				zip_dump = tmp_dir + "tmpzip.zip";

				clog << "ADD node path = '" << spayload_desc << "'" << endl;

				writeFileFromBase64(payload, (char*)zip_dump.c_str());
				SystemCompat::unzip_file((char*)zip_dump.c_str(), (char*)appPath.c_str());

				remove((char*)zip_dump.c_str());
				handled = true;
			}
			break;

			case FormatterActiveDevice::REMOVE_DOCUMENT: {
				clog << "FormatterActiveDevice::REMOVE DOCUMENT" << endl;
				handled = true;
			}
			break;

			case FormatterActiveDevice::START_DOCUMENT: {
				clog << "FormatterActiveDevice::START:" << spayload_desc << endl;

				if (!payload.empty()) {
					if(!isDirectory(appPath.c_str()))
					     SystemCompat::makeDir(appPath.c_str(), 0755);
					zip_dump = tmp_dir + "tmpzip.zip";

					clog << "ADD node path = '" << appPath << spayload_desc << "'" << endl;
                    clog << "PAYLOAD SIZE = " << strlen(payload.c_str()) << endl;

					writeFileFromBase64(payload, (char*)zip_dump.c_str());
					SystemCompat::unzip_file((char*)zip_dump.c_str(),(char*)appPath.c_str());
					//remove((char*)zip_dump.c_str());
				}

				string full_path = appPath+spayload_desc;

				if (currentDocUri.compare(full_path) != 0) {
					if (openDocument(full_path)) {

						clog << "FormatterActiveDevice::START_DOCUMENT play '";
						clog << full_path << "'";

						clog << endl;
						formatter->setKeyHandler(true);
						formatter->play();

						//TODO: improve vars initialization
						//using formatter->setPropertyValue(pname,pvalue);
						//would do Player (gingacc-player) properties[name] = value

						map<string,string>::iterator it;

						for ( it=initVars.begin() ; it != initVars.end(); it++ ) {

							string pname = (string)(*it).first;
							string pvalue = (string)(*it).second;

							//formatter->setPropertyValue(pname,pvalue);

							((FormatterMediator *)formatter)->getPresentationContext()->
									setPropertyValue(pname,pvalue);

						    //clog << (*it).first << " => " << (*it).second << endl;
						}

					}
					else {
						clog << "FormatterActiveDevice::START_DOCUMENT: " << full_path;
						clog << " open failure!"<<endl;
					}
				}
				else {
					//TODO: handle start with different doc
				}

				handled = true;
			}
			break;

			case FormatterActiveDevice::STOP_DOCUMENT: {
				clog << "FormatterActiveDevice::STOP DOCUMENT" << endl;
				string full_path = string("");
				full_path.append(appPath);
				full_path.append(spayload_desc);
				if (currentDocUri.compare(full_path) == 0) {
					if (formatter != NULL) {
						formatter->stop();
					}

					currentDocUri = "";
					//TODO: handle start with different doc
				}

				handled = true;
			}
			break;

			case FormatterActiveDevice::PAUSE_DOCUMENT: {
				clog << "FormatterActiveDevice::PAUSE DOCUMENT" << endl;

				string full_path = string("");
				full_path.append(appPath);
				full_path.append(spayload_desc);

				if (currentDocUri.compare(full_path) == 0) {
					if (formatter != NULL) {
						formatter->pause();
					}
				}
				handled = true;
			}
			break;

			case FormatterActiveDevice::RESUME_DOCUMENT: {
				clog << "FormatterActiveDevice::RESUME DOCUMENT" << endl;
				//spayload_desc = appPath + spayload_desc;

				string full_path = string("");
				full_path.append(appPath);
				full_path.append(spayload_desc);
				if (currentDocUri.compare(full_path) == 0) {
					if (formatter != NULL) {
						formatter->resume();
					}
				}
				handled = true;
			}
			break;

			case FormatterActiveDevice::SET_VAR: {
				string pname, pvalue;
				size_t pos;

				pos = spayload_desc.find("=");
				pname = spayload_desc.substr(0,pos);
				pvalue = spayload_desc.substr(pos+1);
				if (formatter != NULL) {
					//TODO: check if formatter is active?
					formatter->setPropertyValue(pname,pvalue);
					clog << "FormatterActiveDevice::SET VAR " << pname << " = " << pvalue << endl;
				}
				else {
					initVars[pname] = pvalue;
					clog << "FormatterActiveDevice::SET VAR (init) " << pname << " = ";
					clog << initVars[pname] << endl;
					//parent session initialization vars
				}
				handled = true;
			}
			break;

			case FormatterActiveDevice::SELECTION: {
				clog << "FormatterActiveDevice::SELECTION" << endl;
				//TODO: handle selection
				handled = true;
			}
			break;

			default:
				break;
		}

		return handled;
	}

	/* TCP client handling function */
	void FormatterActiveDevice::handleTCPClient(TCPSocket *sock) {
		unsigned int i;
		size_t pos;
		string buf;
		bool valid_command = false;

		string sid, snpt, scommand, spayload_desc, spayload_size;
		string payload = "";
		int payload_size;
		stringstream ss;
		char buffer[FormatterActiveDevice::RCVBUFSIZE+1] = {0};
		int recvMsgSize;
		bool reading = true;
		vector<string> tokens;
        //char pri[100];
		char pri[FormatterActiveDevice::RCVBUFSIZE]; //first line; MAX command size
		char *sec; //second line

		if (rdm != NULL) {
			rdm->release();
		}

		clog << "FormatterActiveDevice::Handling connection from: ";

		try {
			clog << sock->getForeignAddress() << ":";

		} catch (SocketException& e) {
			cerr << "FormatterActiveDevice::Unable to get foreign address";
			cerr << endl;
		}

		try {
			clog << sock->getForeignPort();

		} catch (SocketException& e) {
			cerr << "FormatterActiveDevice::Unable to get foreign port" << endl;
		}

		clog << endl;
		/////////

		//while ((recvMsgSize = sock->recv(
				//buffer, FormatterActiveDevice::RCVBUFSIZE)) > 0)

		while (reading) {
			clog << "FormatterActiveDevice:: waiting"<<endl;
			recvMsgSize = 0;
			valid_command = false;
			payload = "";
			buf = "";
			payload_size = 0;

			recvMsgSize = sock->recv(buffer, FormatterActiveDevice::RCVBUFSIZE);
			//TODO: improve read/write to buffer using a loop to assure it gets at least 100b
			//the above line is ok for usage over a local network

			//clog << "FormatterActiveDevice recv Msg" << buffer << endl;

			if (recvMsgSize <= 0) {
				clog << "FormatterActiveDevice: Lost connection to base device";
				clog << endl;
				clog << "FormatterActiveDevice: restart ginga as an active";
				clog << " device";
				clog << " again if you wish to search for a base device.";
				clog << endl;
				reading = false;

				//TODO: player->stop()? flag to define this?
				//TODO: only change image if nothing is playing?

				serialized = dm->createWindow(
						myScreen, 0, 0, DV_QVGA_WIDTH, DV_QVGA_HEIGHT, -1.0);

				if (fileExists(img_reset)) {
					s = dm->createRenderedSurfaceFromImageFile(
							myScreen, img_reset.c_str());

					int cap = dm->getWindowCap (myScreen, serialized, "ALPHACHANNEL");
					dm->setWindowCaps (myScreen, serialized, cap);
					dm->drawWindow (myScreen, serialized);

					dm->showWindow (myScreen, serialized);
					dm->renderWindowFrom (myScreen, serialized, s);
					dm->lowerWindowToBottom (myScreen, serialized);

					dm->deleteSurface(s);
					s = NULL;
				}
				break;
			}

			//Splitting the two lines of a command (the second is optional)
			sec = strchr(buffer,'\n');
			pos = sec - buffer+1;
			strncpy(pri,buffer,pos);
			pri[pos] = '\0';  //first line (command)

			ss << pri;

			payload_size = 0;
			tokens.reserve(5);

			while (ss >> buf) {
				tokens.push_back(buf);
			}

			//improve the quality of the code below
			//command with more than 5 tokens = error
			if ((tokens.size() > 5) || (tokens.size() < 4))  {
				clog << "FormatterActiveDevice::received an invalid command";
				clog << endl;

				tokens.clear();
				for (i = 0; i < RCVBUFSIZE+1; i++) {
					buffer[i] = 0;
				}
				break;
			}

			//no payload
			if (tokens.size() == 4) {
				sid = string(tokens[0]);
				snpt = string(tokens[1]);
				scommand = string(tokens[2]);
				spayload_desc = string(tokens[3]);
				valid_command = handleTCPCommand(
						sid, snpt, scommand, spayload_desc, "");

			} else if (tokens.size() >= 5) {
				sid = string(tokens[0]);
				snpt = string(tokens[1]);
				scommand = string(tokens[2]);
				spayload_desc = string(tokens[3]);
				spayload_size = string(tokens[4]);

				payload_size = ::atoi(spayload_size.c_str());

                clog << "FormatterActiveDevice::Payload size=";
				clog << payload_size << endl;

				//check all sizes
				//There is another line for the payload
				if (payload_size > 0) {
					int pri_len = (int)strlen(pri);
					//pri_len--;
					int diff = recvMsgSize - (pri_len);
					++sec;
                    ++sec;
/*
#if defined(_WIN32) && !defined(__MINGW32__)
					++sec;
#endif
*/
					//TODO: fix the lines above (\n removal) with portable solution

					sec[diff] = '\0'; //part of the payload (second line)

					if (payload_size > diff) {
						int rest = ((payload_size - diff)+1);
						int buff_size = 1024;
						int received = 0;
						int par_rec;
						char par_payload[1025]; //buff_size

						payload.append(sec);//mounting payload
						while (received < rest) {
							par_rec = sock->recv(par_payload, buff_size	);
							received = received + par_rec;
							par_payload[par_rec] = '\0';
							payload.append(par_payload, par_rec);
						}
						clog << "FormatterActiveDevice::received_size = ";
						clog << received << endl;

//						rest_payload[rest-1] = '\0';
						//payload.append(rest_payload);
					}

				} else {
					int pri_len = (int)strlen(pri);
					int diff = recvMsgSize - (pri_len);
					sec[diff] = '\0'; //part of the payload (second line)

					++sec;
#if defined(_WIN32) && !defined(__MINGW32__)
//TODO: fix this
					++sec;
#endif

					payload.append(sec);
				}

				//clog << "::payload = "<<payload << endl;
				//++sec;
				//clog << "::PAYLOAD: " << payload << endl; //sec = payload
				valid_command = handleTCPCommand(
						sid, snpt, scommand, spayload_desc, payload);
			}

			while (tokens.size() > 0) {
				tokens.pop_back();
			}

			ss.flush();
			ss.clear();

			//payload.clear();

			for (i = 0; i < FormatterActiveDevice::RCVBUFSIZE+1; i++) {
				buffer[i] = 0;
			}

			if (valid_command) {
				//sock->send("OK\n",3);
				clog << "FormatterActiveDevice::received a valid command";
				clog << endl;

			} else {
				//sock->send("NOK\n",4);
				clog << "FormatterActiveDevice::received an invalid command";
				clog << endl;
			}

		}//while
		delete sock;
	}
}
}
}
}
}
}
