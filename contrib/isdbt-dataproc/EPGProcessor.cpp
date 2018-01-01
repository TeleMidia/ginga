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
#include "EPGProcessor.h"

#include "util/functions.h"
using namespace ::ginga::util;

#include "isdbt-tsparser/ServiceInfo.h"
#include "isdbt-tsparser/EventInfo.h"
#include "isdbt-tsparser/TOT.h"
#include "isdbt-tsparser/ShortEventDescriptor.h"
#include "isdbt-tsparser/LogoTransmissionDescriptor.h"

#include "isdbt-tsparser/IServiceInfo.h"
#include "isdbt-tsparser/IEventInfo.h"
using namespace ::ginga::tsparser;

#include "isdbt-tsparser/IMpegDescriptor.h"
using namespace ::ginga::tsparser;

#include "isdbt-tsparser/IShortEventDescriptor.h"
#include "isdbt-tsparser/IExtendedEventDescriptor.h"
#include "isdbt-tsparser/IServiceDescriptor.h"
using namespace ::ginga::tsparser;

#include "IEPGListener.h"

#include "DataProcessor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

EPGProcessor::EPGProcessor ()
{
  files = 0;
  cdt = new set<string>;
  eventPresent = new map<unsigned int, IEventInfo *>;
  eventSchedule = new map<unsigned int, IEventInfo *>;
  // services           = new map<unsigned int, IServiceInfo*>;
  // epgListeners       = new map<string, set<IEPGListener*>*>;
  epgListeners = NULL;
  serviceListeners = NULL;
  dataProcessor = NULL;
  processedSections = new set<string>;
  presentMapReady = false;
  scheduleMapReady = false;
}

EPGProcessor::~EPGProcessor ()
{
  /*TODO:
   * delete listeners vector/set/map, but do NOT delete listeners!
   */

  map<unsigned int, IEventInfo *>::iterator i;

  for (i = eventPresent->begin (); i != eventPresent->end (); ++i)
    {
      delete i->second;
    }

  delete eventPresent;
  eventPresent = NULL;

  for (i = eventSchedule->begin (); i != eventSchedule->end (); ++i)
    {
      delete i->second;
    }

  delete eventSchedule;
  eventSchedule = NULL;

  if (epgListeners != NULL)
    {
      delete epgListeners;
      epgListeners = NULL;
    }

  delete service;
  service = NULL;

  if (serviceListeners != NULL)
    {
      delete serviceListeners;
      serviceListeners = NULL;
    }

  delete processedSections;
  processedSections = NULL;

  _instance = NULL;
}

EPGProcessor *EPGProcessor::_instance = NULL;

EPGProcessor *
EPGProcessor::getInstance ()
{
  if (EPGProcessor::_instance == NULL)
    {
      EPGProcessor::_instance = new EPGProcessor ();
    }
  return EPGProcessor::_instance;
}

void
EPGProcessor::release ()
{
  if (EPGProcessor::_instance != NULL)
    {
      delete EPGProcessor::_instance;
      _instance = NULL;
    }
}

void
EPGProcessor::setDataProcessor (void *dataProcessor)
{
  this->dataProcessor = dataProcessor;
}

/*
 * The EPG events could only be resquested by Lua node. When a Lua node
 * request
 * epg events, the Lua Player register himself as epgListener associated
 * with
 * request (more than one Lua could make the same request, making 1 request
 * to
 * n
 * epgListener association).
 * The standard defines three general request types:
 * 1 - current event (with desired metadata fiedls)
 * 2 - next event to a specified event (with desired metadata fields)
 * 3 - schedule (with desired time range and desired metadata fields)
 *
 * The EPGProcessor only process EIT table if there is at least one
 * epgListener.
 *
 */

void
EPGProcessor::addEPGListener (IEPGListener *listener, unused (const string &request),
                              unsigned char type)
{
  // TODO: handle request properly.
  clog << "EPGProcessor::addEPGListener with type:" << type << endl;

  if (type == IEPGListener::EPG_LISTENER)
    {
      if (epgListeners == NULL)
        {
          epgListeners = new set<IEPGListener *>;
        }

      if (epgListeners->empty ())
        {
          if (dataProcessor != NULL)
            {
              DataProcessor *dp = (DataProcessor *)dataProcessor;
              dp->createPidSectionFilter (SDT_PID); // SDT
              dp->createPidSectionFilter (EIT_PID); // EIT
              dp->createPidSectionFilter (CDT_PID); // CDT
            }
        }

      epgListeners->insert (listener);
    }
  else if (type == IEPGListener::SI_LISTENER)
    {
      if (serviceListeners == NULL)
        {
          serviceListeners = new set<IEPGListener *>;
        }

      serviceListeners->insert (listener);
    }
  else if (type == IEPGListener::TIME_LISTENER)
    {
      if (timeListeners == NULL)
        {
          timeListeners = new set<IEPGListener *>;
        }

      timeListeners->insert (listener);
    }
}

void
EPGProcessor::removeEPGListener (IEPGListener *listener)
{
  set<IEPGListener *>::iterator i;

  i = epgListeners->find (listener);
  if (i != epgListeners->end ())
    {
      epgListeners->erase (i);
    }

  if (epgListeners->empty ())
    {
      // TODO: removePidSectionFilter
    }

  // TODO: remove SDT e TOT listeners
}

void
EPGProcessor::decodeSdtSection (ITransportSection *section)
{
  IServiceInfo *srvi;
  unsigned int payloadSize, sectionNumber;
  char *data;
  size_t pos;
  string newSectionName;

  clog << "EPGProcessor::decodeSdtSection decoding SDT section";
  payloadSize = section->getPayloadSize ();
  clog << " with payloadSize = " << payloadSize << endl;

  if (checkProcessedSections (section))
    {
      return;
    }

  sectionNumber = section->getSectionNumber ();
  newSectionName = section->getSectionName () + xstrbuild ("%u", sectionNumber);

  data = new char[payloadSize];
  memcpy ((void *)&(data[0]), section->getPayload (), payloadSize);

  pos = 0;
  pos += 3;
  while (pos < payloadSize)
    {
      // there's at least one serviceinfo
      srvi = new ServiceInfo ();
      pos = srvi->process (data, pos);
      srvi->print ();

      //(*services)[sectionNumber] = srvi;
      service = srvi;
      addProcessedSection (section);
    }
  // clog << "EPGProcessor::decodedSdtSection section decoded" << endl;
}

void
EPGProcessor::callMapGenerator (unsigned tableId)
{
  if (tableId == 0x4E)
    {
      if (!presentMapReady)
        {
          clog << "EPGProcessor::callMapGenerator presentMap";
          cout << " ready to move" << endl;
          presentMapReady = true;
          if (epgListeners != NULL)
            {
              generateEitMap (eventPresent);
            }
        }
    }
  else if (tableId >= 0x50 && tableId <= 0x5F)
    {
      if (!scheduleMapReady)
        {
          clog << "EPGProcessor::callMapGenerator scheduleMap";
          cout << " ready to move" << endl;
          scheduleMapReady = true;
          if (epgListeners != NULL)
            {
              // TODO: handle schedule TableIDs properly.
              // generateEitMap(eventSchedule);
            }
        }
    }
  else if (tableId == 0x42)
    {
      // everytime a new service info section is received the map has
      // to be generate to send to the listeners.
      if (!serviceMapReady)
        {
          serviceMapReady = true;
          generateSdtMap (service);
          clog << "EPGProcessor::callMapGenerator serviceMap ready to";
          clog << "move" << endl;
        }
    }
  else if (tableId == 0x73)
    {
      timeMapReady = true;
      // generate map
    }
}

/* If a section is the last section means that in that moment all sections
 * were received so the map could be sent to the listeners. The problem is
 * if the TS is malformed the last section could never arrives. For this
 * reason, another verification if the map is ready to send to the listeners
 * is done by checkProcessedSections.
 */
void
EPGProcessor::addProcessedSection (ITransportSection *section)
{
  unsigned int sectionNumber;
  string newName;

  /*TODO: handle section syntax: this is valid just for section with
   * syntax == 1. section with syntax == 0 does not have number, version.
  */
  sectionNumber = section->getSectionNumber ();
  newName = section->getSectionName () + xstrbuild ("%u", sectionNumber);
  processedSections->insert (newName);

  if (sectionNumber == section->getLastSectionNumber ())
    {
      callMapGenerator (section->getTableId ());
    }
}

/*
 * When a section is received twice means the all events of the same type
 * were received so the map could be sent to the listeners. This is done
 * by checkProcessedSections. When a non-processed section arrives any
 * moment after a processed section, means that new events will arriving,
 * so the map should be clean it up (this is done by the decoding sections
 * methods). The events will be collected until the first processed section
 * arrives, re-starting the cycle.
 *
 * The function checkProcessedSection also discards section with payload
 * <=6 and sections with CurrentNextIndicator == 0.
 */
bool
EPGProcessor::checkProcessedSections (ITransportSection *section)
{
  unsigned int tableId, sectionNumber, lastSectionNumber;
  string newSectionName;

  if (section->getPayloadSize () <= 6)
    {
      return true;
    }

  sectionNumber = section->getSectionNumber ();
  lastSectionNumber = section->getLastSectionNumber ();
  newSectionName = section->getSectionName () + xstrbuild ("%u", sectionNumber);

  if (processedSections->count (newSectionName) > 0)
    {
      tableId = section->getTableId ();
      clog << "EPGProcessor::checkSection section exists:!";
      clog << endl;

      clog << "  -TableId: " << hex << tableId << dec;
      clog << "    -SectionVersion: " << section->getVersionNumber ();
      clog << "    -SectionNumber: " << sectionNumber;
      clog << "	 -LastSectionNumber: " << lastSectionNumber;
      clog << "    -CurrentNextId: ";
      clog << section->getCurrentNextIndicator ();
      clog << "    -SectionName:" << newSectionName << endl;

      callMapGenerator (tableId);
      return true;
    }
  else
    {
      return false;
    }
}

void
EPGProcessor::decodeEitSection (ITransportSection *section)
{
  unsigned int payloadSize;
  unsigned int tableId, sectionNumber;
  string sectionName, newSectionName;
  IEventInfo *ei;
  char *data;
  size_t pos;
  map<unsigned int, IEventInfo *>::iterator i;

  tableId = section->getTableId ();
  payloadSize = section->getPayloadSize ();

  if (checkProcessedSections (section))
    {
      return;
    }
  clog << endl;
  clog << "EPGProcessor::decodeEitSection with tableId = " << hex;
  clog << tableId << dec << " and payloadSize = ";
  clog << payloadSize << endl;

  sectionNumber = section->getSectionNumber ();
  sectionName = section->getSectionName ();
  newSectionName = sectionName + xstrbuild ("%u", sectionNumber);

  data = new char[payloadSize];
  memcpy ((void *)&(data[0]), section->getPayload (), payloadSize);

  pos = 0;
  pos += 2;
  pos += 2;
  pos++;
  pos++; // pos = 6;
  while (pos < payloadSize)
    {
      ei = new EventInfo ();
      pos = ei->process (data, pos);

      if (tableId == 0x4E)
        {
          if (presentMapReady)
            {
              clog << "EPGProcessor::decodeEitSection cleaning pres map";
              clog << endl;
              eventPresent->clear ();
              presentMapReady = false;
              /* receiving the first non-processed section after processed
               *  sections (receiving new events, discarding the old ones)
               */
            }
          (*eventPresent)[sectionNumber] = ei;
        }
      else if (tableId >= 0x50 && tableId <= 0x5F)
        {
          if (scheduleMapReady)
            {
              clog << "EPGProcessor::decodeEitSection cleaning sched map";
              clog << endl;
              eventSchedule->clear ();
              scheduleMapReady = false;
              /* receiving the first non-processed section after processed
               * sections (receiving new events, discarding the old ones)
               */
            }
          (*eventSchedule)[sectionNumber] = ei;
        }
      addProcessedSection (section);
    }
}

void
EPGProcessor::generateSdtMap (IServiceInfo *si)
{
  struct SIField field, fieldMap;
  map<string, struct SIField> responseMap, data;
  vector<IMpegDescriptor *>::iterator i;
  vector<IMpegDescriptor *> *descs;
  IServiceDescriptor *sd;
  set<IEPGListener *>::iterator j;

  if (si == NULL)
    {
      return;
    }

  field.str = xstrbuild ("%d", si->getServiceId ());
  (responseMap)["id"] = field;

  field.str = xstrbuild ("%d", si->getRunningStatus ());
  (responseMap)["runningStatus"] = field;

  descs = si->getDescriptors ();
  if (descs == NULL)
    {
      fieldMap.table = responseMap;
      string name = "0";
      (data)["0"] = fieldMap;
      // TODO: send and test! All SDT section has at least one descriptors.
      return;
    }

  for (i = descs->begin (); i != descs->end (); ++i)
    {
      switch ((*i)->getDescriptorTag ())
        {
        case IServiceInfo::DT_SERVICE:
          sd = (IServiceDescriptor *)(*i);

          field.str = sd->getServiceProviderNameChar ();
          (responseMap)["providerName"] = field;

          field.str = sd->getServiceNameChar ();
          (responseMap)["serviceName"] = field;
          break;

        default:
          clog << "EPGProcessor::generateSdtMap Unknown type" << endl;
          break;
        }
    }

  fieldMap.table = responseMap;
  string name = "0";
  (data)["0"] = fieldMap;

  // printFieldMap(&data);
  if (serviceListeners != NULL && !serviceListeners->empty ())
    {
      for (j = serviceListeners->begin (); j != serviceListeners->end ();
           ++j)
        {
          (*j)->pushSIEvent (data, IEPGListener::SI_LISTENER);
        }
    }
  else
    {
      clog << "EPGProcessor::generateSdtMap there is no service listener";
      clog << " to notify" << endl;
    }
}

void
EPGProcessor::generateTotMap (ITOT *tot)
{
  map<string, struct SIField> responseMap, data;
  set<IEPGListener *>::iterator i;
  struct SIField field, fieldMap;
  struct tm time;

  if (tot == NULL)
    {
      return;
    }

  time = tot->getUTC3TimeTm ();
  field.str = xstrbuild ("%d", time.tm_year);
  (responseMap)["year"] = field;

  field.str = xstrbuild ("%d", time.tm_mon);
  (responseMap)["month"] = field;

  field.str = xstrbuild ("%d", time.tm_mday);
  (responseMap)["day"] = field;

  field.str = xstrbuild ("%d", time.tm_hour);
  (responseMap)["hours"] = field;

  field.str = xstrbuild ("%d", time.tm_min);
  (responseMap)["minutes"] = field;

  field.str = xstrbuild ("%d", time.tm_sec);
  (responseMap)["seconds"] = field;

  if (timeListeners != NULL && !timeListeners->empty ())
    {
      for (i = timeListeners->begin (); i != timeListeners->end (); ++i)
        {
          (*i)->pushSIEvent (responseMap, IEPGListener::TIME_LISTENER);
        }
    }
}

void
EPGProcessor::generateEitMap (map<unsigned int, IEventInfo *> *actualMap)
{
  map<string, struct SIField> responseMap, data;
  IEventInfo *ei;
  map<unsigned int, IEventInfo *>::iterator i;
  vector<IMpegDescriptor *>::iterator j;
  struct SIField field, fieldMap;
  IShortEventDescriptor *sed;
  vector<IMpegDescriptor *> *descs;
  string name;
  set<IEPGListener *>::iterator k;

  //
  // clog << "EPGProcessor::generateMap beginning" << endl;
  for (i = actualMap->begin (); i != actualMap->end (); ++i)
    {
      ei = i->second;
      if (ei != NULL)
        {
          field.str = xstrbuild ("%d", ei->getEventId ());
          (responseMap)["id"] = field;

          field.str = ei->getStartTimeSecsStr ();
          (responseMap)["startTime"] = field;

          field.str = ei->getEndTimeSecsStr ();
          (responseMap)["endTime"] = field;
          descs = ei->getDescriptors ();

          /*clog << "EPGProcessor::generateMap printing:" << endl;
          clog << "  -id = " << (responseMap)["id"].str;
          clog << "  -startTime = " << (responseMap)["startTime"].str;
          clog << "  -endTime = " << (responseMap)["endTime"].str;*/

          if (descs != NULL)
            {
              for (j = descs->begin (); j != descs->end (); ++j)
                {
                  switch ((*j)->getDescriptorTag ())
                    {
                    case IEventInfo::DT_SHORT_EVENT:
                      sed = (IShortEventDescriptor *)(*j);

                      field.str = sed->getEventName ();
                      (responseMap)["name"] = field;
                      // clog << "  -name = ";
                      // clog << (responseMap)["name"].str;

                      field.str = sed->getTextChar ();
                      (responseMap)["shortDescription"] = field;
                      // clog << "  -shortDescription = ";
                      // clog << (responseMap)["shortDescription"].str;
                      break;

                    case IEventInfo::DT_PARENTAL_RATING:
                      break;

                    default:
                      break;
                    }
                }
            }
          // clog << endl;
          name = "evt" + xstrbuild ("%d", ei->getEventId ());
          fieldMap.table = responseMap;
          (data)[name] = fieldMap;
          // clog << "(evt belongs to data[" << name << "])" << endl;
        }
    }

  // printFieldMap(&data);
  if (epgListeners != NULL && !epgListeners->empty ())
    {
      for (k = epgListeners->begin (); k != epgListeners->end (); ++k)
        {
          (*k)->pushSIEvent (data, IEPGListener::EPG_LISTENER);
        }
    }
  else
    {
      clog << "EPGProcessor::generateEitMap there is no epg listener to "
              "notify";
      clog << endl;
    }
}

void
EPGProcessor::printFieldMap (map<string, struct SIField> *fieldMap)
{
  map<string, struct SIField>::iterator i;

  clog << "EPGProcesor::printFieldMap printing..." << endl;
  for (i = fieldMap->begin (); i != fieldMap->end (); ++i)
    {
      clog << i->first << " = ";
      if (i->second.str.empty ())
        {
          if (i->second.table.empty ())
            {
              clog << "all empty" << endl;
            }
          else
            {
              clog << "map: { " << endl;
              printFieldMap (&(i->second.table));
              clog << "}" << endl;
            }
        }
      else
        {
          clog << i->second.str << endl;
        }
    }
}

struct SIField *
EPGProcessor::handleFieldStr (const string &str)
{
  struct SIField *field;
  // clog << "EPGProcessor::handleFieldstr with str = " << str << endl;

  field = new struct SIField;
  if (str == "")
    {
      field->str = "0";
      return field;
    }

  field->str = str;
  return field;
}

void
EPGProcessor::decodeTot (ITransportSection *section)
{
  unsigned int payloadSize;
  char *data;
  payloadSize = section->getPayloadSize ();

  clog << "EPGProcessor::decodeTOT";
  clog << "withPayloadSize: " << payloadSize << endl;

  data = new char[payloadSize];
  memcpy ((void *)&(data[0]), section->getPayload (), payloadSize);

  tot = new TOT ();
  tot->process (data, payloadSize);
  // tot->print();
}

void
EPGProcessor::decodeCdt (const string &fileName)
{
  char data[4084];
  FILE *fd;
  int rval, pngSize, totalSize, times, remainder, pos;

  clog << "Decoding CDT stream..." << endl << endl;

  fd = fopen (fileName.c_str (), "rb");
  pngSize = 0;
  pos = 0;

  fseek (fd, 0L, SEEK_END);
  totalSize = ftell (fd);
  fseek (fd, 0L, SEEK_SET);

  remainder = totalSize % 4084;
  times = ((totalSize - remainder) / 4084);

  pngSize = (times * 4079) + (remainder - 5);

  char *pngData = new char[pngSize];

  clog << "CDT reading first blocks. pngSize is '" << pngSize << "'";
  clog << endl;
  for (int i = 0; i < times; i++)
    {
      rval = fread ((void *)&(data[0]), 1, 4084, fd);
      if (rval == 4084)
        {
          // TODO: check data_type!!!

          // descriptorsLoopLength = ((((data[3] & 0x0F) << 8) & 0xFF00) |
          //	(data[4] & 0xFF));

          memcpy (pngData + pos, data + 5, 4079);
          pos += 4079;
        }
      else
        {
          clog << "CDT Warning! Can't read 4079 block of bytes.";
          clog << " pos = '" << pos << "'" << endl;

          delete[] pngData;
          return;
        }
    }

  clog << "CDT reading remainder." << endl;
  rval = fread ((void *)&(data[0]), 1, remainder, fd);
  if (rval == remainder)
    {
      memcpy (pngData + pos, data + 5, remainder);
    }
  else
    {
      clog << "CDT Warning! Can't read remaining of '" << remainder;
      clog << "' bytes. pos = '" << pos << "'" << endl;

      delete[] pngData;
      return;
    }

  fclose (fd);

  this->savePNG (pngData, pngSize);
  clog << "Stream decoded successfully. PngSize is '" << pngSize;
  clog << "', pos is '" << pos + remainder << "'" << endl;

  delete[] pngData;
}

int
EPGProcessor::savePNG (char *pngData, int pngSize)
{
  FILE *png;
  int wc;
  string path;

  xstrassign (path, "%d.png", files);
  files++;
  if (pngData == NULL)
    {
      clog << "No data to save." << endl;
      return 1;
    }

  clog << "Creating png file." << endl;
  png = fopen (path.c_str (), "wb");
  if (png == NULL)
    {
      clog << "cannot open PNG file. (" << path << ")" << endl;
      return 2;
    }

  clog << "Writing data to png file." << endl;
  wc = (int) fwrite (pngData, 1, pngSize, png);
  if (wc != pngSize)
    {
      clog << "Writing error." << endl;
      return 3;
    }

  clog << "Closing png file." << endl;
  cdt->insert (path);

  fclose (png);
  return 0;
}

GINGA_DATAPROC_END
