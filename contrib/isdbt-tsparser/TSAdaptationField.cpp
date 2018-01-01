/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
#include "TSAdaptationField.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

TSAdaptationField::TSAdaptationField ()
{
  init ();
  pcrFlag = 0;
}

TSAdaptationField::TSAdaptationField (uint64_t pcr)
{
  init ();
  this->pcrFlag = 1;
  this->pcrBase = Stc::stcToBase (pcr);
  this->pcrExtension = Stc::stcToExt (pcr);
}

TSAdaptationField::TSAdaptationField (uint64_t pcrBase,
                                      uint64_t pcrExtension)
{
  init ();

  this->pcrFlag = 1;
  this->pcrBase = pcrBase;
  this->pcrExtension = pcrExtension;
}

TSAdaptationField::TSAdaptationField (
    char adapField[MAX_ADAPTATION_FIELD_SIZE])
{
  privateDataStream = NULL;
  streamUpdated = true;
  fixedSize = true;

  memcpy (adapFieldStream, adapField, MAX_ADAPTATION_FIELD_SIZE);
  process ();
}

TSAdaptationField::~TSAdaptationField () { releasePrivateDataStream (); }

void
TSAdaptationField::releasePrivateDataStream ()
{
  if (privateDataStream != NULL)
    {
      delete privateDataStream;
      privateDataStream = NULL;
    }
}

void
TSAdaptationField::init ()
{
  streamUpdated = false;
  fixedSize = false;
  discontinuityIndicator = 0;
  randomAccessIndicator = 0;
  elementaryStreamPriorityIndicator = 0;
  oPCRFlag = 0;
  splicingPointFlag = 0;
  transportPrivateDataFlag = 0;
  adaptationFieldExtensionFlag = 0;
  ltwFlag = 0;
  piecewiseRateFlag = 0;
  seamlessSpliceFlag = 0;
  ltwValidFlag = 0;
  originalPcrBase = 0;
  originalPcrExtension = 0;
  privateDataStream = NULL;
}

void
TSAdaptationField::process ()
{ // decode stream
  int pos = 2;

  adaptationFieldLength = adapFieldStream[0] & 0xFF;

  if (adaptationFieldLength > MAX_ADAPTATION_FIELD_SIZE)
    {
      // adaptationFieldLength == 0 is a valid value!
      clog << "TSAdaptationField::process Warning! Invalid adaptation ";
      clog << "field length: " << (adaptationFieldLength & 0xFF);
      clog << endl;
      return;
    }

  discontinuityIndicator = (adapFieldStream[1] & 0x80) >> 7;
  randomAccessIndicator = (adapFieldStream[1] & 0x40) >> 6;
  elementaryStreamPriorityIndicator = (adapFieldStream[1] & 0x20) >> 5;
  pcrFlag = (adapFieldStream[1] & 0x10) >> 4;
  oPCRFlag = (adapFieldStream[1] & 0x08) >> 3;
  splicingPointFlag = (adapFieldStream[1] & 0x04) >> 2;
  transportPrivateDataFlag = (adapFieldStream[1] & 0x02) >> 1;
  adaptationFieldExtensionFlag = (adapFieldStream[1] & 0x01);

  if (pcrFlag)
    {
      pcrBase = ((adapFieldStream[pos++] & 0xFF) << 23);
      pcrBase = pcrBase << 2;
      pcrBase = pcrBase | ((adapFieldStream[pos++] & 0xFF) << 17);
      pcrBase = pcrBase | ((adapFieldStream[pos++] & 0xFF) << 9);
      pcrBase = pcrBase | ((adapFieldStream[pos++] & 0xFF) << 1);
      pcrBase = pcrBase | ((adapFieldStream[pos] & 0x80) >> 7);

      pcrExtension = ((adapFieldStream[pos++] & 0x01) << 8);
      pcrExtension = pcrExtension | (adapFieldStream[pos++] & 0xFF);
    }
  if (oPCRFlag)
    {
      originalPcrBase = ((adapFieldStream[pos++] & 0xFF) << 23);
      originalPcrBase = originalPcrBase << 2;
      originalPcrBase
          = originalPcrBase | ((adapFieldStream[pos++] & 0xFF) << 17);
      originalPcrBase
          = originalPcrBase | ((adapFieldStream[pos++] & 0xFF) << 9);
      originalPcrBase
          = originalPcrBase | ((adapFieldStream[pos++] & 0xFF) << 1);
      originalPcrBase
          = originalPcrBase | ((adapFieldStream[pos] & 0x80) >> 7);

      originalPcrExtension = ((adapFieldStream[pos++] & 0x01) << 8);
      originalPcrExtension
          = originalPcrExtension | (adapFieldStream[pos++] & 0xFF);
    }

  if (splicingPointFlag)
    {
      spliceCountdown = adapFieldStream[pos++] & 0xFF;
    }
  if (transportPrivateDataFlag)
    {
      transportPrivateDataLength = adapFieldStream[pos++] & 0xFF;
      releasePrivateDataStream ();
      privateDataStream = new char[transportPrivateDataLength];
      memcpy (privateDataStream, adapFieldStream + pos,
              transportPrivateDataLength);
      pos = pos + transportPrivateDataLength;
    }
  if (adaptationFieldExtensionFlag)
    {
      adaptationFieldExtensionLength = adapFieldStream[pos++] & 0xFF;
      ltwFlag = (adapFieldStream[pos] & 0x80) >> 7;
      piecewiseRateFlag = (adapFieldStream[pos] & 0x40) >> 6;
      seamlessSpliceFlag = (adapFieldStream[pos++] & 0x20) >> 5;
      if (ltwFlag)
        {
          ltwValidFlag = (adapFieldStream[pos] & 0x80) >> 7;
          ltwOffset = ((adapFieldStream[pos++] & 0x7F) << 8);
          ltwOffset = ltwOffset | (adapFieldStream[pos++] & 0xFF);
        }
      if (piecewiseRateFlag)
        {
          piecewiseRate = 0;
          piecewiseRate = ((adapFieldStream[pos++] & 0x3F) << 16);
          piecewiseRate
              = piecewiseRate | ((adapFieldStream[pos++] & 0xFF) << 8);
          piecewiseRate = piecewiseRate | (adapFieldStream[pos++] & 0xFF);
        }
      if (seamlessSpliceFlag)
        {
          spliceType = (adapFieldStream[pos] & 0xF0) >> 4;
          dtsNextAU = ((adapFieldStream[pos++] & 0x0E) << 23);
          dtsNextAU = dtsNextAU << 6;
          dtsNextAU = dtsNextAU | ((adapFieldStream[pos++] & 0xFF) << 22);
          dtsNextAU = dtsNextAU | ((adapFieldStream[pos++] & 0xFE) << 14);
          dtsNextAU = dtsNextAU | ((adapFieldStream[pos++] & 0xFF) << 7);
          dtsNextAU = dtsNextAU | ((adapFieldStream[pos] & 0xFE) >> 1);
        }
    }
}

unsigned char
TSAdaptationField::getAdaptationFieldLength ()
{
  if (!fixedSize)
    {
      unsigned int pos = 1;
      if (pcrFlag)
        {
          pos += 6;
        }
      if (oPCRFlag)
        {
          pos += 6;
        }
      if (splicingPointFlag)
        {
          pos += 1;
        }
      if (transportPrivateDataFlag)
        {
          pos += 1;
          if (transportPrivateDataLength > 0)
            {
              pos += transportPrivateDataLength;
            }
        }
      if (adaptationFieldExtensionFlag)
        {
          pos += 2;
          if (ltwFlag)
            {
              pos += 2;
            }
          if (piecewiseRateFlag)
            {
              pos += 3;
            }
          if (seamlessSpliceFlag)
            {
              pos += 5;
            }
        }
      adaptationFieldLength = pos;
    }
  return adaptationFieldLength;
}

void
TSAdaptationField::setAdaptationFieldLength (unsigned char afl)
{
  adaptationFieldLength = afl;
  streamUpdated = false;
  fixedSize = true;
}

void
TSAdaptationField::noFixedSize ()
{
  streamUpdated = false;
  fixedSize = false;
}

unsigned char
TSAdaptationField::getDiscontinuityIndicator ()
{
  return discontinuityIndicator;
}

void
TSAdaptationField::setDiscontinuityIndicator (unsigned char di)
{
  discontinuityIndicator = di;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getRandomAccessIndicator ()
{
  return randomAccessIndicator;
}

void
TSAdaptationField::setRandomAccessIndicator (unsigned char rai)
{
  randomAccessIndicator = rai;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getElementaryStreamPriorityIndicator ()
{
  return elementaryStreamPriorityIndicator;
}

void
TSAdaptationField::setElementaryStreamPriorityIndicator (unsigned char espi)
{
  elementaryStreamPriorityIndicator = espi;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getPcrFlag ()
{
  return pcrFlag;
}

void
TSAdaptationField::setPcrFlag (unsigned char pf)
{
  pcrFlag = pf;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getOPcrFlag ()
{
  return oPCRFlag;
}

void
TSAdaptationField::setOPcrFlag (unsigned char opf)
{
  oPCRFlag = opf;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getSplicingPointFlag ()
{
  return splicingPointFlag;
}

void
TSAdaptationField::setSplicingPointFlag (unsigned char spf)
{
  splicingPointFlag = spf;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getTransportPrivateDataFlag ()
{
  return transportPrivateDataFlag;
}

void
TSAdaptationField::setTransportPrivateDataFlag (unsigned char tpdf)
{
  transportPrivateDataFlag = tpdf;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getAdaptationFieldExtensionFlag ()
{
  return adaptationFieldExtensionFlag;
}

void
TSAdaptationField::setAdaptationFieldExtensionFlag (unsigned char afef)
{
  adaptationFieldExtensionFlag = afef;
  streamUpdated = false;
}

uint64_t
TSAdaptationField::getPcrBase ()
{
  return pcrBase;
}

void
TSAdaptationField::setPcrBase (uint64_t pb)
{
  pcrBase = pb;
  streamUpdated = false;
}

uint64_t
TSAdaptationField::getPcrExtension ()
{
  return pcrExtension;
}

void
TSAdaptationField::setPcrExtension (uint64_t pe)
{
  pcrExtension = pe;
  streamUpdated = false;
}

uint64_t
TSAdaptationField::getOriginalPcrBase ()
{
  return originalPcrBase;
}

void
TSAdaptationField::setOriginalPcrBase (uint64_t opb)
{
  originalPcrBase = opb;
  streamUpdated = false;
}

uint64_t
TSAdaptationField::getOriginalPcrExtension ()
{
  return originalPcrExtension;
}

void
TSAdaptationField::setOriginalPcrExtension (uint64_t ope)
{
  originalPcrExtension = ope;
  streamUpdated = false;
}

char
TSAdaptationField::getSpliceCountdown ()
{
  return spliceCountdown;
}

void
TSAdaptationField::setSpliceCountdown (char sc)
{
  spliceCountdown = sc;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getTransportPrivateDataLength ()
{
  return transportPrivateDataLength;
}

void
TSAdaptationField::setTransportPrivateDataLength (unsigned char tpdl)
{
  transportPrivateDataLength = tpdl;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getAdaptationFieldExtensionLength ()
{
  return adaptationFieldExtensionLength;
}

void
TSAdaptationField::setAdaptationFieldExtensionLength (unsigned char afel)
{
  adaptationFieldExtensionLength = afel;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getLtwFlag ()
{
  return ltwFlag;
}

void
TSAdaptationField::setLtwFlag (unsigned char lf)
{
  ltwFlag = lf;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getPiecewiseRateFlag ()
{
  return piecewiseRateFlag;
}

void
TSAdaptationField::setPiecewiseRateFlag (unsigned char prf)
{
  piecewiseRateFlag = prf;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getSeamlessSpliceFlag ()
{
  return seamlessSpliceFlag;
}

void
TSAdaptationField::setSeamlessSpliceFlag (unsigned char ssf)
{
  seamlessSpliceFlag = ssf;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getLtwValidFlag ()
{
  return ltwValidFlag;
}

void
TSAdaptationField::setLtwValidFlag (unsigned char lvf)
{
  ltwValidFlag = lvf;
  streamUpdated = false;
}

unsigned int
TSAdaptationField::getLtwOffset ()
{
  return ltwOffset;
}

void
TSAdaptationField::setLtwOffset (unsigned int lo)
{
  ltwOffset = lo;
  streamUpdated = false;
}

unsigned int
TSAdaptationField::getPiecewiseRate ()
{
  return piecewiseRate;
}

void
TSAdaptationField::setPiecewiseRate (unsigned int pr)
{
  piecewiseRate = pr;
  streamUpdated = false;
}

unsigned char
TSAdaptationField::getSpliceType ()
{
  return spliceType;
}

void
TSAdaptationField::setSpliceType (unsigned char st)
{
  spliceType = st;
  streamUpdated = false;
}

uint64_t
TSAdaptationField::getDtsNextAU ()
{
  return dtsNextAU;
}

void
TSAdaptationField::setDtsNextAU (uint64_t dna)
{
  dtsNextAU = dna;
  streamUpdated = false;
}

void
TSAdaptationField::setTransportPrivateDataStream (char *stream,
                                                  unsigned char length)
{
  assert (stream != privateDataStream);

  releasePrivateDataStream ();
  privateDataStream = new char[length];
  memcpy (privateDataStream, stream, length);
  transportPrivateDataLength = length;
}

int
TSAdaptationField::getTransportPrivateDataStream (char **stream)
{
  *stream = privateDataStream;
  return transportPrivateDataLength;
}

void
TSAdaptationField::updateStream ()
{ // encode stream
  int pos = 1;
  char value;

  streamUpdated = true;

  adapFieldStream[1] = adapFieldStream[1] & 0x7F;
  adapFieldStream[1]
      = adapFieldStream[1] | ((discontinuityIndicator << 7) & 0x80);
  adapFieldStream[1] = adapFieldStream[1] & 0xBF;
  adapFieldStream[1]
      = adapFieldStream[1] | ((randomAccessIndicator << 6) & 0x40);
  adapFieldStream[1] = adapFieldStream[1] & 0xDF;
  adapFieldStream[1] = adapFieldStream[1]
                       | ((elementaryStreamPriorityIndicator << 5) & 0x20);
  adapFieldStream[1] = adapFieldStream[1] & 0xEF;
  adapFieldStream[1] = adapFieldStream[1] | ((pcrFlag << 4) & 0x10);
  adapFieldStream[1] = adapFieldStream[1] & 0xF7;
  adapFieldStream[1] = adapFieldStream[1] | ((oPCRFlag << 3) & 0x08);
  adapFieldStream[1] = adapFieldStream[1] & 0xFB;
  adapFieldStream[1]
      = adapFieldStream[1] | ((splicingPointFlag << 2) & 0x04);
  adapFieldStream[1] = adapFieldStream[1] & 0xFD;
  adapFieldStream[1]
      = adapFieldStream[1] | ((transportPrivateDataFlag << 1) & 0x02);
  adapFieldStream[1] = adapFieldStream[1] & 0xFE;
  adapFieldStream[1]
      = adapFieldStream[1] | ((adaptationFieldExtensionFlag)&0x01);
  if (pcrFlag)
    {
      adapFieldStream[++pos] = ((pcrBase >> 25) & 0xFF);
      adapFieldStream[++pos] = ((pcrBase >> 17) & 0xFF);
      adapFieldStream[++pos] = ((pcrBase >> 9) & 0xFF);
      adapFieldStream[++pos] = ((pcrBase >> 1) & 0xFF);
      adapFieldStream[++pos] = ((pcrBase << 7) & 0x80);
      adapFieldStream[pos] = adapFieldStream[pos] | 0x7E;
      value = ((pcrExtension >> 8) & 0x01);
      adapFieldStream[pos] = adapFieldStream[pos] | value;
      adapFieldStream[++pos] = pcrExtension & 0xFF;
    }
  if (oPCRFlag)
    {
      adapFieldStream[++pos] = ((originalPcrBase >> 25) & 0xFF);
      adapFieldStream[++pos] = ((originalPcrBase >> 17) & 0xFF);
      adapFieldStream[++pos] = ((originalPcrBase >> 9) & 0xFF);
      adapFieldStream[++pos] = ((originalPcrBase >> 1) & 0xFF);
      adapFieldStream[++pos] = ((originalPcrBase << 7) & 0x80);
      adapFieldStream[pos] = adapFieldStream[pos] | 0x7E;
      value = ((originalPcrExtension >> 8) & 0x01);
      adapFieldStream[pos] = adapFieldStream[pos] | value;
      adapFieldStream[++pos] = originalPcrExtension & 0xFF;
    }
  if (splicingPointFlag)
    {
      adapFieldStream[++pos] = spliceCountdown & 0xFF;
    }
  if (transportPrivateDataFlag)
    {
      adapFieldStream[++pos] = transportPrivateDataLength & 0xFF;
      if ((transportPrivateDataLength > 0) && (privateDataStream))
        {
          pos++;
          memcpy (adapFieldStream + pos, privateDataStream,
                  transportPrivateDataLength);
        }
      pos = pos + transportPrivateDataLength - 1;
    }

  if (adaptationFieldExtensionFlag)
    {
      adaptationFieldExtensionLength = 1;
      if (ltwFlag)
        adaptationFieldExtensionLength += 2;
      if (piecewiseRateFlag)
        adaptationFieldExtensionLength += 3;
      if (seamlessSpliceFlag)
        adaptationFieldExtensionLength += 5;
      adapFieldStream[++pos] = adaptationFieldExtensionLength & 0xFF;
      adapFieldStream[++pos] = (ltwFlag << 7) & 0x80;
      adapFieldStream[pos]
          = adapFieldStream[pos] | ((piecewiseRateFlag << 6) & 0x40);
      adapFieldStream[pos]
          = adapFieldStream[pos] | ((seamlessSpliceFlag << 5) & 0x20);
      adapFieldStream[pos] = adapFieldStream[pos] | 0x1F;
      if (ltwFlag)
        {
          adapFieldStream[++pos] = (ltwValidFlag << 7) & 0x80;
          adapFieldStream[pos]
              = adapFieldStream[pos] | ((ltwOffset >> 8) & 0x7F);
          adapFieldStream[++pos] = ltwOffset & 0xFF;
        }
      if (piecewiseRateFlag)
        {
          adapFieldStream[++pos] = 0xC0;
          adapFieldStream[pos]
              = adapFieldStream[pos] | ((piecewiseRate >> 16) & 0x3F);
          adapFieldStream[++pos] = (piecewiseRate >> 8) & 0xFF;
          adapFieldStream[++pos] = piecewiseRate & 0xFF;
        }
      if (seamlessSpliceFlag)
        {
          adapFieldStream[++pos] = (spliceType << 4) & 0xF0;
          adapFieldStream[pos]
              = adapFieldStream[pos] | ((dtsNextAU >> 29) & 0x0E);
          adapFieldStream[pos] = adapFieldStream[pos] | 0x01;
          adapFieldStream[++pos] = (dtsNextAU >> 22) & 0xFF;
          adapFieldStream[++pos] = (dtsNextAU >> 14) & 0xFE;
          adapFieldStream[pos] = adapFieldStream[pos] | 0x01;
          adapFieldStream[++pos] = (dtsNextAU >> 7) & 0xFF;
          adapFieldStream[++pos] = (dtsNextAU << 1) & 0xFE;
          adapFieldStream[pos] = adapFieldStream[pos] | 0x01;
        }
    }
  if (fixedSize)
    {
      if (adaptationFieldLength > pos)
        {
          pos++;
          for (int i = pos; i <= adaptationFieldLength; i++)
            {
              adapFieldStream[pos] = 0xFF;
              pos++;
            }
        }
    }
  else
    {
      adaptationFieldLength = pos;
    }
  adapFieldStream[0] = adaptationFieldLength;
}

int
TSAdaptationField::getStream (char stream[MAX_ADAPTATION_FIELD_SIZE])
{
  if (!streamUpdated)
    {
      updateStream ();
    }
  memcpy (stream, adapFieldStream, MAX_ADAPTATION_FIELD_SIZE);
  return adaptationFieldLength + 1;
}

GINGA_TSPARSER_END
