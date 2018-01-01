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

#ifndef TSADAPTATIONFIELD_H_
#define TSADAPTATIONFIELD_H_

#define MAX_ADAPTATION_FIELD_SIZE 183
#define MAX_PRIVATE_DATA_SIZE 255

#include "system/Stc.h"
using namespace ::ginga::system;

GINGA_TSPARSER_BEGIN

class TSAdaptationField
{
private:
  char adapFieldStream[MAX_ADAPTATION_FIELD_SIZE];
  char *privateDataStream;
  bool streamUpdated;
  bool fixedSize;
  void init ();
  void updateStream ();
  void process ();

protected:
  unsigned char adaptationFieldLength;
  unsigned char discontinuityIndicator;
  unsigned char randomAccessIndicator;
  unsigned char elementaryStreamPriorityIndicator;
  unsigned char pcrFlag;
  unsigned char oPCRFlag;
  unsigned char splicingPointFlag;
  unsigned char transportPrivateDataFlag;
  unsigned char adaptationFieldExtensionFlag;
  uint64_t pcrBase;
  uint64_t pcrExtension;
  uint64_t originalPcrBase;
  uint64_t originalPcrExtension;
  char spliceCountdown;
  unsigned char transportPrivateDataLength;
  unsigned char adaptationFieldExtensionLength;
  unsigned char ltwFlag;
  unsigned char piecewiseRateFlag;
  unsigned char seamlessSpliceFlag;
  unsigned char ltwValidFlag;
  unsigned int ltwOffset;
  unsigned int piecewiseRate;
  unsigned char spliceType;
  uint64_t dtsNextAU;

public:
  TSAdaptationField ();
  TSAdaptationField (uint64_t pcr);
  TSAdaptationField (uint64_t pcrBase, uint64_t pcrExtension);
  TSAdaptationField (char adapField[MAX_ADAPTATION_FIELD_SIZE]);
  ~TSAdaptationField ();

private:
  void releasePrivateDataStream ();

public:
  unsigned char getAdaptationFieldLength ();
  void setAdaptationFieldLength (unsigned char afl);
  void noFixedSize ();
  unsigned char getDiscontinuityIndicator ();
  void setDiscontinuityIndicator (unsigned char di);
  unsigned char getRandomAccessIndicator ();
  void setRandomAccessIndicator (unsigned char rai);
  unsigned char getElementaryStreamPriorityIndicator ();
  void setElementaryStreamPriorityIndicator (unsigned char espi);
  unsigned char getPcrFlag ();
  void setPcrFlag (unsigned char pf);
  unsigned char getOPcrFlag ();
  void setOPcrFlag (unsigned char opf);
  unsigned char getSplicingPointFlag ();
  void setSplicingPointFlag (unsigned char spf);
  unsigned char getTransportPrivateDataFlag ();
  void setTransportPrivateDataFlag (unsigned char tpdf);
  unsigned char getAdaptationFieldExtensionFlag ();
  void setAdaptationFieldExtensionFlag (unsigned char afef);
  uint64_t getPcrBase ();
  void setPcrBase (uint64_t pb);
  uint64_t getPcrExtension ();
  void setPcrExtension (uint64_t pe);
  uint64_t getOriginalPcrBase ();
  void setOriginalPcrBase (uint64_t opb);
  uint64_t getOriginalPcrExtension ();
  void setOriginalPcrExtension (uint64_t ope);
  char getSpliceCountdown ();
  void setSpliceCountdown (char sc);
  unsigned char getTransportPrivateDataLength ();
  void setTransportPrivateDataLength (unsigned char tpdl);
  unsigned char getAdaptationFieldExtensionLength ();
  void setAdaptationFieldExtensionLength (unsigned char afel);
  unsigned char getLtwFlag ();
  void setLtwFlag (unsigned char lf);
  unsigned char getPiecewiseRateFlag ();
  void setPiecewiseRateFlag (unsigned char prf);
  unsigned char getSeamlessSpliceFlag ();
  void setSeamlessSpliceFlag (unsigned char ssf);
  unsigned char getLtwValidFlag ();
  void setLtwValidFlag (unsigned char lvf);
  unsigned int getLtwOffset ();
  void setLtwOffset (unsigned int lo);
  unsigned int getPiecewiseRate ();
  void setPiecewiseRate (unsigned int pr);
  unsigned char getSpliceType ();
  void setSpliceType (unsigned char st);
  uint64_t getDtsNextAU ();
  void setDtsNextAU (uint64_t dna);
  void setTransportPrivateDataStream (char *stream, unsigned char length);
  int getTransportPrivateDataStream (char **stream);

  int getStream (char stream[MAX_ADAPTATION_FIELD_SIZE]);
};

GINGA_TSPARSER_END

#endif /* TSADAPTATIONFIELD_H_ */
