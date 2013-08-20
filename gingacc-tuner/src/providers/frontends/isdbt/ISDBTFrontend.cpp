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

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <linux/dvb/dmx.h>
#include <linux/dvb/version.h>
#include <errno.h>

#include "tuner/providers/frontends/isdbt/ISDBTFrontend.h"
#include "tuner/providers/IDataProvider.h"
#include "tuner/providers/ISTCProvider.h"
#include "tuner/providers/Channel.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {

	const string ISDBTFrontend::IFE_FE_DEV_NAME("/dev/dvb/adapter0/frontend0");
	const string ISDBTFrontend::IFE_DVR_DEV_NAME("/dev/dvb/adapter0/dvr0");
	const string ISDBTFrontend::IFE_DEMUX_DEV_NAME("/dev/dvb/adapter0/demux0");
	struct pollfd ISDBTFrontend::pollFds[ISDBTFrontend::IFE_MAX_FILTERS];

	bool ISDBTFrontend::firstFilter = true;
	vector<IFrontendFilter*>* ISDBTFrontend::runningFilters = (
			new vector<IFrontendFilter*>);

	vector<ActionsToFilters*>* ISDBTFrontend::actsToRunningFilters = (
			new vector<ActionsToFilters*>);

	ISDBTFrontend::ISDBTFrontend(int feFd) : Thread() {

                int res;
		clog << "ISDBTFrontend::ISDBTFrontend" << endl;

		this->feFd = feFd;
		this->dmFd = -1;
                this->dvrFd = -1;

		res = ioctl(feFd, FE_GET_INFO, &info);
                if (res == -1)
                  clog << "ISDBTFrontend::ISDBTFrontend error: " <<  strerror(errno) << endl;

                dumpFrontendInfo();

		initIsdbtParameters();
        }

	ISDBTFrontend::~ISDBTFrontend() {

	}

	bool ISDBTFrontend::hasFrontend() {
		clog << "ISDBTFrontend::hasFrontend" << endl;
		return feFd > 0;
	}

	void ISDBTFrontend::initIsdbtParameters() {
	    
		clog << "ISDBTFrontend::initIsdbtParameters: Enter" << endl;

                memset( &params, 0, sizeof(dvb_frontend_parameters) );

		// for debugging purposes...
		// params.frequency = 533142000; // sbt,  521142000 - rede vida,  599142000 - band; 

                params.inversion                    = (info.caps & FE_CAN_INVERSION_AUTO) ? INVERSION_AUTO : INVERSION_OFF;
                //                params.inversion                    = INVERSION_AUTO;
                params.u.ofdm.code_rate_HP          = FEC_AUTO;
                params.u.ofdm.code_rate_LP          = FEC_AUTO;
                params.u.ofdm.constellation         = QAM_AUTO;
                params.u.ofdm.transmission_mode     = TRANSMISSION_MODE_AUTO;
                params.u.ofdm.guard_interval        = GUARD_INTERVAL_AUTO;
                params.u.ofdm.hierarchy_information = HIERARCHY_NONE;
                params.u.ofdm.bandwidth = BANDWIDTH_6_MHZ;                                                                                                                                  

		// updateIsdbtFrontendParameters();
        }

	void ISDBTFrontend::dumpFrontendInfo() {
		clog << "frontend_info:" << endl;
		clog << "name:            " << info.name << endl;
		clog << "fe_type:         " << info.type << endl;
		clog << "freq_min:        " << info.frequency_min << endl;
		clog << "freq_max:        " << info.frequency_max << endl;
		clog << "freq_step:       " << info.frequency_stepsize << endl;
		clog << "symbol_rate_min: " << info.symbol_rate_min << endl;
		clog << "symbol_rate_max: " << info.symbol_rate_max << endl;
		clog << "symbol_rate_tol: " << info.symbol_rate_tolerance << endl;
		clog << "notifier_delay:  " << info.notifier_delay << endl;
		clog << "caps:            " << info.caps << endl;
	}

	void ISDBTFrontend::updateIsdbtFrontendParameters() {
		clog << "ISDBTFrontend::updateIsdbtFrontendParameters Enter" << endl;
		
		if (params.frequency == 0)
		    return;

		clog << "ISDBTFrontend::updateIsdbtFrontendParameters: frequency: " << params.frequency <<  endl;

		if (ioctl( feFd, FE_SET_FRONTEND, &params ) == -1) {
		    clog << "ISDBTFrontend::updateIsdbtFrontendParameters: ioctl error with arg FE_SET_FRONTEND" << endl;
                }

		if (dmFd == -1)
		{
		    dmFd = open(IFE_DEMUX_DEV_NAME.c_str(), O_RDWR);
		    /* WARNING: For now we're just _not_ using the filter infrastructure of the linux kernel, so we just grab the "full" TS */
		    if (dmFd < 0) {
			
			clog << "ISDBTFrontend::updateIsdbtFrontendParameters: " <<  IFE_DEMUX_DEV_NAME.c_str() << " could not be opened, bad things will happen!" << endl;
		    }
		
		    struct dmx_pes_filter_params filter_dmx;
		    filter_dmx.pid = 8192;
		    filter_dmx.input = DMX_IN_FRONTEND;
		    filter_dmx.output = DMX_OUT_TS_TAP;
		    filter_dmx.pes_type = DMX_PES_OTHER;
		    filter_dmx.flags = DMX_IMMEDIATE_START;
		    
		    if (ioctl(dmFd, DMX_SET_PES_FILTER, &filter_dmx)) {
			clog << "ISDBTFrontend::updateIsdbtFrontendParameters: ioctl error with arg IFE_DEMUX_DEV_NAME" << endl;
		    }
		}
                
		if (dvrFd == -1)
		{

		    // opening DVR device (non-blocking mode), we read TS data in this fd
		    dvrFd = open(IFE_DVR_DEV_NAME.c_str(), O_RDONLY | O_NONBLOCK);
		    if (dvrFd < 0) {
			clog << "ISDBTFrontend::updateIsdbtFrontendParameters: " <<  IFE_DVR_DEV_NAME.c_str() << " could not be opened, bad things will happen!" << endl;
		    }
		}



	}

	bool ISDBTFrontend::isTuned() {
                int value, signal;

                // why do we need this?
		SystemCompat::uSleep(200000);

                clog << "ISDBTFrontend::isTuned" << endl;

		if (ioctl(feFd, FE_READ_STATUS, &feStatus) == -1) {
			clog << "ISDBTFrontend::isTuned FE_READ_STATUS failed" << endl;
			return false;
		}

                if (feStatus & FE_HAS_LOCK)
                {
                    if (ioctl(feFd, FE_READ_SIGNAL_STRENGTH, &value) == -1) {                                                                                                                                                                                            
                        clog << "ISDBTFrontend::isTuned FE_READ_SIGNAL_STRENGTH failed" << endl;

                    }
                    else {
                        signal = value * 100 / 65535;
			clog << "ISDBTFrontend::isTuned: Signal locked, received power level is " << signal << "%" << endl;
                    }
                    return true;
                }
                return false;
	}

	bool ISDBTFrontend::getSTCValue(uint64_t* stc, int* valueType) {
		struct dmx_stc* _stc;
		int result, fd;
                
		if ((fd = open(IFE_DEMUX_DEV_NAME.c_str(), O_RDWR, 644)) < 0) {
			perror("ISDBTFrontend::getSTCValue FD");
			return false;
		}

		_stc = new struct dmx_stc;
		memset(_stc, 0, sizeof(struct dmx_stc));

		result = ioctl(fd, DMX_GET_STC, _stc);
		if (result == -1) {
			perror("ISDBTFrontend::getSTCValue IOCTL");
			delete _stc;
			return false;
		}

		*stc       = _stc->stc / _stc->base;
		*valueType = ST_90KHz;

		clog << "ISDBTFrontend::getSTCValue '" << *stc << "'" << endl;
		close(fd);

		delete _stc;
		return true;
	}

	bool ISDBTFrontend::changeFrequency(unsigned int frequency) {
		int i;
                
		clog << "ISDBTFrontend::changeFrequency 4" << endl;

		currentFreq      = frequency;
		params.frequency = currentFreq;
		updateIsdbtFrontendParameters();

                // try 6 times..
		for (i = 0; i < 6; i++) {
			if (isTuned()) {
				clog << "ISDBTFrontend::changeFrequency tuned at '";
				clog << currentFreq << "'" << endl;
				return true;
			}
		}

		clog << "ISDBTFrontend::changeFrequency can't change frequency";
		clog << endl;
		return false;
	}

	void ISDBTFrontend::scanFrequencies(vector<IChannel*>* channels) {
		int i;
		bool infFm = false, infVhf = false, infUhf = false;
		IChannel* channel;

		clog << "ISDBTFrontend::scanFrequencies searching channels" << endl;

		currentFreq             = IFE_FORCED_MIN_FREQ;
		info.frequency_stepsize = IFE_FORCED_STEP_SIZE;

		while (currentFreq < info.frequency_max) {
			currentFreq = currentFreq + info.frequency_stepsize;

			if (currentFreq < 115000000) {
				if (!infFm) {
					clog << "Current Frequency at Low VHF band" << endl;
					infFm = true;
				}
				continue;
			}

			if (currentFreq >= 115000000 && currentFreq < 250000000) {
				if (!infVhf) {
					clog << "Current Frequency at VHF band" << endl;
					infVhf = true;
				}
				continue;
			}

			if (currentFreq >= 250000000 && currentFreq < 863000000) {
				if (!infUhf) {
					clog << "Current Frequency at UHF band" << endl;
					infUhf = true;
				}

			} else {
				break;
			}

			params.frequency = currentFreq;
			updateIsdbtFrontendParameters();

			for (i = 0; i < 4; i++) {
				if (isTuned()) {
					clog << "ISDBTFrontend::scanFrequencies tuned at '";
					clog << currentFreq << "'" << endl;

					channel = new Channel();
					channel->setFrequency(currentFreq);

					channels->push_back(channel);
				}
			}
		}

		clog << "ISDBTFrontend::scanFrequencies Finished." << endl;
	}

	void ISDBTFrontend::attachFilter(IFrontendFilter* filter) {
                clog << "ISDBTFrontend::attachFilter not implemented! " << endl;
#if 0
		struct dmx_sct_filter_params f;
		int fd, numOfFilters;
		ActionsToFilters* action;

		clog << "ISDBTFrontend::attachFilter pid = '";
		clog << filter->getPid() << "' tid = '" << filter->getTid();
		clog << "'" << endl;

		numOfFilters = runningFilters->size();

		if (numOfFilters == IFE_MAX_FILTERS) {
			clog << "ISDBTFrontend::attachFilter reached max filters" << endl;
			return;
		}

		if ((fd = open(IFE_DEMUX_DEV_NAME.c_str(), O_RDWR | O_NONBLOCK, 0644))
				< 0) {

			clog << "ISDBTFrontend::attachFilter can't open '";
			clog << IFE_DEMUX_DEV_NAME << "'" << endl;
			return;
		}

		filter->setDescriptor(fd);

		memset(&f, 0, sizeof(f));

		if (filter->getPid() > 0) {
			f.pid = (uint16_t)filter->getPid();
		}

		if (filter->getTid() < 0x100 && filter->getTid() >= 0) {
			f.filter.filter[0] = (uint8_t)filter->getTid();
			f.filter.mask[0]   = 0xff;
		}

		if (filter->getTidExt() < 0x10000 && filter->getTidExt() > 0) {
			f.filter.filter[1] = (uint8_t)((filter->getTidExt() >> 8) & 0xff);
			f.filter.filter[2] = (uint8_t)(filter->getTidExt() & 0xff);
			f.filter.mask[1] = 0xff;
			f.filter.mask[2] = 0xff;
		}

		f.timeout = 0;
		f.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;

		if (ioctl(fd, DMX_SET_FILTER, &f) == -1) {
			clog << "ISDBTFrontend::attachFilter ";
			clog << "ioctl DMX_SET_FILTER failed" << endl;
			return;
		}

		action = new ActionsToFilters;
		action->ff    = filter;
		action->isAdd = true;

		lock();
		actsToRunningFilters->push_back(action);
		unlock();


		if (firstFilter) {
			firstFilter = false;
			startThread();
		}
#endif

	}

	int ISDBTFrontend::createPesFilter(
	 		int pid, int pesType, bool compositeFiler) {
                clog << "ISDBTFrontend::createPesFilter not implemented!" << endl;
#if 0

		int fd, res;
		struct dmx_pes_filter_params f;
		__u16 convPid;

		convPid = pid;

		clog << "ISDBTFrontend::createPesFilter pid '" << convPid << "'";
		clog << " pesType '" << pesType << "'" << endl;
		memset(&f, 0, sizeof(f));

		fd = open(IFE_DEMUX_DEV_NAME.c_str(), O_RDWR, 0644);
		if (fd < 0) {
			clog << "ISDBTFrontend::createPesFilter can't open '";
			clog << IFE_DEMUX_DEV_NAME << "'" << endl;
			return fd;
		}

		f.pid      = convPid;
		f.input    = DMX_IN_FRONTEND;
		f.output   = DMX_OUT_TS_TAP;

		if (compositeFiler) {
			f.pes_type = DMX_PES_OTHER;

		} else if (pesType == PFT_AUDIO) {
			f.pes_type = DMX_PES_AUDIO0;

		} else if (pesType == PFT_VIDEO) {
			f.pes_type = DMX_PES_VIDEO0;

		} else if (pesType == PFT_PCR) {
			f.pes_type = DMX_PES_PCR0;

		} else {
			f.pes_type = DMX_PES_OTHER;
		}

		f.flags = DMX_IMMEDIATE_START;

		res = ioctl(fd, DMX_SET_PES_FILTER, &f);
		if (res < 0) {
			clog << "ISDBTFrontend::createPesFilter ";
			clog << "ioctl DMX_SET_PES_FILTER failed" << endl;
		}
		return fd;
#endif
                return -1;
	}

	void ISDBTFrontend::removeFilter(IFrontendFilter* filter) {
                clog << "ISDBTFrontend::removeFilter not implemented!" << endl;
#if 0
		ActionsToFilters* action;

		clog << "ISDBTFrontend::removeFilter" << endl;

		action = new ActionsToFilters;
		action->ff    = filter;
		action->isAdd = false;

		lock();
		actsToRunningFilters->push_back(action);
		unlock();
#endif
	}

	void ISDBTFrontend::updatePool() {
		int i;
		vector<IFrontendFilter*>::iterator j;

		for (i = 0; i < IFE_MAX_FILTERS; i++) {
			pollFds[i].fd = -1;
		}

		clog << "ISDBTFrontend::updatePool: current size is ";
		clog << runningFilters->size() << endl;

		i = 0;
		j = runningFilters->begin();
		while(j != runningFilters->end()) {
			pollFds[i].fd      = (*j)->getDescriptor();
			pollFds[i].events  = POLLIN;
			pollFds[i].revents = 0;
			i++;
			++j;
		}

		clog << "ISDBTFrontend::updatePool: OUT ";
	}

	void ISDBTFrontend::readFilters() {
                clog << "ISDBTFrontend::readFilters not implemented! ";
#if 0
		IFrontendFilter* filter;
		int fd;
		int i = 0, n, recv, fSize;
		vector<IFrontendFilter*>::iterator j;
		vector<ActionsToFilters*>::iterator k;
		char buf[4096];
		bool mustUpdate = false;

		lock();
		k = actsToRunningFilters->begin();
		while (k != actsToRunningFilters->end()) {
			mustUpdate = true;
			if ((*k)->isAdd) {
				runningFilters->push_back((*k)->ff);

			} else {
				j = runningFilters->begin();
				while(j != runningFilters->end()) {
					if (*j == (*k)->ff) {
						runningFilters->erase(j);
						break;
					}
					++j;
				}
			}
			delete (*k);
			++k;
		}
		actsToRunningFilters->clear();
		unlock();

		if (mustUpdate) {
			updatePool();
		}

		fSize = runningFilters->size();

		n = poll(pollFds, fSize, 1000);
		if (n == -1) {
			clog << "ISDBTFrontend::readFilters poll failed! ";
			clog << endl;
			return;

		} else if (n == 0) {
			/*clog << "ISDBTFrontend::readFilters poll timeout! ";
			clog << endl;*/
			return;
		}

		j = runningFilters->begin();
		while(j != runningFilters->end()) {
			filter = *j;
			fd     = filter->getDescriptor();
			if (pollFds[i].revents) {
				if (pollFds[i].fd != fd) {
					clog << "ISDBTFrontend::readFilters Warning Poolfd has ";
					clog << "a different fd of the filter";
					clog << endl;
				}

				if (((recv = read(fd, buf, sizeof(buf)) < 0)) &&
						errno == EOVERFLOW) {

					//avoid overflow
					recv = read(fd, buf, sizeof(buf));
				}

				if (recv > 4) {
					filter->receiveSection(buf, recv);
				}
			}
			i++;
			++j;
		}
#endif
	}

	void ISDBTFrontend::run() {
		clog << "ISDBTFrontend::run" << endl;
#if 0
		do {
			readFilters();
		} while (true);
#endif
		clog << "ISDBTFrontend::run no filters running!" << endl;
	}
}
}
}
}
}
}
