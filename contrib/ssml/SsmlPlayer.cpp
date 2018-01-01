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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "ginga.h"

#include "SsmlPlayer.h"

// TODO: Develop a ginga common audio system and integrate this player in
// it.
// The eSpeak header

// size of the max read by the voice syntetizer
#define MAX_READ 100000

bool isRunning;
bool terminateSpeak;

// Callback method which delivers the synthetized audio samples and the
// events.
static int
SynthCallback (unused (short *wav), unused (int numsamples), unused (espeak_EVENT *events))
{
  if (terminateSpeak == true)
    return 1;

  return 0;
}

GINGA_NAMESPACE_BEGIN

SsmlPlayer::SsmlPlayer (const string &mrl)
    : Thread (), Player (mrl)
{
}

SsmlPlayer::~SsmlPlayer () {}

void
SsmlPlayer::setFile (const string &mrl)
{
  clog << "SsmlPlayer::setFile!! " << endl;

  if (mrl.length () > 5)
    {
      string fileType;

      this->mrl = mrl;
      fileType = this->mrl.substr (this->mrl.length () - 5, 5);
      if (fileType != ".ssml")
        {
          clog << "SsmlPlayer::loadFile Warning! Unknown file ";
          clog << "type for: '" << this->mrl << "'" << endl;
        }
    }
  else
    {
      clog << "SsmlPlayer::loadFile Warning! Unknown extension ";
      clog << "type for: '" << mrl << "'" << endl;
    }
}

// This method is the most important one. It sets up the audio synthesizer,
// the
// output audio device, reads the input SSML file and calls the apropriate
// methods to perform the audio synthesis and playback.
void
SsmlPlayer::loadSsml ()
{
  espeak_AUDIO_OUTPUT outType = AUDIO_OUTPUT_SYNCH_PLAYBACK;
  clog << "SsmlPlayer::loadSsml!! " << endl;
  ifstream fis;

  fis.open ((this->mrl).c_str (), ifstream::in);

  if (!fis.is_open () && (mrl != "" || content == ""))
    {
      clog << "SsmlPlayer::loadFile Warning! can't open input ";
      clog << "file: '" << this->mrl << "'" << endl;
      return;
    }

  if (isRunning == true)
    {
      terminateSpeak = true;
      while (isRunning == true)
        sleep (1);
    }

  espeak_Initialize (outType, MAX_READ, NULL, 0);
  isRunning = true;

  espeak_SetSynthCallback (SynthCallback);

  string line;
  do
    {
      if (terminateSpeak == true)
        break;

      getline (fis, line);
    }
  while (!fis.eof ());

  fis.close ();

  espeak_Synchronize ();
  espeak_Terminate ();

  if (terminateSpeak == false)
    notifyPlayerListeners (PL_NOTIFY_STOP, "");

  terminateSpeak = false;
  isRunning = false;
}

bool
SsmlPlayer::play ()
{
  clog << "SsmlPlayer::play ok" << endl;

  bool ret = Player::play ();
  Thread::startThread ();

  return ret;
}

void
SsmlPlayer::stop ()
{
  clog << "SsmlPlayer::stop ok" << endl;

  Player::stop ();
}

void
SsmlPlayer::resume ()
{
  SsmlPlayer::play ();
}

void
SsmlPlayer::setPropertyValue (const string &name, const string &value)
{
  Player::setPropertyValue (name, value);
}

void
SsmlPlayer::run ()
{
  clog << "SsmlPlayer::run thread created!" << endl;
  loadSsml ();
}

GINGA_NAMESPACE_END
