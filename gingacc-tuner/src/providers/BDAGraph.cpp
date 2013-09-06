/******************************************************************************
Este arquivo eh parte da implementacao das ferramentas DTV do TeleMidia

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
This file is part of the TeleMidia DTV Tools

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

#include "tuner/providers/BDAGraph.h"

/*****************************************************************************
* Constructor
*****************************************************************************/
namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
BDAGraph::BDAGraph(string channelsFile, Channels *channelsList) {
    guid_network_type = GUID_NULL;
    l_tuner_used = -1;
    systems = 0;
    d_graph_register = 0;
	searching = false;
	Thread::mutexInit(&bufferMutex, true);
	
	canReadNetworkName = false;
	currentNetworkName.assign("Unknown");
	currentVirtualChannel = 0;
	this->channelsFile.assign(channelsFile);
	this->channelsList = channelsList;
	this->channelsList->loadFromFile(channelsFile);
	tunedFreq = -1;

    p_media_control = NULL;
	p_tuning_space = NULL;
    p_filter_graph = NULL;
    p_system_dev_enum = NULL;
    p_network_provider = p_tuner_device = p_capture_device = NULL;
    p_sample_grabber = p_mpeg_demux = p_transport_info = NULL;
    p_scanning_tuner = NULL;
    p_grabber = NULL;

    /* Initialize COM - MS says to use CoInitializeEx in preference to
     * CoInitialize */
    CoInitializeEx( 0, COINIT_APARTMENTTHREADED );
}

/*****************************************************************************
* Destructor
*****************************************************************************/
BDAGraph::~BDAGraph() {
    Destroy();
	Thread::mutexDestroy(&bufferMutex);

    if (p_tuning_space) p_tuning_space->Release();
    p_tuning_space = NULL;

    systems = 0;
    CoUninitialize();
}

/*****************************************************************************
 * Enumerate Systems
 *****************************************************************************
 * here is logic for special case where user uses an MRL that points
 * to DTV but is not fully specific. This is usually dvb:// and can come
 * either from a playlist, a channels.conf MythTV file, or from manual entry.
 *
 * Since this is done before the real tune request is submitted, we can
 * use the global device enumerator, etc., so long as we do a Destroy() at
 * the end
 *****************************************************************************/
unsigned BDAGraph::EnumSystems() {
    HRESULT hr = S_OK;
    GUID guid_network_provider = GUID_NULL;

    guid_network_provider = CLSID_DVBTNetworkProvider;
    hr = Check(guid_network_provider);
    if (FAILED(hr))
		clog << "BDAGraph::EnumSystems: Check failed, trying next" << endl;

    if (p_filter_graph) Destroy();
    return systems;
}

long BDAGraph::getSignalStrength(void) {
    HRESULT hr = S_OK;
    long l_strength = 0;
    if (!p_scanning_tuner) return 0;
    hr = p_scanning_tuner->get_SignalStrength(&l_strength);
    if (FAILED(hr)) {
		clog << "BDAGraph::GetSignalStrength: Cannot get value: " << hr << endl;
        return 0;
    }
    if (l_strength == -1) return -1;
    return l_strength / 100;
}

int BDAGraph::SubmitTuneRequest(void) {
    HRESULT hr;
    int i = 0;

    /* Build and Start the Graph. If a Tuner device is in use the graph will
     * fail to start. Repeated calls to Build will check successive tuner
     * devices */
    do {
        hr = Build();
        if (FAILED(hr)) {
			clog << "BDAGraph::SubmitTuneRequest: Cannot Build the Graph: hr=" << hr << endl;
            return -1;
        }

        hr = Start();
        if (FAILED(hr)) {
            clog << "BDAGraph::SubmitTuneRequest: Cannot Start the Graph, retrying: hr=" << hr << endl;
            ++i;
        }
    }
    while (hr != S_OK && i < 10); /* give up after 10 tries */

    if (FAILED(hr)) {
        clog << "BDAGraph::SubmitTuneRequest: Failed to Start the Graph: hr=" << hr << endl;
        return -1;
    }

    return 0;
}


/*****************************************************************************
* Set DVB-T
*
* This provides the tune request that everything else is built upon.
*
* Stores the tune request to the scanning tuner, where it is pulled out by
* dvb_tune a/k/a SubmitTuneRequest.
******************************************************************************/
HRESULT BDAGraph::SetDVBT(long l_frequency, long l_bandwidth) {
    HRESULT hr = S_OK;

    /* try to set p_scanning_tuner */
    hr = Check(CLSID_DVBTNetworkProvider);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot create Tuning Space: hr=" << hr << endl;
        return hr;
    }

	hr = changeChannelTo(l_frequency);
	if (FAILED(hr)) {
		clog << "BDAGraph::SetDVBT: Cannot set channel: hr=" << hr << endl;
		return hr;
	}

    return hr;
}

/*****************************************************************************
* SetUpTuner
******************************************************************************
* Sets up global p_scanning_tuner and sets guid_network_type according
* to the Network Type requested.
*
* Logic: if tuner is set up and is the right network type, use it.
* Otherwise, poll the tuner for the right tuning space. 
*
* Then set up a tune request and try to validate it. Finally, put
* tune request and tuning space to tuner
*
* on success, sets globals: p_scanning_tuner and guid_network_type
*
******************************************************************************/
HRESULT BDAGraph::SetUpTuner(REFCLSID guid_this_network_type) {
    HRESULT hr = S_OK;
    class localComPtr {
    public:
        ITuningSpaceContainer*      p_tuning_space_container;
        IEnumTuningSpaces*          p_tuning_space_enum;
        ITuningSpace*               p_test_tuning_space;
        ITuneRequest*               p_tune_request;
        IDVBTuneRequest*            p_dvb_tune_request;

        IDigitalCableTuneRequest*   p_cqam_tune_request;
        IATSCChannelTuneRequest*    p_atsc_tune_request;
        ILocator*                   p_locator;
        IDVBTLocator*               p_dvbt_locator;
        IDVBCLocator*               p_dvbc_locator;
        IDVBSLocator*               p_dvbs_locator;

        BSTR                        bstr_name;

        CLSID                       guid_test_network_type;
        char*                       psz_network_name;
        char*                       psz_bstr_name;
        int                         i_name_len;

        localComPtr():
            p_tuning_space_container(NULL),
            p_tuning_space_enum(NULL),
            p_test_tuning_space(NULL),
            p_tune_request(NULL),
            p_dvb_tune_request(NULL),
            p_cqam_tune_request(NULL),
            p_atsc_tune_request(NULL),
            p_locator(NULL),
            p_dvbt_locator(NULL),
            p_dvbc_locator(NULL),
            p_dvbs_locator(NULL),
            bstr_name(NULL),
            guid_test_network_type(GUID_NULL),
            psz_network_name(NULL),
            psz_bstr_name(NULL),
            i_name_len(0)
        {}
        ~localComPtr() {
            if (p_tuning_space_enum)
                p_tuning_space_enum->Release();
            if (p_tuning_space_container)
                p_tuning_space_container->Release();
            if (p_test_tuning_space)
                p_test_tuning_space->Release();
            if (p_tune_request)
                p_tune_request->Release();
            if (p_dvb_tune_request)
                p_dvb_tune_request->Release();
            if (p_cqam_tune_request)
                p_cqam_tune_request->Release();
            if (p_atsc_tune_request)
                p_atsc_tune_request->Release();
            if (p_locator)
                p_locator->Release();
            if (p_dvbt_locator)
                p_dvbt_locator->Release();
            if (p_dvbc_locator)
                p_dvbc_locator->Release();
            if (p_dvbs_locator)
                p_dvbs_locator->Release();
            SysFreeString(bstr_name);
            delete[] psz_bstr_name;
            free(psz_network_name);
        }
    } l;

    /* We shall test for a specific Tuning space name supplied on the command
     * line as dvb-network-name=xxx.
     * For some users with multiple cards and/or multiple networks this could
     * be useful. This allows us to reasonably safely apply updates to the
     * System Tuning Space in the registry without disrupting other streams. */

    //l.psz_network_name = var_GetNonEmptyString( p_access, "dvb-network-name" );
	l.psz_network_name = NULL;

    if (l.psz_network_name == 0) {
        l.psz_network_name = new char[1];
        *l.psz_network_name = '\0';
    }

    /* Tuner may already have been set up. If it is for the same
     * network type then all is well. Otherwise, reset the Tuning Space and get
     * a new one */
    if (!p_scanning_tuner) {
        clog << "BDAGraph::SetUpTuner: Cannot find scanning tuner" << endl;
        return E_FAIL;
    }

    if (p_tuning_space) {
        hr = p_tuning_space->get__NetworkType(&l.guid_test_network_type);
        if (FAILED(hr)) {
            clog << "BDAGraph::SetUpTuner: Cannot get network type: hr=" << hr << endl;
            l.guid_test_network_type = GUID_NULL;
        }

        if (l.guid_test_network_type == guid_this_network_type) {
            SysFreeString(l.bstr_name);

            hr = p_tuning_space->get_UniqueName(&l.bstr_name);
            if (FAILED(hr)) {
                /* should never fail on a good tuning space */
                clog << "BDAGraph::SetUpTuner: Cannot get UniqueName for Tuning Space: hr=" << hr << endl;
                goto NoTuningSpace;
            }

            /* Test for a specific Tuning space name supplied on the command
             * line as dvb-network-name=xxx */
            if (l.psz_bstr_name) delete[] l.psz_bstr_name;
            l.i_name_len = WideCharToMultiByte( CP_ACP, 0, l.bstr_name, -1,
                l.psz_bstr_name, 0, NULL, NULL );
            l.psz_bstr_name = new char[ l.i_name_len ];
            l.i_name_len = WideCharToMultiByte( CP_ACP, 0, l.bstr_name, -1,
                l.psz_bstr_name, l.i_name_len, NULL, NULL );

            /* if no name was requested on command line, or if the name
             * requested equals the name of this space, we are OK */
            if (*l.psz_network_name == '\0' ||
                strcmp(l.psz_network_name, l.psz_bstr_name ) == 0) {
                /* p_tuning_space and guid_network_type are already set */
                /* you probably already have a tune request, also */
                hr = p_scanning_tuner->get_TuneRequest(&l.p_tune_request);
                if (SUCCEEDED(hr)) {
                    return S_OK;
                }
                /* CreateTuneRequest adds l.p_tune_request to p_tuning_space
                 * l.p_tune_request->RefCount = 1 */
                hr = p_tuning_space->CreateTuneRequest(&l.p_tune_request);
                if (SUCCEEDED(hr)) {
                    return S_OK;
                }
                clog << "BDAGraph::SetUpTuner: Cannot Create Tune Request: hr=" << hr << endl;
               /* fall through to NoTuningSpace */
            }
        }

        /* else different guid_network_type */
    NoTuningSpace:
        if (p_tuning_space) p_tuning_space->Release();
        p_tuning_space = NULL;
        /* pro forma; should have returned S_OK if we created this */
        if (l.p_tune_request) l.p_tune_request->Release();
        l.p_tune_request = NULL;
    }


    /* p_tuning_space is null at this point; we have already
       returned S_OK if it was good. So find a tuning
       space on the scanning tuner. */

    if (l.p_tuning_space_enum) l.p_tuning_space_enum->Release();
    l.p_tuning_space_enum = NULL;

    hr = p_scanning_tuner->EnumTuningSpaces(&l.p_tuning_space_enum);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetUpTuner: Cannot create TuningSpaces Enumerator: hr=" << hr << endl;
        goto TryToClone;
    }

    do {
        l.guid_test_network_type = GUID_NULL;
        if (l.p_test_tuning_space)
            l.p_test_tuning_space->Release();
        l.p_test_tuning_space = NULL;
        if (p_tuning_space)
            p_tuning_space->Release();
        p_tuning_space = NULL;
        SysFreeString(l.bstr_name);
        if(!l.p_tuning_space_enum) break;
        hr = l.p_tuning_space_enum->Next(1, &l.p_test_tuning_space, NULL);
        if (hr != S_OK) break;
        hr = l.p_test_tuning_space->get__NetworkType(&l.guid_test_network_type);
        if (FAILED(hr)) {
            clog << "BDAGraph::SetUpTuner: Cannot get network type: hr=" << hr << endl;
            l.guid_test_network_type = GUID_NULL;
        }
        if (l.guid_test_network_type == guid_this_network_type) {

            SysFreeString(l.bstr_name);

            hr = l.p_test_tuning_space->get_UniqueName(&l.bstr_name);
            if (FAILED(hr)) {
                /* should never fail on a good tuning space */
                clog << "SetUpTuner: Cannot get UniqueName for Tuning Space: hr=" << hr << endl;
                continue;
            }
            if (l.psz_bstr_name)
                delete[] l.psz_bstr_name;
            l.i_name_len = WideCharToMultiByte(CP_ACP, 0, l.bstr_name, -1,
                l.psz_bstr_name, 0, NULL, NULL);
            l.psz_bstr_name = new char[l.i_name_len];
            l.i_name_len = WideCharToMultiByte(CP_ACP, 0, l.bstr_name, -1,
                l.psz_bstr_name, l.i_name_len, NULL, NULL);
            break;
        }

    }
    while(true);

    if (l.guid_test_network_type == GUID_NULL) {
        goto TryToClone;
    }

    hr = p_scanning_tuner->put_TuningSpace(l.p_test_tuning_space);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetUpTuner: cannot put tuning space: hr=" << hr << endl;
        goto TryToClone;
    }

    hr = l.p_test_tuning_space->get_DefaultLocator(&l.p_locator);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetUpTuner: cannot get default locator: hr=" << hr << endl;
        goto TryToClone;
    }

    hr = l.p_test_tuning_space->CreateTuneRequest(&l.p_tune_request);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetUpTuner: cannot create tune request: hr=" << hr << endl;
        goto TryToClone;
    }

    hr = l.p_tune_request->put_Locator(l.p_locator);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetUpTuner: cannot put locator: hr=" << hr << endl;
        goto TryToClone;
    }

    hr = p_scanning_tuner->Validate(l.p_tune_request);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetUpTuner: Tune Request cannot be validated: hr=" << hr << endl;
    }

    /* increments ref count for scanning tuner */
    hr = p_scanning_tuner->put_TuneRequest(l.p_tune_request);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetUpTuner: Cannot submit the tune request: hr=" << hr << endl;
        return hr;
    }

    return S_OK;

    /* Get the SystemTuningSpaces container
     * p_tuning_space_container->Refcount = 1  */
TryToClone:
    clog << "BDAGraph::SetUpTuner: won't try to clone " << endl;
    return E_FAIL;
}

HRESULT BDAGraph::changeChannelTo(long freq, bool setDefault) {
	HRESULT hr;
	ITuneRequest*       np_tune_request;
	IDVBTuneRequest*    np_dvb_tune_request;
	IDVBTLocator*       np_dvbt_locator;
	IDVBTuningSpace2*   np_dvb_tuning_space;

	if (!p_scanning_tuner) {
        clog << "BDAGraph::SetDVBT: Cannot get scanning tuner" << endl;
        return S_FALSE;
    }

    hr = p_scanning_tuner->get_TuneRequest(&np_tune_request);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot get Tune Request: hr=" << hr << endl;
        return hr;
    }

    hr = np_tune_request->QueryInterface(__uuidof(IDVBTuneRequest),
        reinterpret_cast<void**>(&np_dvb_tune_request));
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot QI for IDVBTuneRequest: hr=" << hr << endl;
        return hr;
    }

    np_dvb_tune_request->put_ONID(-1);
    np_dvb_tune_request->put_SID(-1);
    np_dvb_tune_request->put_TSID(-1);

    hr = p_scanning_tuner->get_TuningSpace(&p_tuning_space);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: cannot get tuning space: hr=" << hr << endl;
        return hr;
    }

    hr = p_tuning_space->QueryInterface(__uuidof(IDVBTuningSpace2),
        reinterpret_cast<void**>(&np_dvb_tuning_space));
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot QI for IDVBTuningSpace2: hr=" << hr << endl;
        return hr;
    }

    hr = ::CoCreateInstance(__uuidof(DVBTLocator), 0, CLSCTX_INPROC,
        __uuidof(IDVBTLocator), reinterpret_cast<void**>(&np_dvbt_locator));
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot create the DVBT Locator: hr=" << hr << endl;
        return hr;
    }

	hr = np_dvb_tuning_space->put_SystemType(DVB_Terrestrial);
    if (SUCCEEDED(hr) && freq > 0)
        np_dvbt_locator->put_CarrierFrequency(freq);
    hr = np_dvbt_locator->put_Bandwidth(FREQ_BANDWIDTH);

    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot set tuning parameters on Locator: hr=" << hr << endl;
        return hr;
    }

    hr = np_dvb_tune_request->put_Locator(np_dvbt_locator);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot put the locator: hr=" << hr << endl;
        return hr;
    }

    hr = p_scanning_tuner->Validate(np_dvb_tune_request);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Tune Request cannot be validated: hr=" << hr << endl;
    }
    /* increments ref count for scanning tuner */
    hr = p_scanning_tuner->put_TuneRequest(np_dvb_tune_request);
    if (FAILED(hr)) {
        clog << "BDAGraph::SetDVBT: Cannot put the tune request: hr=" << hr << endl;
        return hr;
    }

	if (setDefault) channelsList->setDefaultFreq(freq);

	if (np_tune_request) np_tune_request->Release();
	if (np_dvb_tune_request) np_dvb_tune_request->Release();
	if (np_dvbt_locator) np_dvbt_locator->Release();
	if (np_dvb_tuning_space) np_dvb_tuning_space->Release();

	return hr;
}

bool BDAGraph::getBuffer(Buffer** buffer) {
	Thread::mutexLock(&bufferMutex);
	if (!bufferList.size()) {
		Thread::mutexUnlock(&bufferMutex);
		return false;
	}
	*buffer = bufferList[0];
	bufferList.erase(bufferList.begin());
	Thread::mutexUnlock(&bufferMutex);
	return true;
}

/******************************************************************************
* Check
*******************************************************************************
* Check if tuner supports this network type
*
* on success, sets globals:
* systems, l_tuner_used, p_network_provider, p_scanning_tuner, p_tuner_device,
* p_tuning_space, p_filter_graph
******************************************************************************/
HRESULT BDAGraph::Check(REFCLSID guid_this_network_type) {
    HRESULT hr = S_OK;

    class localComPtr {
    public:
        ITuningSpaceContainer*  p_tuning_space_container;

        localComPtr():
             p_tuning_space_container(NULL)
        {};
        ~localComPtr() {
            if( p_tuning_space_container )
                p_tuning_space_container->Release();
        }
    } l;

    /* Note that the systems global is persistent across Destroy().
     * So we need to see if a tuner has been physically removed from
     * the system since the last Check. Before we do anything,
     * assume that this Check will fail and remove this network type
     * from systems. It will be restored if the Check passes.
     */

    systems &= ~DVB_T;


    /* If we have already have a filter graph, rebuild it*/
    if (p_filter_graph) Destroy();
    p_filter_graph = NULL;
    hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
        IID_IGraphBuilder, reinterpret_cast<void**>(&p_filter_graph));
    if (FAILED(hr)) {
        clog << "BDAGraph::Check: Cannot CoCreate IFilterGraph: hr=" << hr << endl;
        return hr;
    }

    /* First filter in the graph is the Network Provider and
     * its Scanning Tuner which takes the Tune Request */
    if (p_network_provider)
        p_network_provider->Release();
    p_network_provider = NULL;
    hr = ::CoCreateInstance(guid_this_network_type, NULL, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, reinterpret_cast<void**>(&p_network_provider));
    if (FAILED(hr)) {
        clog << "BDAGraph::Check: Cannot CoCreate Network Provider: hr=" << hr << endl;
        return hr;
    }

    hr = p_filter_graph->AddFilter(p_network_provider, L"Network Provider");
    if (FAILED(hr)) {
        clog << "BDAGraph::Check: Cannot load network provider: hr=" << hr << endl;
        return hr;
    }

    /* Add the Network Tuner to the Network Provider. On subsequent calls,
     * l_tuner_used will cause a different tuner to be selected.
     *
     * To select a specific device first get the parameter that nominates the
     * device (dvb-adapter) and use the value to initialise l_tuner_used.
     * Note that dvb-adapter is 1-based, while l_tuner_used is 0-based.
     * When FindFilter returns, check the contents of l_tuner_used.
     * If it is not what was selected, then the requested device was not
     * available, so return with an error. */

    long l_adapter = -1;
    //l_adapter = var_GetInteger( p_access, "dvb-adapter" );
    if(l_tuner_used < 0 && l_adapter >= 0)
        l_tuner_used = l_adapter - 1;

    /* If tuner is in cold state, we have to do a successful put_TuneRequest
     * before it will Connect. */
    if (p_scanning_tuner)
        p_scanning_tuner->Release();
    p_scanning_tuner = NULL;
    hr = p_network_provider->QueryInterface(__uuidof(IScanningTuner),
        reinterpret_cast<void**>(&p_scanning_tuner));
    if (FAILED(hr)) {
        clog << "BDAGraph::Check: Cannot QI Network Provider for Scanning Tuner: hr=" << hr << endl;
        return hr;
    }

    /* try to set up p_scanning_tuner */
    hr = SetUpTuner(guid_this_network_type);
    if (FAILED(hr)) {
        clog << "BDAGraph::Check: Cannot set up scanner in Check mode: hr=" << hr << endl;
        return hr;
    }

    hr = FindFilter(KSCATEGORY_BDA_NETWORK_TUNER, &l_tuner_used,
        p_network_provider, &p_tuner_device);
    if (FAILED(hr)) {
        clog << "BDAGraph::Check: Cannot load tuner device and connect network provider: hr=" << hr << endl;
        return hr;
    }

    if (l_adapter > 0 && l_tuner_used != l_adapter) {
         clog << "BDAGraph::Check: Tuner device %ld is not available" << l_adapter << endl;
         return E_FAIL;
    }

    /* success!
     * already set l_tuner_used,
     * p_tuning_space
     */
    systems |= DVB_T;
    return S_OK;
}


/******************************************************************************
* Build
*******************************************************************************
* Build the Filter Graph
*
* connects filters and
* creates the media control and registers the graph
* on success, sets globals:
* d_graph_register, p_media_control, p_grabber, p_sample_grabber,
* p_mpeg_demux, p_transport_info
******************************************************************************/
HRESULT BDAGraph::Build() {
    HRESULT			hr = S_OK;
    long            l_capture_used;
    long            l_tif_used;
    AM_MEDIA_TYPE   grabber_media_type;

    class localComPtr {
    public:
        ITuningSpaceContainer*  p_tuning_space_container;
        localComPtr():
            p_tuning_space_container(NULL)
        {};
        ~localComPtr() {
            if(p_tuning_space_container)
                p_tuning_space_container->Release();
        }
    } l;

    /* at this point, you've connected to a scanning tuner of the right
     * network type.
     */
    if (!p_scanning_tuner || !p_tuner_device) {
        clog << "BDAGraph::Build: Scanning Tuner does not exist" << endl;
        return E_FAIL;
    }

    hr = p_scanning_tuner->get_TuneRequest(&p_tune_request);
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: no tune request" << endl;
        return hr;
    }
    hr = p_scanning_tuner->get_TuningSpace(&p_tuning_space);
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: no tuning space" << endl;
        return hr;
    }
    hr = p_tuning_space->get__NetworkType(&guid_network_type);


    /* Always look for all capture devices to match the Network Tuner */
    l_capture_used = -1;
    hr = FindFilter(KSCATEGORY_BDA_RECEIVER_COMPONENT, &l_capture_used,
        p_tuner_device, &p_capture_device);
    if (FAILED(hr)) {
        /* Some BDA drivers do not provide a Capture Device Filter so force
         * the Sample Grabber to connect directly to the Tuner Device */
        clog << "BDAGraph::Build: Cannot find Capture device. Connect to tuner and AddRef() : hr=" << hr << endl;
        p_capture_device = p_tuner_device;
        p_capture_device->AddRef();
    }

    if (p_sample_grabber)
         p_sample_grabber->Release();
    p_sample_grabber = NULL;
    /* Insert the Sample Grabber to tap into the Transport Stream. */
    hr = ::CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, reinterpret_cast<void**>(&p_sample_grabber));
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot load Sample Grabber Filter: hr=" << hr << endl;
        return hr;
    }

    hr = p_filter_graph->AddFilter(p_sample_grabber, L"Sample Grabber");
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot add Sample Grabber Filter to graph: hr=" << hr << endl;
        return hr;
    }

    /* create the sample grabber */
    if (p_grabber)
        p_grabber->Release();
    p_grabber = NULL;
    hr = p_sample_grabber->QueryInterface(IID_ISampleGrabber,
        reinterpret_cast<void**>(&p_grabber));
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot QI Sample Grabber Filter: hr=" << hr << endl;
        return hr;
    }

    /* Try the possible stream type */
    hr = E_FAIL;
    for (int i = 0; i < 2; i++) {
        ZeroMemory(&grabber_media_type, sizeof(AM_MEDIA_TYPE));
        grabber_media_type.majortype = MEDIATYPE_Stream;
        grabber_media_type.subtype   =  i == 0 ? MEDIASUBTYPE_MPEG2_TRANSPORT : KSDATAFORMAT_SUBTYPE_BDA_MPEG2_TRANSPORT;
		clog << endl;
        hr = p_grabber->SetMediaType(&grabber_media_type);
        if (SUCCEEDED(hr)) {
            hr = Connect(p_capture_device, p_sample_grabber);
            if (SUCCEEDED(hr)) {
                break;
            }
            //clog << "Build: Cannot connect Sample Grabber to Capture device: hr=" << hr << "(try %d/2)" << 1+i << endl;
        } else {
            clog << "BDAGraph::Build: Cannot set media type on grabber filter: hr=" << hr << "(try %d/2)" << 1+i << endl;
        }
    }
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot use capture device: hr=" << hr << endl;
        return hr;
    }

    /* We need the MPEG2 Demultiplexer even though we are going to use the VLC
     * TS demuxer. The TIF filter connects to the MPEG2 demux and works with
     * the Network Provider filter to set up the stream */
    if (p_mpeg_demux)
        p_mpeg_demux->Release();
    p_mpeg_demux = NULL;
    hr = ::CoCreateInstance(CLSID_MPEG2Demultiplexer, NULL,
        CLSCTX_INPROC_SERVER, IID_IBaseFilter,
        reinterpret_cast<void**>(&p_mpeg_demux));
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot CoCreateInstance MPEG2 Demultiplexer: hr=" << hr << endl;
        return hr;
    }

    hr = p_filter_graph->AddFilter(p_mpeg_demux, L"Demux");
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot add demux filter to graph: hr=" << hr << endl;
        return hr;
    }

    hr = Connect(p_sample_grabber, p_mpeg_demux);
    if(FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot connect demux to grabber: hr=" << hr << endl;
        return hr;
    }

    /* Always look for the Transport Information Filter from the start
     * of the collection */
    l_tif_used = -1;

    hr = FindFilter(KSCATEGORY_BDA_TRANSPORT_INFORMATION, &l_tif_used,
        p_mpeg_demux, &p_transport_info);
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot load TIF onto demux: hr=" << hr << endl;
        return hr;
    }

    /* Configure the Sample Grabber to buffer the samples continuously */
    hr = p_grabber->SetBufferSamples(true);
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot set Sample Grabber to buffering: hr=" << hr << endl;
        return hr;
    }

    hr = p_grabber->SetOneShot(false);
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot set Sample Grabber to multi shot: hr=" << hr << endl;
        return hr;
    }

    /* Second parameter to SetCallback specifies the callback method; 0 uses
     * the ISampleGrabberCB::SampleCB method, which receives an IMediaSample
     * pointer. */
    hr = p_grabber->SetCallback(this, 0);
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot set SampleGrabber Callback: hr=" << hr << endl;
        return hr;
    }

    hr = Register(); /* creates d_graph_register */
    if (FAILED(hr)) {
        d_graph_register = 0;
        clog << "BDAGraph::Build: Cannot register graph: hr=" << hr << endl;
    }

    /* The Media Control is used to Run and Stop the Graph */
    if (p_media_control)
        p_media_control->Release();
    p_media_control = NULL;
    hr = p_filter_graph->QueryInterface(IID_IMediaControl,
        reinterpret_cast<void**>(&p_media_control));
    if (FAILED(hr)) {
        clog << "BDAGraph::Build: Cannot QI Media Control: hr=" << hr << endl;
        return hr;
    }

    /* success! */
    return S_OK;
}

/* debugging */
HRESULT BDAGraph::ListFilters(REFCLSID this_clsid) {
    HRESULT                 hr = S_OK;

    class localComPtr {
    public:
        ICreateDevEnum*    p_local_system_dev_enum;
        IEnumMoniker*      p_moniker_enum;
        IMoniker*          p_moniker;
        IBaseFilter*       p_filter;
        IBaseFilter*       p_this_filter;
        IBindCtx*          p_bind_context;
        IPropertyBag*      p_property_bag;

        char*              psz_downstream;
        char*              psz_bstr;
        int                i_bstr_len;

        localComPtr():
            p_local_system_dev_enum(NULL),
            p_moniker_enum(NULL),
            p_moniker(NULL),
            p_filter(NULL),
            p_this_filter(NULL),
            p_bind_context(NULL),
            p_property_bag(NULL),
            psz_downstream(NULL),
            psz_bstr(NULL)
        {}
        ~localComPtr() {
            if (p_property_bag)
                p_property_bag->Release();
            if (p_bind_context)
                p_bind_context->Release();
            if (p_filter)
                p_filter->Release();
            if (p_this_filter)
                p_this_filter->Release();
            if (p_moniker)
                p_moniker->Release();
            if (p_moniker_enum)
                p_moniker_enum->Release();
            if (p_local_system_dev_enum)
                p_local_system_dev_enum->Release();
            if (psz_bstr)
                delete[] psz_bstr;
            if (psz_downstream)
                delete[] psz_downstream;
        }
    } l;


    if (l.p_local_system_dev_enum)
        l.p_local_system_dev_enum->Release();
    l.p_local_system_dev_enum = NULL;
    hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC,
        IID_ICreateDevEnum, reinterpret_cast<void**>(&l.p_local_system_dev_enum));
    if (FAILED(hr)) {
        clog << "BDAGraph::ListFilters: Cannot CoCreate SystemDeviceEnum: hr=" << hr << endl;
        return hr;
    }

    if (l.p_moniker_enum)
        l.p_moniker_enum->Release();
    l.p_moniker_enum = NULL;
    hr = l.p_local_system_dev_enum->CreateClassEnumerator(this_clsid,
        &l.p_moniker_enum, 0);
    if (FAILED(hr)) {
        clog << "BDAGraph::ListFilters: Cannot CreateClassEnumerator: hr=" << hr << endl;
        return hr;
    }

    do {
        /* We are overwriting l.p_moniker so we should Release and nullify
         * It is important that p_moniker and p_property_bag are fully released
         * l.p_filter may not be dereferenced so we could force to NULL */
        if (l.p_property_bag)
            l.p_property_bag->Release();
        l.p_property_bag = NULL;
        if (l.p_filter)
            l.p_filter->Release();
        l.p_filter = NULL;
        if (l.p_bind_context)
           l.p_bind_context->Release();
        l.p_bind_context = NULL;
        if (l.p_moniker)
            l.p_moniker->Release();
        l.p_moniker = NULL;

        if (!l.p_moniker_enum) break;
        hr = l.p_moniker_enum->Next(1, &l.p_moniker, 0);
        if (hr != S_OK) break;

        /* l.p_bind_context is Released at the top of the loop */
        hr = CreateBindCtx(0, &l.p_bind_context);
        if (FAILED(hr)) {
            clog << "BDAGraph::ListFilters: Cannot create bind_context, trying another: hr=" << hr << endl;
            continue;
        }

        /* l.p_filter is Released at the top of the loop */
        hr = l.p_moniker->BindToObject(l.p_bind_context, NULL, IID_IBaseFilter,
            reinterpret_cast<void**>(&l.p_filter));
        if (FAILED(hr)) {
            clog << "BDAGraph::ListFilters: Cannot create p_filter, trying another: hr=" << hr << endl;
            continue;
        }

#ifdef DEBUG_MONIKER_NAME
        WCHAR*  pwsz_downstream = NULL;

        hr = l.p_moniker->GetDisplayName(l.p_bind_context, NULL,
            &pwsz_downstream );
        if (FAILED(hr)) {
            clog << "BDAGraph::ListFilters: Cannot get display name, trying another: hr=" << hr << endl;
            continue;
        }

        if (l.psz_downstream)
            delete[] l.psz_downstream;
        l.i_bstr_len = WideCharToMultiByte(CP_ACP, 0, pwsz_downstream, -1,
            l.psz_downstream, 0, NULL, NULL);
        l.psz_downstream = new char[l.i_bstr_len];
        l.i_bstr_len = WideCharToMultiByte(CP_ACP, 0, pwsz_downstream, -1,
            l.psz_downstream, l.i_bstr_len, NULL, NULL);

        LPMALLOC p_alloc;
        ::CoGetMalloc(1, &p_alloc);
        p_alloc->Free(pwsz_downstream);
        p_alloc->Release();
        clog << "BDAGraph::ListFilters: Moniker name is  %s" <<  l.psz_downstream << endl;
#else
        l.psz_downstream = _strdup("Downstream");
#endif
        /* l.p_property_bag is released at the top of the loop */
        hr = l.p_moniker->BindToStorage(NULL, NULL, IID_IPropertyBag,
            reinterpret_cast<void**>(&l.p_property_bag));
        if (FAILED(hr)) {
            clog << "BDAGraph::ListFilters: Cannot Bind to Property Bag: hr=" << hr << endl;
            continue;
        }

        //clog << "ListFilters: displaying another" );
    }
    while(true);

    return S_OK;
}

/******************************************************************************
* FindFilter
* Looks up all filters in a category and connects to the upstream filter until
* a successful match is found. The index of the connected filter is returned.
* On subsequent calls, this can be used to start from that point to find
* another match.
* This is used when the graph does not run because a tuner device is in use so
* another one needs to be selected.
******************************************************************************/
HRESULT BDAGraph::FindFilter(REFCLSID this_clsid, long* i_moniker_used,
    IBaseFilter* p_upstream, IBaseFilter** p_p_downstream) {
    HRESULT                 hr = S_OK;
    int                     i_moniker_index = -1;
    class localComPtr {
    public:
        IEnumMoniker*  p_moniker_enum;
        IMoniker*      p_moniker;
        IBindCtx*      p_bind_context;
        IPropertyBag*  p_property_bag;
        char*          psz_upstream;
        int            i_upstream_len;

        char*          psz_downstream;
        VARIANT        var_bstr;
        int            i_bstr_len;
        localComPtr():
            p_moniker_enum(NULL),
            p_moniker(NULL),
            p_bind_context(NULL),
            p_property_bag(NULL),
            psz_upstream(NULL),
            psz_downstream(NULL) {
            ::VariantInit(&var_bstr);
        }
        ~localComPtr() {
            if (p_moniker_enum)
                p_moniker_enum->Release();
            if (p_moniker)
                p_moniker->Release();
            if (p_bind_context)
                p_bind_context->Release();
            if (p_property_bag)
                p_property_bag->Release();
            if (psz_upstream)
                delete[] psz_upstream;
            if (psz_downstream)
                delete[] psz_downstream;

            ::VariantClear(&var_bstr);
        }
    } l;

    /* create system_dev_enum the first time through, or preserve the
     * existing one to loop through classes */
    if (!p_system_dev_enum) {
        hr = ::CoCreateInstance(CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC,
            IID_ICreateDevEnum, reinterpret_cast<void**>(&p_system_dev_enum));
        if (FAILED(hr)) {
            clog << "BDAGraph::FindFilter: Cannot CoCreate SystemDeviceEnum: hr=" << hr << endl;
            return hr;
        }
    }

    hr = p_system_dev_enum->CreateClassEnumerator(this_clsid,
        &l.p_moniker_enum, 0);
    if (FAILED(hr)) {
        clog << "BDAGraph::FindFilter: Cannot CreateClassEnumerator: hr=" << hr << endl;
        return hr;
    }

    hr = GetFilterName(p_upstream, &l.psz_upstream);
    if (FAILED(hr)) {
        clog << "BDAGraph::FindFilter: Cannot GetFilterName: hr=" << hr << endl;
        return hr;
    }

    do {
        /* We are overwriting l.p_moniker so we should Release and nullify
         * It is important that p_moniker and p_property_bag are fully released */
        if (l.p_property_bag)
            l.p_property_bag->Release();
        l.p_property_bag = NULL;
        if (l.p_bind_context)
           l.p_bind_context->Release();
        l.p_bind_context = NULL;
        if (l.p_moniker)
            l.p_moniker->Release();
        l.p_moniker = NULL;

        if (!l.p_moniker_enum) break;
        hr = l.p_moniker_enum->Next(1, &l.p_moniker, 0);
        if (hr != S_OK) break;

        i_moniker_index++;

        /* Skip over devices already found on previous calls */

        if (i_moniker_index <= *i_moniker_used) continue;
        *i_moniker_used = i_moniker_index;

        /* l.p_bind_context is Released at the top of the loop */
        hr = CreateBindCtx(0, &l.p_bind_context);
        if (FAILED(hr)) {
            clog << "BDAGraph::FindFilter: Cannot create bind_context, trying another: hr=" << hr << endl;
            continue;
        }

        *p_p_downstream = NULL;
        hr = l.p_moniker->BindToObject(l.p_bind_context, NULL, IID_IBaseFilter,
            reinterpret_cast<void**>(p_p_downstream));
        if (FAILED(hr)) {
            clog << "BDAGraph::FindFilter: Cannot bind to downstream, trying another: hr=" << hr << endl;
            continue;
        }

#ifdef DEBUG_MONIKER_NAME
        clog << "BDAGraph::FindFilter: get downstream filter name" << endl;

        WCHAR*  pwsz_downstream = NULL;

        hr = l.p_moniker->GetDisplayName(l.p_bind_context, NULL,
            &pwsz_downstream);
        if (FAILED(hr)) {
            clog << "BDAGraph::FindFilter: Cannot get display name, trying another: hr=" << hr << endl;
            continue;
        }

        if (l.psz_downstream)
            delete[] l.psz_downstream;
        l.i_bstr_len = WideCharToMultiByte(CP_ACP, 0, pwsz_downstream, -1,
            l.psz_downstream, 0, NULL, NULL);
        l.psz_downstream = new char[l.i_bstr_len];
        l.i_bstr_len = WideCharToMultiByte(CP_ACP, 0, pwsz_downstream, -1,
            l.psz_downstream, l.i_bstr_len, NULL, NULL);

        LPMALLOC p_alloc;
        ::CoGetMalloc(1, &p_alloc);
        p_alloc->Free(pwsz_downstream);
        p_alloc->Release();
#else
        l.psz_downstream = _strdup("Downstream");
#endif

        /* l.p_property_bag is released at the top of the loop */
        hr = l.p_moniker->BindToStorage(l.p_bind_context, NULL,
            IID_IPropertyBag, reinterpret_cast<void**>(&l.p_property_bag));
        if (FAILED(hr)) {
            clog << "BDAGraph::FindFilter: Cannot Bind to Property Bag: hr=" << hr << endl;
            continue;
        }

        hr = l.p_property_bag->Read(L"FriendlyName", &l.var_bstr, NULL);
        if (FAILED(hr)) {
           clog << "BDAGraph::FindFilter: Cannot read friendly name, next?: hr=" << hr << endl;
           continue;
        }

        hr = p_filter_graph->AddFilter(*p_p_downstream, l.var_bstr.bstrVal);
        if (FAILED(hr)) {
            clog << "BDAGraph::FindFilter: Cannot add filter, trying another: hr=" << hr << endl;
            continue;
        }

        hr = Connect(p_upstream, *p_p_downstream);
        if (SUCCEEDED(hr)) {
            return S_OK;
        }

        /* Not the filter we want so unload and try the next one */
        /* Warning: RemoveFilter does an undocumented Release()
         * on pointer but does not set it to NULL */
        hr = p_filter_graph->RemoveFilter(*p_p_downstream);
        *p_p_downstream = NULL;
        if (FAILED(hr)) {
            clog << "BDAGraph::FindFilter: Failed unloading Filter: hr=" << hr << endl;
            continue;
        }
    }
    while(true);

    /* nothing found */
    clog << "BDAGraph::FindFilter: No filter connected" << endl;
    hr = E_FAIL;
    return hr;
}

/*****************************************************************************
* Connect is called from Build to enumerate and connect pins
*****************************************************************************/
HRESULT BDAGraph::Connect( IBaseFilter* p_upstream, IBaseFilter* p_downstream ) {
    HRESULT             hr = E_FAIL;
    class localComPtr {
    public:
        IPin*      p_pin_upstream;
        IPin*      p_pin_downstream;
        IEnumPins* p_pin_upstream_enum;
        IEnumPins* p_pin_downstream_enum;
        IPin*      p_pin_temp;
        char*      psz_upstream;
        char*      psz_downstream;

        localComPtr():
            p_pin_upstream(NULL), p_pin_downstream(NULL),
            p_pin_upstream_enum(NULL), p_pin_downstream_enum(NULL),
            p_pin_temp(NULL),
            psz_upstream(NULL),
            psz_downstream(NULL) 
            { };
        ~localComPtr() {
            if (p_pin_temp)
                p_pin_temp->Release();
            if (p_pin_downstream)
                p_pin_downstream->Release();
            if (p_pin_upstream)
                p_pin_upstream->Release();
            if (p_pin_downstream_enum)
                p_pin_downstream_enum->Release();
            if (p_pin_upstream_enum)
                p_pin_upstream_enum->Release();
            if (psz_upstream)
                delete[] psz_upstream;
            if (psz_downstream)
                delete[] psz_downstream;
        }
    } l;

    PIN_DIRECTION pin_dir;

    hr = p_upstream->EnumPins(&l.p_pin_upstream_enum);
    if (FAILED(hr)) {
        clog << "BDAGraph::Connect: Cannot get upstream filter enumerator: hr=" << hr << endl;
        return hr;
    }

    do {
        /* Release l.p_pin_upstream before next iteration */
        if (l.p_pin_upstream)
            l.p_pin_upstream ->Release();
        l.p_pin_upstream = NULL;
        if (!l.p_pin_upstream_enum) break;
        hr = l.p_pin_upstream_enum->Next(1, &l.p_pin_upstream, 0);
        if(hr != S_OK) break;

        hr = GetPinName(l.p_pin_upstream, &l.psz_upstream);
        if (FAILED(hr)) {
            clog << "BDAGraph::Connect: Cannot GetPinName: hr=" << hr << endl;
            return hr;
        }

        hr = l.p_pin_upstream->QueryDirection(&pin_dir);
        if (FAILED(hr)) {
            clog << "BDAGraph::Connect: Cannot get upstream filter pin direction: hr=" << hr << endl;
            return hr;
        }

        hr = l.p_pin_upstream->ConnectedTo(&l.p_pin_downstream);
        if (SUCCEEDED(hr)) {
            l.p_pin_downstream->Release();
            l.p_pin_downstream = NULL;
        }

        if (FAILED(hr) && hr != VFW_E_NOT_CONNECTED) {
            clog << "BDAGraph::Connect: Cannot check upstream filter connection: hr=" << hr << endl;
            return hr;
        }

        if ((pin_dir == PINDIR_OUTPUT) && (hr == VFW_E_NOT_CONNECTED)) {
            /* The upstream pin is not yet connected so check each pin on the
             * downstream filter */
            hr = p_downstream->EnumPins(&l.p_pin_downstream_enum);
            if (FAILED(hr)) {
                clog << "BDAGraph::Connect: Cannot get downstream filter enumerator: hr=" << hr << endl;
                return hr;
            }

            do {
                /* Release l.p_pin_downstream before next iteration */
                if (l.p_pin_downstream)
                    l.p_pin_downstream ->Release();
                l.p_pin_downstream = NULL;
                if (!l.p_pin_downstream_enum) break;
                hr = l.p_pin_downstream_enum->Next(1, &l.p_pin_downstream, 0);
                if (hr != S_OK) break;

                hr = GetPinName(l.p_pin_downstream, &l.psz_downstream);
                if (FAILED(hr)) {
                    clog << "BDAGraph::Connect: Cannot GetPinName: hr=" << hr << endl;
                    return hr;
                }

                hr = l.p_pin_downstream->QueryDirection(&pin_dir);
                if (FAILED(hr)) {
                    clog << "BDAGraph::Connect: Cannot get downstream filter pin direction: hr=" << hr << endl;
                    return hr;
                }

                /* Looking for a free Pin to connect to
                 * A connected Pin may have an reference count > 1
                 * so Release and nullify the pointer */
                hr = l.p_pin_downstream->ConnectedTo(&l.p_pin_temp);
                if (SUCCEEDED(hr)) {
                    l.p_pin_temp->Release();
                    l.p_pin_temp = NULL;
                }

                if (hr != VFW_E_NOT_CONNECTED) {
                    if (FAILED(hr)) {
                        clog << "BDAGraph::Connect: Cannot check downstream filter connection: hr=" << hr << endl;
                        return hr;
                    }
                }

                if ((pin_dir == PINDIR_INPUT) &&
                    (hr == VFW_E_NOT_CONNECTED)) {

                    hr = p_filter_graph->ConnectDirect(l.p_pin_upstream,
                        l.p_pin_downstream, NULL);
                    if (SUCCEEDED(hr)) {
                        /* If we arrive here then we have a matching pair of
                         * pins. */
                        return S_OK;
                    }
                }
                /* If we arrive here it means this downstream pin is not
                 * suitable so try the next downstream pin.
                 * l.p_pin_downstream is released at the top of the loop */
            }
            while(true);
            /* If we arrive here then we ran out of pins before we found a
             * suitable one. Release outstanding refcounts */
            if (l.p_pin_downstream_enum)
                l.p_pin_downstream_enum->Release();
            l.p_pin_downstream_enum = NULL;
            if (l.p_pin_downstream)
                l.p_pin_downstream->Release();
            l.p_pin_downstream = NULL;
        }
        /* If we arrive here it means this upstream pin is not suitable
         * so try the next upstream pin
         * l.p_pin_upstream is released at the top of the loop */
    }
    while(true);
    /* If we arrive here it means we did not find any pair of suitable pins
     * Outstanding refcounts are released in the destructor */
    //clog << "Connect: No pins connected" );
    return E_FAIL;
}

/*****************************************************************************
* Start uses MediaControl to start the graph
*****************************************************************************/
HRESULT BDAGraph::Start() {
    HRESULT hr = S_OK;
    OAFilterState i_state; /* State_Stopped, State_Paused, State_Running */

    if (!p_media_control) {
        clog << "BDAGraph::Start: Media Control has not been created" << endl;
        return E_FAIL;
    }

    hr = p_media_control->Run();
    if (SUCCEEDED(hr)) {
        return S_OK;
    }

    clog << "BDAGraph::Start: would not start, will retry" << endl;
    /* Query the state of the graph - timeout after 100 milliseconds */
    while ((hr = p_media_control->GetState(100, &i_state)) != S_OK) {
        if (FAILED(hr)) {
            clog <<
                "BDAGraph::Start: Cannot get Graph state: hr=" << hr << endl;
            return hr;
        }
    }

    if (i_state == State_Running) {
        return S_OK;
    }

    /* The Graph is not running so stop it and return an error */
    clog << "BDAGraph::Start: Graph not started: " << (int)i_state << endl;
    hr = p_media_control->StopWhenReady(); /* Instead of Stop() */
    if (FAILED(hr)) {
        clog << "BDAGraph::Start: Cannot stop Graph after Run failed: hr=" << hr << endl;
        return hr;
    }

    return E_FAIL;
}

/******************************************************************************
* SampleCB - Callback when the Sample Grabber has a sample
******************************************************************************/
STDMETHODIMP BDAGraph::SampleCB(double /*date*/, IMediaSample *p_sample) {
    if (p_sample->IsDiscontinuity() == S_OK) {
        //clog << "BDA SampleCB: Sample Discontinuity." << endl;
		return S_OK;
	}

    const size_t i_sample_size = p_sample->GetActualDataLength();

    /* The buffer memory is owned by the media sample object, and is automatically
     * released when the media sample is destroyed. The caller should not free or
     * reallocate the buffer. */
    BYTE *p_sample_data;
    p_sample->GetPointer(&p_sample_data);

    if (i_sample_size > 0 && p_sample_data) {
		if (!searching) {
			Buffer* buf = new Buffer();
			buf->buffer = new char[i_sample_size];
			memcpy(buf->buffer, p_sample_data, i_sample_size);
			buf->len = i_sample_size;
			Thread::mutexLock(&bufferMutex);
			bufferList.push_back(buf);
			Thread::mutexUnlock(&bufferMutex);
		}
		if (canReadNetworkName) {
			readNI(p_sample_data, i_sample_size);
		}
    }
    return S_OK;
}

/******************************************************************************
* removes each filter from the graph
******************************************************************************/
HRESULT BDAGraph::Destroy() {
    HRESULT hr = S_OK;
    ULONG mem_ref = 0;

    if (p_media_control)
        p_media_control->StopWhenReady(); /* Instead of Stop() */

    if (d_graph_register)
        Deregister();

    if (p_transport_info) {
        /* Warning: RemoveFilter does an undocumented Release()
         * on pointer but does not set it to NULL */
        hr = p_filter_graph->RemoveFilter(p_transport_info);
        if (FAILED(hr)) {
            clog << "BDAGraph::Destroy: Failed unloading TIF: hr=" << hr << endl;
        }
        p_transport_info = NULL;
    }

    if (p_mpeg_demux) {
        p_filter_graph->RemoveFilter(p_mpeg_demux);
        if (FAILED(hr)) {
            clog << "BDAGraph::Destroy: Failed unloading demux: hr=" << hr << endl;
        }
        p_mpeg_demux = NULL;
    }

    if (p_grabber) {
        mem_ref = p_grabber->Release();
        p_grabber = NULL;
    }

    if (p_sample_grabber) {
        hr = p_filter_graph->RemoveFilter(p_sample_grabber);
        p_sample_grabber = NULL;
        if (FAILED(hr)) {
            clog << "BDAGraph::Destroy: Failed unloading sampler: hr=" << hr << endl;
        }
    }

    if (p_capture_device) {
        p_filter_graph->RemoveFilter(p_capture_device);
        if (FAILED(hr)) {
            clog << "BDAGraph::Destroy: Failed unloading capture device: hr=" << hr << endl;
        }
        p_capture_device = NULL;
    }

    if (p_tuner_device) {
        hr = p_filter_graph->RemoveFilter(p_tuner_device);
        p_tuner_device = NULL;
        if (FAILED(hr)) {
            clog << "BDAGraph::Destroy: Failed unloading tuner device: hr=" << hr << endl;
        }
    }

    if (p_scanning_tuner) {
        mem_ref = p_scanning_tuner->Release();
        p_scanning_tuner = NULL;
    }

    if (p_network_provider) {
        hr = p_filter_graph->RemoveFilter(p_network_provider);
        p_network_provider = NULL;
        if (FAILED(hr)) {
            clog << "BDAGraph::Destroy: Failed unloading net provider: hr=" << hr << endl;
        }
    }

    if (p_filter_graph) {
        mem_ref = p_filter_graph->Release();
        p_filter_graph = NULL;
    }

    /* first call to FindFilter creates p_system_dev_enum */

    if (p_system_dev_enum) {
        mem_ref = p_system_dev_enum->Release();
        p_system_dev_enum = NULL;
    }

    if (p_media_control) {
        mem_ref = p_media_control->Release();
        p_media_control = NULL;
    }

    d_graph_register = 0;
    l_tuner_used = -1;
    guid_network_type = GUID_NULL;

    return S_OK;
}

/*****************************************************************************
* Add/Remove a DirectShow filter graph to/from the Running Object Table.
* Allows GraphEdit to "spy" on a remote filter graph.
******************************************************************************/
HRESULT BDAGraph::Register() {
    class localComPtr {
    public:
        IMoniker*             p_moniker;
        IRunningObjectTable*  p_ro_table;
        localComPtr():
            p_moniker(NULL),
            p_ro_table(NULL)
        {};
        ~localComPtr() {
            if (p_moniker)
                p_moniker->Release();
            if (p_ro_table)
                p_ro_table->Release();
        }
    } l;
    WCHAR     pwsz_graph_name[128];
    HRESULT   hr;

    hr = ::GetRunningObjectTable(0, &l.p_ro_table);
    if (FAILED(hr)) {
        clog << "BDAGraph::Register: Cannot get ROT: hr=" << hr << endl;
        return hr;
    }

    size_t len = sizeof(pwsz_graph_name) / sizeof(pwsz_graph_name[0]);
    _snwprintf_s(pwsz_graph_name, len - 1, L"Ginga BDA Graph %08x Pid %08x",
        (DWORD_PTR) p_filter_graph, ::GetCurrentProcessId());
    pwsz_graph_name[len-1] = 0;
    hr = CreateItemMoniker(L"!", pwsz_graph_name, &l.p_moniker);
    if (FAILED(hr)) {
        clog << "BDAGraph::Register: Cannot Create Moniker: hr=" << hr << endl;
        return hr;
    }
    hr = l.p_ro_table->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE,
        p_filter_graph, l.p_moniker, &d_graph_register);
    if (FAILED(hr)) {
        clog << "BDAGraph::Register: Cannot Register Graph: hr=" << hr << endl;
        return hr;
    }

    clog << "BDAGraph::Register: registered Graph: " << pwsz_graph_name << endl;
    return hr;
}

void BDAGraph::Deregister() {
    HRESULT   hr;
    IRunningObjectTable* p_ro_table;
    hr = ::GetRunningObjectTable( 0, &p_ro_table );
    /* docs say this does a Release() on d_graph_register stuff */
    if( SUCCEEDED( hr ) )
        p_ro_table->Revoke( d_graph_register );
    d_graph_register = 0;
    p_ro_table->Release();
}

HRESULT BDAGraph::GetFilterName(IBaseFilter* p_filter, char** psz_bstr_name) {
    FILTER_INFO filter_info;
    HRESULT     hr = S_OK;

    hr = p_filter->QueryFilterInfo(&filter_info);
    if (FAILED(hr)) return hr;
    int i_name_len = WideCharToMultiByte(CP_ACP, 0, filter_info.achName,
        -1, *psz_bstr_name, 0, NULL, NULL);
    *psz_bstr_name = new char[i_name_len];
    i_name_len = WideCharToMultiByte(CP_ACP, 0, filter_info.achName,
        -1, *psz_bstr_name, i_name_len, NULL, NULL);

    // The FILTER_INFO structure holds a pointer to the Filter Graph
    // Manager, with a reference count that must be released.
    if (filter_info.pGraph)
        filter_info.pGraph->Release();
    return S_OK;
}

HRESULT BDAGraph::GetPinName(IPin* p_pin, char** psz_bstr_name) {
    PIN_INFO    pin_info;
    HRESULT     hr = S_OK;

    hr = p_pin->QueryPinInfo(&pin_info);
    if (FAILED(hr)) return hr;
    int i_name_len = WideCharToMultiByte(CP_ACP, 0, pin_info.achName,
        -1, *psz_bstr_name, 0, NULL, NULL);
    *psz_bstr_name = new char[i_name_len];
    i_name_len = WideCharToMultiByte(CP_ACP, 0, pin_info.achName,
        -1, *psz_bstr_name, i_name_len, NULL, NULL);

    // The PIN_INFO structure holds a pointer to the Filter,
    // with a referenppce count that must be released.
    if (pin_info.pFilter)
        pin_info.pFilter->Release();
    return S_OK;
}

STDMETHODIMP BDAGraph::BufferCB( double /*date*/, BYTE* /*buffer*/,
                                 long /*buffer_len*/ ) {
    return E_FAIL;
}

void BDAGraph::readNI(BYTE* buf, size_t bufLen) {
	if (clock() < rntStopTime) {
		unsigned int i = 0;
		while (i < bufLen) {
			if (buf[i] == 0x47) {
				unsigned short pid = (((buf[i+1] & 0x1F) << 8) | (buf[i+2] & 0xFF));
				if (pid == 0x10) { //NIT PID
					unsigned int j = buf[i+4];
					j = i + j + 5; //NIT begins
					unsigned int k = (((buf[j+8] & 0x0F) << 8) | (buf[j+9] & 0xFF));
					k = j + k + 10; //transport_stream_loop_length
					unsigned int m = (((buf[k] & 0x0F) << 8) | (buf[k+1] & 0xFF));
					if ((buf[j] == 0x40) && (buf[j+10] == 0x40)) { //NIT Table && Network name descriptor
						unsigned char len = buf[j+11];
						if (len > 0) {
							j = j + 12;
							if ((buf[j] < 0x20) || (buf[j] > 0x7E)) { //First character
								len--;
								j++;
							}
							if ((buf[j+len-1] < 0x20) || (buf[j+len-1] > 0x7E)) { //Last character
								len--;
							}
							currentNetworkName.assign((char*)(buf+j), len);
						}
					}
					if (m > 0) { //Limited to one network only
						k += 6; //transport_descriptors_length
						m = (((buf[k] & 0x0F) << 8) | (buf[k+1] & 0xFF));
						if (m > 0) {
							k += 2; // first descriptor in loop
							j = k;
							while (k < (j + m)) {
								if (buf[k] == 0xCD) {
									currentVirtualChannel = buf[k+2];
									canReadNetworkName = false;
									break;
								} else {
									k += buf[k+1] + 2;
								}
							}
						}
					}
				}
			}
			i += 188;
		}
	} else {
		canReadNetworkName = false;
	}
}

void BDAGraph::readNetworkInfo(clock_t stopTime) {
	rntStopTime = stopTime;
	currentNetworkName.assign("Unknown");
	currentVirtualChannel = 0;
	canReadNetworkName = true;
}

HRESULT BDAGraph::tryToTune() {
	searching = true;
	tunedFreq = channelsList->getDefaultFreq();
	if (tunedFreq > 0) {
		changeChannelTo(tunedFreq);
		Sleep(1500);
		if (getSignalStrength()) {
			searching = false;
			return S_OK;
		}
		else {
			tunedFreq = -1;
		}
	}
	long freq = 0;
	if (channelsList->getListSize()) {
		while (freq >= 0) {
			freq = channelsList->getNextFreq(freq);
			changeChannelTo(freq, true);
			Sleep(1500);
			if (getSignalStrength()) {
				channelsList->saveToFile(channelsFile);
				break;
			}
		}
	} else freq = -1;
	tunedFreq = freq;
	searching = false;
	if (freq == -1) {
		Destroy();
		cout << "cmd::1::tuner::Unable to tune any channel." << endl;
		return S_FALSE;
	}
	return S_OK;
}

HRESULT BDAGraph::searchChannels() {
	long freqBegin = FREQ_LOW;
	long freqEnd = FREQ_HIGH;
	long cProgress = 0, vp = 0;
	long mProgress = freqEnd - freqBegin;
	float ch = 14.0;
	long st = 0;
	bool setDefault = true;

	searching = true;
	channelsList->cleanList();
	cout << "cmd::0::tunerscanprogress::0%" << endl;
	while (1) {
		freqBegin += 3000;
		cProgress += 3000;
		ch += 0.5;
		if (ch == 37.0) { //Reserved to radio astronomy
			ch = 38.5;
			cProgress += 9000;
			freqBegin += 9000;
		}
		if (freqBegin >= freqEnd) {
			if (channelsList->getListSize()) {
				channelsList->saveToFile(channelsFile);
			} else {
				channelsList->loadFromFile(channelsFile);
			}
			break;
		}
		vp = (float(cProgress)/mProgress)*100;
		cout << "cmd::0::tunerscanprogress::" << vp << "%" << endl;
		changeChannelTo(freqBegin);

		Sleep(1500);
		st = getSignalStrength();
		if (st > 0) {
			readNetworkInfo(clock()+1500);
			Sleep(1500);
			cout << "cmd::0::channelfound::" << currentNetworkName << endl;
			channelsList->insertFreq(currentNetworkName, freqBegin, currentVirtualChannel);
			if (setDefault) {
				setDefault = false;
				channelsList->setDefaultFreq(freqBegin);
			}
			channelsList->saveToFile(channelsFile);
			vp = (float(cProgress)/mProgress)*100;
			cout << "cmd::0::tunerscanprogress::" << vp << "%" << endl;
		}
	}
	searching = false;
	tunedFreq = channelsList->getDefaultFreq();
	channelsList->saveToFile(channelsFile);
	cout << "cmd::0::tunerscanprogress::100%" << endl;
	Destroy();
	//changeChannelTo(channelsList->getDefaultFreq());
	//clog << "Tuned in " << channelsList->getName(tunedFreq) << "." << endl;
	return S_OK;
}

bool BDAGraph::initDevice() {
	HRESULT hr = SetDVBT(FREQ_LOW, FREQ_BANDWIDTH);
	if (FAILED(hr)) {
		cout << "cmd::1::tuner::Can't find tuner device." << endl;
		return false;
	}

	hr = SubmitTuneRequest();
	if (FAILED(hr)) {
		Destroy();
		cout << "cmd::1::tuner::Cannot submit tune request." << endl;
		return false;
	}

	return true;
}

HRESULT BDAGraph::execute(long freq) {
	if (freq > 0) {
		changeChannelTo(freq);
		Sleep(1500);
		if (getSignalStrength())  {
			tunedFreq = freq;
			channelsList->setDefaultFreq(tunedFreq);
			channelsList->saveToFile(channelsFile);
			return S_OK;
		} else {
			tunedFreq = -1;
			Destroy();
			cout << "cmd::1::tuner::Unable to tune." << endl;
			return S_FALSE;
		}
	}
	if ((freq == 0) && (tryToTune() == S_OK)) {
		return S_OK;
	}
	return S_FALSE;
}

long BDAGraph::getTunedFreq() {
	return tunedFreq;
}

}
}
}
}
}
}