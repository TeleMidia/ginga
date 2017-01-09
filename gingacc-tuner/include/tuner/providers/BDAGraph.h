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

#ifndef BDAGRAPH_H_
#define BDAGRAPH_H_

#include "tuner/providers/Channels.h"

#include "system/thread/Thread.h"
using namespace br::pucrio::telemidia::ginga::core::system::thread;

#include <wtypes.h>
#include <unknwn.h>
#include <ole2.h>
#include <limits.h>
/* FIXME: mingw.org doesn't define secure versions of
 * http://msdn.microsoft.com/en-us/library/f30dzcf6.aspxu */
#define NO_DSHOW_STRSAFE
#include <DShow.h>
#include <comcat.h>

#include <atlbase.h>
#include <initguid.h>

#include <ks.h>
#include <ksmedia.h>
#include <bdatypes.h>
#include <bdamedia.h>
#include <bdaiface.h>
#include <bdatif.h>

#include <dvdmedia.h>
#include <Tuner.h>
#include <uuids.h>
#include <time.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <comutil.h>

#include <map>
#include <list>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////

#ifndef __qedit_h__
#define __qedit_h__

///////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////

interface
ISampleGrabberCB : public IUnknown {
	virtual STDMETHODIMP SampleCB( double SampleTime, IMediaSample *pSample ) = 0;
	virtual STDMETHODIMP BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen ) = 0;
};

///////////////////////////////////////////////////////////////////////////////////

static
const
IID IID_ISampleGrabberCB = { 0x0579154A, 0x2B53, 0x4994, { 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85 } };

///////////////////////////////////////////////////////////////////////////////////

interface
ISampleGrabber : public IUnknown {
	virtual HRESULT STDMETHODCALLTYPE SetOneShot( BOOL OneShot ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetMediaType( const AM_MEDIA_TYPE *pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( AM_MEDIA_TYPE *pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( BOOL BufferThem ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( long *pBufferSize, long *pBuffer ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( IMediaSample **ppSample ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetCallback( ISampleGrabberCB *pCallback, long WhichMethodToCallback ) = 0;
};

///////////////////////////////////////////////////////////////////////////////////

static
const
IID IID_ISampleGrabber = { 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };

///////////////////////////////////////////////////////////////////////////////////

static
const
CLSID CLSID_SampleGrabber = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

///////////////////////////////////////////////////////////////////////////////////

static
const
CLSID CLSID_NullRenderer = { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };

///////////////////////////////////////////////////////////////////////////////////

static
const
CLSID CLSID_VideoEffects1Category = { 0xcc7bfb42, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };

///////////////////////////////////////////////////////////////////////////////////

static
const
CLSID CLSID_VideoEffects2Category = { 0xcc7bfb43, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };

///////////////////////////////////////////////////////////////////////////////////

static
const
CLSID CLSID_AudioEffects1Category = { 0xcc7bfb44, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };

///////////////////////////////////////////////////////////////////////////////////

static
const
CLSID CLSID_AudioEffects2Category = { 0xcc7bfb45, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////


using namespace std;

enum {
    ATSC   = 0x00000001,
    DVB_C  = 0x00000010,
    DVB_C2 = 0x00000020,
    DVB_S  = 0x00000040,
    DVB_S2 = 0x00000080,
    DVB_T  = 0x00000100,
    DVB_T2 = 0x00000200,
    ISDB_C = 0x00001000,
    ISDB_S = 0x00002000,
    ISDB_T = 0x00004000,
};

#define FREQ_LOW 470143
#define FREQ_HIGH 806143
#define FREQ_BANDWIDTH 6

struct Buffer {
	char* buffer;
	int len;
};

/* The main class for building the filter graph */
namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
class BDAGraph : public ISampleGrabberCB {

public:
    BDAGraph(string channelsFile, Channels *channelsList);
    virtual ~BDAGraph();

    /* */
	HRESULT tryToTune();
	HRESULT searchChannels();
    long getSignalStrength();

	bool initDevice();
	HRESULT execute(long freq);
	HRESULT changeChannelTo(long freq, bool setDefault = false);
	bool getBuffer(Buffer** buffer);
	long getTunedFreq();

private:
	bool searching;
	list<Buffer*> bufferList;
	pthread_mutex_t bufferMutex;

    /* ISampleGrabberCB methods */
    ULONG ul_cbrc;
    STDMETHODIMP_(ULONG) AddRef() { return ++ul_cbrc; }
    STDMETHODIMP_(ULONG) Release() { return --ul_cbrc; }
    STDMETHODIMP QueryInterface(REFIID /*riid*/, void** /*p_p_object*/)
        { return E_NOTIMPL; }
    STDMETHODIMP SampleCB(double d_time, IMediaSample* p_sample);
    STDMETHODIMP BufferCB(double d_time, BYTE* p_buffer, long l_buffer_len);

    CLSID     guid_network_type;   /* network type in use */
    long      l_tuner_used;        /* Index of the Tuning Device in use */
    unsigned  systems;             /* bitmask of all tuners' network types */

    /* registration number for the RunningObjectTable */
    DWORD     d_graph_register;

    IMediaControl*         p_media_control;
    IGraphBuilder*         p_filter_graph;
    ITuningSpaceContainer* p_tuning_space_container;
    ITuningSpace*          p_tuning_space;
    ITuneRequest*          p_tune_request;
    IDVBTuningSpace*       p_dvb_tuning_space;
    IDVBSTuningSpace*      p_dvbs_tuning_space;

	bool		  canReadNetworkName;
	string		  currentNetworkName;
	unsigned char currentVirtualChannel;
	clock_t		  rntStopTime;
	Channels	  *channelsList;
	string		  channelsFile;
	long		  tunedFreq;

    ICreateDevEnum* p_system_dev_enum;
    IBaseFilter*    p_network_provider;
    IBaseFilter*    p_tuner_device;
    IBaseFilter*    p_capture_device;
    IBaseFilter*    p_sample_grabber;
    IBaseFilter*    p_mpeg_demux;
    IBaseFilter*    p_transport_info;
    IScanningTuner* p_scanning_tuner;
    ISampleGrabber* p_grabber;

	void readNetworkInfo(clock_t stopTime);
	void readNI(BYTE* buf, size_t bufLen);
	int SubmitTuneRequest(void);
    unsigned EnumSystems(void);

    HRESULT SetUpTuner(REFCLSID guid_this_network_type);
    HRESULT Build();
    HRESULT Check(REFCLSID guid_this_network_type);
    HRESULT GetFilterName(IBaseFilter* p_filter, char** psz_bstr_name);
    HRESULT GetPinName(IPin* p_pin, char** psz_bstr_name);
    unsigned GetSystem(REFCLSID clsid);
    HRESULT ListFilters(REFCLSID this_clsid);
    HRESULT FindFilter(REFCLSID clsid, long* i_moniker_used,
        IBaseFilter* p_upstream, IBaseFilter** p_p_downstream);
    HRESULT Connect(IBaseFilter* p_filter_upstream,
        IBaseFilter* p_filter_downstream);
	HRESULT SetDVBT(long, long);
    HRESULT Start();
    HRESULT Destroy();
    HRESULT Register();
    void Deregister();
};
}
}
}
}
}
}
#endif /* BDAGRAPH_H_ */
