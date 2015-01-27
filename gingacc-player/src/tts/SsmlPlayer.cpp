/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2014 PUC-Rio/Laboratorio TeleMidia

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

Copyright: 1989-2014 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

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

#include <cstdlib>

#include "player/SsmlPlayer.h"

#include "player/PlayersComponentSupport.h"

// TODO: Develop a ginga common audio system and integrate this player in it.
// The eSpeak header
#include <espeak/speak_lib.h>

// size of the max read by the voice syntetizer
#define MAX_READ 100000

bool isRunning;
bool terminateSpeak;

// Callback method which delivers the synthetized audio samples and the events.
static int SynthCallback(short *wav, int numsamples, espeak_EVENT *events)
{
    if (terminateSpeak == true)
        return 1;

    return 0;
}


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

    SsmlPlayer::SsmlPlayer(GingaScreenID screenId, string mrl) :
        Thread(), Player(screenId, mrl) {

    }

    SsmlPlayer::~SsmlPlayer() {

    }

    void SsmlPlayer::setFile(string mrl) {
        clog << "SsmlPlayer::setFile!! " << endl;

        if (mrl == "" || !fileExists(mrl)) {
            clog << "SsmlPlayer::setFile Warning! File not found: '";
            clog << mrl << "'" << endl;
            return;
        }

        if (mrl.length() > 5) {
            string fileType;
	
            this->mrl = mrl;
            fileType = this->mrl.substr(this->mrl.length() - 5, 5);
            if (fileType != ".ssml") {
                clog << "SsmlPlayer::loadFile Warning! Unknown file ";
                clog << "type for: '" << this->mrl << "'" << endl;
            }

        } else {
            clog << "SsmlPlayer::loadFile Warning! Unknown extension ";
            clog << "type for: '" << mrl << "'" << endl;
        }
    }

    // This method is the most important one. It sets up the audio synthesizer, the
    // output audio device, reads the input SSML file and calls the apropriate 
    // methods to perform the audio synthesis and playback.
    void SsmlPlayer::loadSsml() {
        espeak_AUDIO_OUTPUT outType = AUDIO_OUTPUT_SYNCH_PLAYBACK;
        espeak_POSITION_TYPE pType = POS_CHARACTER;
        espeak_VOICE voiceType;
        espeak_ERROR errType = EE_OK;
        int sampleRate = 0;
        clog << "SsmlPlayer::loadSsml!! " << endl;

        voiceType.name = NULL;
        // TODO: Hardcoded to Brazilian Portuguese
        voiceType.languages = "pt-br";
        voiceType.gender = 0;
        voiceType.age = 0;
        voiceType.variant = 0;
        
        ifstream fis;

        fis.open((this->mrl).c_str(), ifstream::in);

        if (!fis.is_open() && (mrl != "" || content == "")) {
            clog << "SsmlPlayer::loadFile Warning! can't open input ";
            clog << "file: '" << this->mrl << "'" << endl;
            return;
        }

        if (isRunning == true) {
            terminateSpeak = true;
            while (isRunning == true)
                sleep (1);
        }
        
        sampleRate = espeak_Initialize(outType, MAX_READ, NULL, 0);
        isRunning = true;

        errType = espeak_SetVoiceByProperties(&voiceType);
        
        espeak_SetSynthCallback(SynthCallback);

        string line;
        do {

            if (terminateSpeak == true)
                break;
            
            getline (fis, line);
            errType = espeak_Synth(line.c_str(),
                     line.length(),
                     0,
                     pType,
                     0,
                     espeakSSML,
                     NULL,
                     NULL);

        } while (!fis.eof());

        fis.close();
        
        espeak_Synchronize();
        espeak_Terminate();

        if (terminateSpeak == false)
            notifyPlayerListeners(PL_NOTIFY_STOP, "");
        
        terminateSpeak = false;
        isRunning = false; 
       

    }

    bool SsmlPlayer::play() {
        clog << "SsmlPlayer::play ok" << endl;

        bool ret = Player::play();
        Thread::startThread();

        return ret;

    }

    void SsmlPlayer::stop() {
        clog << "SsmlPlayer::stop ok" << endl;

        Player::stop();
    }


    void SsmlPlayer::resume() {
        SsmlPlayer::play();
    }


    void SsmlPlayer::setPropertyValue(string name, string value) {
        Player::setPropertyValue(name, value);
        
    }

    void SsmlPlayer::run() {
        clog << "SsmlPlayer::run thread created!" << endl;
        loadSsml();
        
    }

    
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
createSsmlPlayer(
    GingaScreenID screenId, const char* mrl, bool hasVisual) {
    
    return (new ::br::pucrio::telemidia::ginga::core::player::
            SsmlPlayer(screenId, (string)mrl));
}

extern "C" void destroySsmlPlayer(
    ::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {
    
    delete p;
}
