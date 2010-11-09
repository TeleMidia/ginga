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
		cout << "ISDBTFrontend::ISDBTFrontend" << endl;

		this->feFd = feFd;

		initIsdbtParameters();
		ioctl(feFd, FE_GET_INFO, &info);
		dumpFrontendInfo();
	}

	ISDBTFrontend::~ISDBTFrontend() {

	}

	bool ISDBTFrontend::hasFrontend() {
		cout << "ISDBTFrontend::hasFrontend" << endl;
		return feFd > 0;
	}

	void ISDBTFrontend::initIsdbtParameters() {
		cout << "ISDBTFrontend::initIsdbtParameters" << endl;

#if defined(DVB_API_VERSION) && DVB_API_VERSION>=5
		params.delivery_system                 = SYS_ISDBT;
		params.inversion                       = INVERSION_AUTO;
		params.u.isdbt.bandwidth_hz            = 6000000;
		params.u.isdbt.guard_interval          = GUARD_INTERVAL_AUTO;
		params.u.isdbt.transmission_mode       = TRANSMISSION_MODE_AUTO;
		params.u.isdbt.isdbt_partial_reception = 0;
		params.u.isdbt.isdbt_sb_mode           = 0;
		params.u.isdbt.isdbt_sb_subchannel     = 0;
		params.u.isdbt.isdbt_sb_segment_idx    = 0;
		params.u.isdbt.isdbt_sb_segment_count  = 0;
		params.u.isdbt.isdbt_layer_enabled     = 0x7;
		params.u.isdbt.layer[0].segment_count  = 0;
		params.u.isdbt.layer[0].modulation     = QAM_AUTO;
		params.u.isdbt.layer[0].fec            = FEC_AUTO;
		params.u.isdbt.layer[0].interleaving   = 0;
		params.u.isdbt.layer[1].segment_count  = 0;
		params.u.isdbt.layer[1].modulation     = QAM_AUTO;
		params.u.isdbt.layer[1].fec            = FEC_AUTO;
		params.u.isdbt.layer[1].interleaving   = 0;
		params.u.isdbt.layer[2].segment_count  = 0;
		params.u.isdbt.layer[2].modulation     = QAM_AUTO;
		params.u.isdbt.layer[2].fec            = FEC_AUTO;
		params.u.isdbt.layer[2].interleaving   = 0;
#endif
	}

	void ISDBTFrontend::dumpFrontendInfo() {
		cout << "frontend_info:" << endl;
		cout << "name:            " << info.name << endl;
		cout << "fe_type:         " << info.type << endl;
		cout << "freq_min:        " << info.frequency_min << endl;
		cout << "freq_max:        " << info.frequency_max << endl;
		cout << "freq_step:       " << info.frequency_stepsize << endl;
		cout << "symbol_rate_min: " << info.symbol_rate_min << endl;
		cout << "symbol_rate_max: " << info.symbol_rate_max << endl;
		cout << "symbol_rate_tol: " << info.symbol_rate_tolerance << endl;
		cout << "notifier_delay:  " << info.notifier_delay << endl;
		cout << "caps:            " << info.caps << endl;
	}

	void ISDBTFrontend::updateIsdbtFrontendParameters() {
		cout << "ISDBTFrontend::updateIsdbtFrontendParameters" << endl;

#if defined(DVB_API_VERSION) && DVB_API_VERSION>=5

		struct dtv_property* dtv_prop_arg;
		struct dtv_properties dtv_prop;
		int index_arg = 0;
		int res;

		dtv_prop_arg = (struct dtv_property*)(
				malloc(27*sizeof(struct dtv_property)));

		dtv_prop_arg[index_arg  ].cmd    = DTV_CLEAR;
		dtv_prop_arg[index_arg++].u.data = 0;
		dtv_prop_arg[index_arg  ].cmd    = DTV_DELIVERY_SYSTEM;
		dtv_prop_arg[index_arg++].u.data = SYS_ISDBT;
		dtv_prop_arg[index_arg  ].cmd    = DTV_FREQUENCY;
		dtv_prop_arg[index_arg++].u.data = params.frequency;
		dtv_prop_arg[index_arg  ].cmd    = DTV_BANDWIDTH_HZ;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.bandwidth_hz;
		dtv_prop_arg[index_arg  ].cmd    = DTV_INVERSION;
		dtv_prop_arg[index_arg++].u.data = params.inversion;
		dtv_prop_arg[index_arg  ].cmd    = DTV_GUARD_INTERVAL;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.guard_interval;
		dtv_prop_arg[index_arg  ].cmd    = DTV_TRANSMISSION_MODE;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.transmission_mode;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_PARTIAL_RECEPTION;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.isdbt_partial_reception;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_SOUND_BROADCASTING;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.isdbt_sb_mode;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_SB_SUBCHANNEL_ID;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.isdbt_sb_subchannel;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_SB_SEGMENT_IDX;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.isdbt_sb_segment_idx;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_SB_SEGMENT_COUNT;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.isdbt_sb_segment_count;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERA_SEGMENT_COUNT;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[0].segment_count;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERA_MODULATION;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[0].modulation;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERA_FEC;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[0].fec;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERA_TIME_INTERLEAVING;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[0].interleaving;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERB_SEGMENT_COUNT;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[1].segment_count;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERB_MODULATION;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[1].modulation;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERB_FEC;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[1].fec;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERB_TIME_INTERLEAVING;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[1].interleaving;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERC_SEGMENT_COUNT;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[2].segment_count;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERC_MODULATION;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[2].modulation;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERC_FEC;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[2].fec;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYERC_TIME_INTERLEAVING;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.layer[2].interleaving;
		dtv_prop_arg[index_arg  ].cmd    = DTV_ISDBT_LAYER_ENABLED;
		dtv_prop_arg[index_arg++].u.data = params.u.isdbt.isdbt_layer_enabled;
		dtv_prop_arg[index_arg  ].cmd    = DTV_TUNE;
		dtv_prop_arg[index_arg++].u.data = 0;

		dtv_prop.num   = index_arg;
		dtv_prop.props = &dtv_prop_arg[0];

		res = ioctl(feFd, FE_SET_PROPERTY, &dtv_prop);
		if (res == -1) {
			cout << "setting properties failed" << endl;
		}
		free(dtv_prop_arg);
#endif
	}

	bool ISDBTFrontend::isTuned() {
		::usleep(200000);
		if (ioctl(feFd, FE_READ_STATUS, &feStatus) == -1) {
			cout << "ISDBTFrontend::isTuned FE_READ_STATUS failed" << endl;
			return false;
		}

		/* everything's working... */
		if (feStatus & FE_HAS_LOCK) {
			/* FEC is stable  */
			if (feStatus & FE_HAS_VITERBI) {
				/* found something above the noise level */
				if (feStatus & FE_HAS_SIGNAL) {
					/* found a carrier signal  */
					if (!feStatus & FE_HAS_CARRIER) {
						cout << "ISDBTFrontend::isTuned Warning! ";
						cout << "can't find carrier."<< endl;
					}

					/* found sync bytes  */
					if (feStatus & FE_HAS_SYNC) {
						cout << "ISDBTFrontend::isTuned Warning! ";
						cout << "can't find sync byte."<< endl;
					}
					return true;
				}
			}
		}
		return false;
	}

	bool ISDBTFrontend::getSTCValue(uint64_t* stc, int* valueType) {
		struct dmx_stc* _stc;
		int result, fd;

		if ((fd = open(IFE_DEMUX_DEV_NAME.c_str(), O_RDWR)) < 0) {
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

		cout << "ISDBTFrontend::getSTCValue '" << *stc << "'" << endl;
		close(fd);

		delete _stc;
		return true;
	}

	bool ISDBTFrontend::changeFrequency(unsigned int frequency) {
		int i;

		cout << "ISDBTFrontend::changeFrequency" << endl;

		currentFreq      = frequency;
		params.frequency = currentFreq;
		updateIsdbtFrontendParameters();

		for (i = 0; i < 6; i++) {
			if (isTuned()) {
				cout << "ISDBTFrontend::changeFrequency tuned at '";
				cout << currentFreq << "'" << endl;
				return true;
			}
		}

		cout << "ISDBTFrontend::changeFrequency can't change frequency";
		cout << endl;
		return false;
	}

	void ISDBTFrontend::scanFrequencies(vector<IChannel*>* channels) {
		int i;
		bool infFm = false, infVhf = false, infUhf = false;
		IChannel* channel;

		cout << "ISDBTFrontend::scanFrequencies searching channels" << endl;

		currentFreq             = IFE_FORCED_MIN_FREQ;
		info.frequency_stepsize = IFE_FORCED_STEP_SIZE;

		while (currentFreq < info.frequency_max) {
			currentFreq = currentFreq + info.frequency_stepsize;

			if (currentFreq < 115000000) {
				if (!infFm) {
					cout << "Current Frequency at FM band" << endl;
					infFm = true;
				}
				continue;
			}

			if (currentFreq >= 115000000 && currentFreq < 250000000) {
				if (!infVhf) {
					cout << "Current Frequency at VHF band" << endl;
					infVhf = true;
				}
				continue;
			}

			if (currentFreq >= 250000000 && currentFreq < 863000000) {
				if (!infUhf) {
					cout << "Current Frequency at UHF band" << endl;
					infUhf = true;
				}

			} else {
				break;
			}

			params.frequency = currentFreq;
			updateIsdbtFrontendParameters();

			for (i = 0; i < 4; i++) {
				if (isTuned()) {
					cout << "ISDBTFrontend::scanFrequencies tuned at '";
					cout << currentFreq << "'" << endl;

					channel = new Channel();
					channel->setFrequency(currentFreq);

					channels->push_back(channel);
				}
			}
		}

		cout << "ISDBTFrontend::scanFrequencies Finished." << endl;
	}

	void ISDBTFrontend::attachFilter(IFrontendFilter* filter) {
		struct dmx_sct_filter_params f;
		int fd, numOfFilters;
		ActionsToFilters* action;

		cout << "ISDBTFrontend::attachFilter pid = '";
		cout << filter->getPid() << "' tid = '" << filter->getTid();
		cout << "'" << endl;

		numOfFilters = runningFilters->size();

		if (numOfFilters == IFE_MAX_FILTERS) {
			cout << "ISDBTFrontend::attachFilter reached max filters" << endl;
			return;
		}

		if ((fd = open(IFE_DEMUX_DEV_NAME.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
			cout << "ISDBTFrontend::attachFilter can't open '";
			cout << IFE_DEMUX_DEV_NAME << "'" << endl;
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
			cout << "ISDBTFrontend::attachFilter ";
			cout << "ioctl DMX_SET_FILTER failed" << endl;
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
			start();
		}
	}

	int ISDBTFrontend::createPesFilter(
			int pid, int pesType, bool compositeFiler) {

		int fd, res;
		struct dmx_pes_filter_params f;
		__u16 convPid;

		convPid = pid;

		cout << "ISDBTFrontend::createPesFilter pid '" << convPid << "'";
		cout << " pesType '" << pesType << "'" << endl;
		memset(&f, 0, sizeof(f));

		fd = open(IFE_DEMUX_DEV_NAME.c_str(), O_RDWR);
		if (fd < 0) {
			cout << "ISDBTFrontend::createPesFilter can't open '";
			cout << IFE_DEMUX_DEV_NAME << "'" << endl;
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
			cout << "ISDBTFrontend::createPesFilter ";
			cout << "ioctl DMX_SET_PES_FILTER failed" << endl;
		}

		return fd;
	}

	void ISDBTFrontend::removeFilter(IFrontendFilter* filter) {
		ActionsToFilters* action;

		cout << "ISDBTFrontend::removeFilter" << endl;

		action = new ActionsToFilters;
		action->ff    = filter;
		action->isAdd = false;

		lock();
		actsToRunningFilters->push_back(action);
		unlock();
	}

	void ISDBTFrontend::updatePool() {
		int i;
		vector<IFrontendFilter*>::iterator j;

		for (i = 0; i < IFE_MAX_FILTERS; i++) {
			pollFds[i].fd = -1;
		}

		cout << "ISDBTFrontend::updatePool: current size is ";
		cout << runningFilters->size() << endl;

		i = 0;
		j = runningFilters->begin();
		while(j != runningFilters->end()) {
			pollFds[i].fd      = (*j)->getDescriptor();
			pollFds[i].events  = POLLIN;
			pollFds[i].revents = 0;
			i++;
			++j;
		}
	}

	void ISDBTFrontend::readFilters() {
		IFrontendFilter* filter;
		int i = 0, n, recv, fSize, fd;
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
			cout << "ISDBTFrontend::readFilters poll failed! ";
			cout << endl;
			return;

		} else if (n == 0) {
			/*cout << "ISDBTFrontend::readFilters poll timeout! ";
			cout << endl;*/
			return;
		}

		j = runningFilters->begin();
		while(j != runningFilters->end()) {
			filter = *j;
			fd     = filter->getDescriptor();
			if (pollFds[i].revents) {
				if (pollFds[i].fd != fd) {
					cout << "ISDBTFrontend::readFilters Warning Poolfd has ";
					cout << "a different fd of the filter";
					cout << endl;
				}

				if (((recv = read(
						fd, buf, sizeof(buf))) < 0) && errno == EOVERFLOW) {

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
	}

	void ISDBTFrontend::run() {
		cout << "ISDBTFrontend::run" << endl;
		do {
			readFilters();
		} while (true);
		cout << "ISDBTFrontend::run no filters running!" << endl;
	}
}
}
}
}
}
}
