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
#include "DsmccNPTProcessor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

DsmccNPTProcessor::DsmccNPTProcessor (ISTCProvider *stcProvider) : Thread ()
{
  this->stcProvider = stcProvider;
  this->isFirstStc = true;
  this->running = true;
  this->currentCid = INVALID_CID;
  occurringTimeBaseId = INVALID_CID;
  this->loopControlMax = false;
  this->loopControlMin = false;
  this->reScheduleIt = false;
  this->nptPrinter = false;

  Thread::mutexInit (&loopMutex, false);
  Thread::mutexInit (&schedMutex, false);
  Thread::mutexInit (&lifeMutex, false);

  startThread ();
}

DsmccNPTProcessor::~DsmccNPTProcessor ()
{
  map<unsigned char,
      map<TimeControl *, set<ITimeBaseProvider *> *> *>::iterator i;
  map<TimeControl *, set<ITimeBaseProvider *> *>::iterator j;
  set<ITimeBaseProvider *>::iterator k;
  map<unsigned char, set<ITimeBaseProvider *> *>::iterator l;

  clearTables ();

  lock ();

  i = timeListeners.begin ();
  while (i != timeListeners.end ())
    {
      j = i->second->begin ();
      while (j != i->second->end ())
        {
          assert (j->first != NULL);
          assert (j->second != NULL);

          delete j->first;
          delete j->second;

          ++j;
        }
      delete i->second;
      ++i;
    }
  timeListeners.clear ();

  unlock ();

  cidListeners.clear ();

  Thread::mutexLock (&loopMutex);
  loopListeners.clear ();
  Thread::mutexUnlock (&loopMutex);
  Thread::mutexDestroy (&loopMutex);

  Thread::mutexLock (&schedMutex);
  Thread::mutexUnlock (&schedMutex);
  Thread::mutexDestroy (&schedMutex);

  Thread::mutexLock (&lifeMutex);
  Thread::mutexUnlock (&lifeMutex);
  Thread::mutexDestroy (&lifeMutex);
}

void
DsmccNPTProcessor::setNptPrinter (bool nptPrinter)
{
  this->nptPrinter = nptPrinter;
}

void
DsmccNPTProcessor::clearTables ()
{
  map<unsigned char, DsmccNPTReference *>::iterator i;
  map<unsigned char, DsmccTimeBaseClock *>::iterator j;
  map<unsigned char, Stc *>::iterator k;

  lock ();

  map<unsigned char,
      map<TimeControl *, set<ITimeBaseProvider *> *> *>::iterator a;
  map<TimeControl *, set<ITimeBaseProvider *> *>::iterator b;
  a = timeListeners.begin ();
  while (a != timeListeners.end ())
    {
      b = a->second->begin ();
      while (b != a->second->end ())
        {
          assert (b->first != NULL);
          assert (b->second != NULL);

          delete b->first;
          delete b->second;

          ++b;
        }
      delete a->second;
      ++a;
    }
  timeListeners.clear ();

  unlock ();

  Thread::mutexLock (&schedMutex);
  i = scheduledNpts.begin ();
  while (i != scheduledNpts.end ())
    {
      delete i->second;
      ++i;
    }
  scheduledNpts.clear ();
  Thread::mutexUnlock (&schedMutex);

  j = timeBaseClock.begin ();
  while (j != timeBaseClock.end ())
    {
      delete j->second;
      ++j;
    }
  timeBaseClock.clear ();

  Thread::mutexLock (&lifeMutex);
  k = timeBaseLife.begin ();
  while (k != timeBaseLife.end ())
    {
      delete k->second;
      ++k;
    }
  timeBaseLife.clear ();
  Thread::mutexUnlock (&lifeMutex);
}

void
DsmccNPTProcessor::clearUnusedTimebase ()
{
  map<unsigned char, Stc *>::iterator itLife;
  map<unsigned char, DsmccTimeBaseClock *>::iterator i;
  DsmccTimeBaseClock *clk;
  bool restart = true;

  i = timeBaseClock.begin ();
  while (i != timeBaseClock.end ())
    {
      clk = i->second;
      if (clk)
        {
          if ((clk->getEndpointAvailable ())
              && (clk->getStcBase () >= clk->getStopNpt ()))
            {
              notifyNaturalEndListeners (
                  clk->getContentId (),
                  Stc::baseToSecond (clk->getStopNpt ()));

              clog
                  << "DsmccNPTProcessor::clearUnusedTimebase - Deleted by ";
              clog << "endpoint: CID = " << (clk->getContentId () & 0xFF);
              clog << endl;

              delete i->second;
              timeBaseClock.erase (i);
              i = timeBaseClock.begin ();
              continue;
            }
        }
      ++i;
    }

  // TODO: Timebase should still be incremented after 1 second without no
  //      NPT Reference updates?
  while (restart)
    {
      restart = false;
      Thread::mutexLock (&lifeMutex);
      itLife = timeBaseLife.begin ();
      if (itLife != timeBaseLife.end ())
        {
          if (itLife->second->getStcBase () > 5400000)
            { // 1 minute
              clk = getTimeBaseClock (itLife->first);
              if (clk)
                {
                  notifyNaturalEndListeners (clk->getContentId (),
                                             clk->getBaseToSecond ());

                  clog
                      << "DsmccNPTProcessor::clearUnusedTimebase - Deleted "
                         "by ";
                  clog << "lifetime: CID = "
                       << (clk->getContentId () & 0xFF);
                  clog << endl;
                }
              delete itLife->second;
              timeBaseLife.erase (itLife);
              restart = true;
            }
          ++itLife;
        }
      Thread::mutexUnlock (&lifeMutex);
    }
}

uint64_t
DsmccNPTProcessor::getSTCValue ()
{
  guint64 stcValue;
  int valueType;

  stcProvider->getSTCValue (&stcValue, &valueType);
  if (valueType == ST_27MHz)
    {
      stcValue = Stc::stcToBase (stcValue);
    }

  return stcValue;
}

bool
DsmccNPTProcessor::addLoopListener (unsigned char cid,
                                    ITimeBaseListener *ltn)
{
  map<unsigned char, set<ITimeBaseProvider *> *>::iterator i;
  set<ITimeBaseProvider *> *listeners;

  clog << "DsmccNPTProcessor::addLoopListener" << endl;

  Thread::mutexLock (&loopMutex);
  i = loopListeners.find (cid);
  if (i != loopListeners.end ())
    {
      listeners = i->second;
    }
  else
    {
      listeners = new set<ITimeBaseProvider *>;
      loopListeners[cid] = listeners;
    }

  listeners->insert ((ITimeBaseProvider *)ltn);
  Thread::mutexUnlock (&loopMutex);

  return true;
}

bool
DsmccNPTProcessor::removeLoopListener (unsigned char cid,
                                       ITimeBaseListener *ltn)
{
  map<unsigned char, set<ITimeBaseProvider *> *>::iterator i;
  set<ITimeBaseProvider *>::iterator j;

  clog << "DsmccNPTProcessor::removeLoopListener() - cid = " << (cid & 0xFF)
       << endl;

  Thread::mutexLock (&loopMutex);

  i = loopListeners.find (cid);
  if (i != loopListeners.end ())
    {
      j = i->second->find ((ITimeBaseProvider *)ltn);
      if (j == i->second->end ())
        {
          Thread::mutexUnlock (&loopMutex);
          return false;
        }
      else
        {
          i->second->erase (j);
        }
    }

  Thread::mutexUnlock (&loopMutex);

  unlockConditionSatisfied ();

  return true;
}

bool
DsmccNPTProcessor::addTimeListener (unsigned char cid, double nptValue,
                                    ITimeBaseListener *ltn)
{
  map<unsigned char,
      map<TimeControl *, set<ITimeBaseProvider *> *> *>::iterator i;
  map<TimeControl *, set<ITimeBaseProvider *> *>::iterator j;
  set<ITimeBaseProvider *>::iterator k;

  map<TimeControl *, set<ITimeBaseProvider *> *> *valueListeners;
  set<ITimeBaseProvider *> *setListeners;
  bool added;
  TimeControl *tc;

  clog << "DsmccNPTProcessor::addTimeListener TIME = " << nptValue << endl;

  lock ();
  i = timeListeners.find (cid);
  if (i == timeListeners.end ())
    {
      valueListeners = new map<TimeControl *, set<ITimeBaseProvider *> *>;
      timeListeners[cid] = valueListeners;
    }
  else
    {
      valueListeners = i->second;
    }

  j = valueListeners->begin ();
  while (j != valueListeners->end ())
    {
      if (xnumeq (j->first->time, nptValue))
        {
          break;
        }
      ++j;
    }

  if (j == valueListeners->end ())
    {
      setListeners = new set<ITimeBaseProvider *>;
      tc = new TimeControl ();
      tc->time = nptValue;
      tc->notified = false;
      (*valueListeners)[tc] = setListeners;
    }
  else
    {
      setListeners = j->second;
    }

  k = setListeners->find ((ITimeBaseProvider *)ltn);
  added = (k == setListeners->end ());
  setListeners->insert ((ITimeBaseProvider *)ltn);
  unlock ();

  unlockConditionSatisfied ();
  return added;
}

bool
DsmccNPTProcessor::removeTimeListener (unsigned char cid,
                                       ITimeBaseListener *ltn)
{
  map<unsigned char,
      map<TimeControl *, set<ITimeBaseProvider *> *> *>::iterator i;
  map<TimeControl *, set<ITimeBaseProvider *> *>::iterator j;
  set<ITimeBaseProvider *>::iterator k;

  clog << "DsmccNPTProcessor::removeTimeListener()" << endl;

  lock ();
  i = timeListeners.find (cid);
  if (i != timeListeners.end ())
    {
      j = i->second->begin ();
      while (j != i->second->end ())
        {
          k = j->second->find ((ITimeBaseProvider *)ltn);
          if (k != j->second->end ())
            {
              j->second->erase (k);
              if (j->second->empty ())
                {
                  delete j->second;
                  i->second->erase (j);
                  if (i->second->empty ())
                    {
                      delete i->second;
                      timeListeners.erase (i);
                      break;
                    }
                  j = i->second->begin ();
                }
            }
          ++j;
        }
      unlockConditionSatisfied ();
      unlock ();
      return true;
    }

  unlock ();
  return false;
}

bool
DsmccNPTProcessor::addIdListener (ITimeBaseListener *ltn)
{
  set<ITimeBaseProvider *>::iterator i;
  bool added;

  i = cidListeners.find ((ITimeBaseProvider *)ltn);
  added = (i == cidListeners.end ());
  cidListeners.insert ((ITimeBaseProvider *)ltn);

  unlockConditionSatisfied ();

  clog << "DsmccNPTProcessor::addIdListener" << endl;
  return added;
}

bool
DsmccNPTProcessor::removeIdListener (ITimeBaseListener *ltn)
{
  set<ITimeBaseProvider *>::iterator i;

  clog << "DsmccNPTProcessor::removeIdListener()" << endl;

  i = cidListeners.find ((ITimeBaseProvider *)ltn);
  if (i == cidListeners.end ())
    {
      return false;
    }

  cidListeners.erase (i);

  unlockConditionSatisfied ();

  return true;
}

unsigned char
DsmccNPTProcessor::getOccurringTimeBaseId ()
{
  return occurringTimeBaseId;
}

unsigned char
DsmccNPTProcessor::getCurrentTimeBaseId ()
{
  map<unsigned char, DsmccTimeBaseClock *>::iterator i;
  DsmccTimeBaseClock *clk;

  i = timeBaseClock.begin ();
  while (i != timeBaseClock.end ())
    {
      clk = i->second;
      if (clk->getScaleNumerator () != 0)
        {
          return i->first;
        }
      ++i;
    }

  return INVALID_CID;
}

DsmccTimeBaseClock *
DsmccNPTProcessor::getCurrentTimebase ()
{
  map<unsigned char, DsmccTimeBaseClock *>::iterator i;
  DsmccTimeBaseClock *clk;

  i = timeBaseClock.begin ();
  while (i != timeBaseClock.end ())
    {
      clk = i->second;
      if (clk)
        {
          if (clk->getScaleNumerator ())
            {
              return i->second;
            }
        }
      ++i;
    }

  return NULL;
}

void
DsmccNPTProcessor::notifyLoopToTimeListeners ()
{
  map<unsigned char, set<ITimeBaseProvider *> *>::iterator i;
  set<ITimeBaseProvider *>::iterator j;

  Thread::mutexLock (&loopMutex);
  i = loopListeners.begin ();
  if (i != loopListeners.end ())
    {
      j = i->second->begin ();
      while (j != i->second->end ())
        {
          clog << "DsmccNPTProcessor::notifyLoopToTimeListeners ";
          clog << "CALL loop detected" << endl;
          ((ITimeBaseListener *)(*j))->loopDetected ();
          ++j;
        }
      ++i;
    }
  Thread::mutexUnlock (&loopMutex);
}

void
DsmccNPTProcessor::notifyNaturalEndListeners (unsigned char cid,
                                              double nptValue)
{
  map<unsigned char, set<ITimeBaseProvider *> *>::iterator i;
  set<ITimeBaseProvider *>::iterator j;

  Thread::mutexLock (&loopMutex);

  i = loopListeners.find (cid);
  if (i != loopListeners.end ())
    {
      j = i->second->begin ();
      while (j != i->second->end ())
        {
          clog << "DsmccNPTProcessor::notifyNaturalEndListeners ";
          clog << "cid " << (cid & 0xFF) << ", nptValue = " << nptValue;
          clog << endl;
          ((ITimeBaseListener *)(*j))->timeBaseNaturalEnd (cid, nptValue);
          ++j;
        }
    }

  Thread::mutexUnlock (&loopMutex);
}

void
DsmccNPTProcessor::notifyTimeListeners (unsigned char cid, double nptValue)
{
  map<unsigned char,
      map<TimeControl *, set<ITimeBaseProvider *> *> *>::iterator i;
  map<TimeControl *, set<ITimeBaseProvider *> *>::iterator j;
  set<ITimeBaseProvider *>::iterator k;

  clog << "DsmccNPTProcessor::notifyTimeListeners "
       << "cid '" << (cid & 0xFF);
  clog << "' nptvalue '" << nptValue << "'" << endl;

  i = timeListeners.find (cid);
  if (i != timeListeners.end ())
    {
      j = i->second->begin ();
      while (j != i->second->end ())
        {
          if (xnumeq (j->first->time, nptValue))
            {
              break;
            }
          ++j;
        }
      if (j != i->second->end ())
        {
          k = j->second->begin ();
          while (k != j->second->end ())
            {
              ((ITimeBaseListener *)(*k))->valueReached (cid, nptValue);
              ++k;
            }
        }
    }
}

void
DsmccNPTProcessor::notifyIdListeners (unsigned char oldCid,
                                      unsigned char newCid)
{
  set<ITimeBaseProvider *>::iterator i;

  i = cidListeners.begin ();
  while (i != cidListeners.end ())
    {
      clog << "DsmccNPTProcessor::notifyIdListeners" << endl;
      ((ITimeBaseListener *)(*i))->updateTimeBaseId (oldCid, newCid);
      ++i;
    }
}

DsmccTimeBaseClock *
DsmccNPTProcessor::getTimeBaseClock (unsigned char cid)
{
  map<unsigned char, DsmccTimeBaseClock *>::iterator i;

  i = timeBaseClock.find (cid);
  if (i != timeBaseClock.end ())
    {
      return i->second;
    }

  return NULL;
}

int
DsmccNPTProcessor::updateTimeBase (DsmccTimeBaseClock *clk,
                                   DsmccNPTReference *npt)
{
  uint64_t value, stcValue;

  if ((!clk) || (!npt))
    {
      return -1;
    }

  stcValue = getSTCValue ();
  if (stcValue >= npt->getStcRef ())
    {
      value = stcValue - npt->getStcRef ();
      value = DsmccTimeBaseClock::convertToNpt (
          value, npt->getScaleNumerator (), npt->getScaleDenominator ());
      value = npt->getNptRef () + value;
    }
  else
    {
      value = npt->getStcRef () - stcValue;
      value = DsmccTimeBaseClock::convertToNpt (
          value, npt->getScaleNumerator (), npt->getScaleDenominator ());
      if (npt->getNptRef () >= value)
        {
          value = npt->getNptRef () - value;
        }
      else
        {
          value = npt->getNptRef ();
        }
    }
  clk->setContentId (npt->getContentId ());
  clk->setScaleNumerator (npt->getScaleNumerator ());
  clk->setScaleDenominator (npt->getScaleDenominator ());
  clk->setReference (value, 0);

  return 0;
}

double
DsmccNPTProcessor::getCurrentTimeValue (unsigned char timeBaseId)
{
  map<unsigned char, DsmccTimeBaseClock *>::iterator i;
  DsmccTimeBaseClock *clk;

  i = timeBaseClock.find (timeBaseId);
  if (i != timeBaseClock.end ())
    {
      clk = i->second;
      return clk->getBaseToSecond ();
    }

  return 0;
}

void
DsmccNPTProcessor::detectLoop ()
{
  DsmccTimeBaseClock *clk;
  map<unsigned char, Stc *>::iterator i;

  if (getSTCValue ())
    {
      if (isFirstStc)
        {
          isFirstStc = false;
          firstStc = getSTCValue ();
        }
      else
        {
          if ((firstStc + 70000) > getSTCValue ())
            {
              Thread::mutexLock (&lifeMutex);
              i = timeBaseLife.begin ();
              while (i != timeBaseLife.end ())
                {
                  clk = getTimeBaseClock (i->first);
                  if (clk)
                    {
                      notifyNaturalEndListeners (i->first,
                                                 clk->getBaseToSecond ());
                    }
                  ++i;
                }
              Thread::mutexUnlock (&lifeMutex);
              // possible loop in TS
              isFirstStc = true;
              clearTables ();
              if (loopListeners.size ())
                {
                  notifyLoopToTimeListeners ();
                }
            }
        }
    }
}

int
DsmccNPTProcessor::decodeDescriptors (vector<DsmccMpegDescriptor *> *list)
{
  vector<DsmccMpegDescriptor *>::iterator it;
  DsmccTimeBaseClock *clk = NULL;
  map<unsigned char, DsmccTimeBaseClock *>::iterator itBase;
  map<unsigned char, Stc *>::iterator itLife;
  vector<pair<bool, DsmccNPTReference *> *>::iterator itEvent;
  DsmccNPTEndpoint *nptEP = NULL;
  DsmccNPTReference *npt = NULL;
  DsmccNPTReference *newNpt;
  int nptLen;
  char *stream;
  DsmccMpegDescriptor *desc = NULL;

  assert (list != NULL);

  if (list->empty ())
    {
      if (nptPrinter)
        {
          cout << "Can't decode NPT: 0 NPT descriptors found" << endl;
        }
      return -1;
    }

  detectLoop ();

  it = list->begin ();
  while (it != list->end ())
    {
      desc = *it;
      if (desc->getDescriptorTag () == 0x01)
        {
          if (!xnumeq (getNPTValue (getCurrentTimeBaseId ()), 0.0))
            {
              clog << "DsmccNPTProcessor::decodeDescriptors - cmp = ";
              clog << getNPTValue (getCurrentTimeBaseId ()) << endl;
            }

          // NPT reference
          npt = (DsmccNPTReference *)desc;

          if (nptPrinter)
            {
              DsmccNPTReference *lastNpt = NULL;
              cout << "FOUND NEW NPT REFERENCE DESCRIPTOR" << endl;
              cout << "CONTENTID: " << (npt->getContentId () & 0xFF)
                   << endl;
              cout << "NPT REFERENCE: " << npt->getNptRef () << " ("
                   << Stc::baseToSecond (npt->getNptRef ()) << "s)" << endl;
              cout << "STC REFERENCE: " << npt->getStcRef () << " ("
                   << Stc::baseToSecond (npt->getStcRef ()) << "s)" << endl;
              cout << "DISCONTINUITY INDICATOR: ";
              cout << (npt->getPostDiscontinuityIndicator () & 0xFF)
                   << endl;
              cout << "NPT SCALE: " << npt->getScaleNumerator () << "/"
                   << npt->getScaleDenominator () << endl;

              if (lastNptList.count (npt->getContentId ()))
                {
                  lastNpt = lastNptList[npt->getContentId ()];
                }
              else
                {
                  lastNpt = NULL;
                }
              if (lastNpt)
                {
                  if ((npt->getScaleNumerator () > 0)
                      && (lastNpt->getScaleNumerator () > 0))
                    {
                      if (lastNpt->getNptRef () >= npt->getNptRef ())
                        {
                          cout << "Error in NPT references: ";
                          cout << "Current NPT value must be greater than "
                                  "last NPT."
                               << endl;
                          cout << "Last NPT: "
                               << Stc::baseToSecond (lastNpt->getNptRef ())
                               << "s" << endl;
                          cout << "Curr NPT: "
                               << Stc::baseToSecond (npt->getNptRef ())
                               << "s" << endl;
                        }
                    }
                  else if ((npt->getScaleNumerator () < 0)
                           && (lastNpt->getScaleNumerator () < 0))
                    {
                      cout << "Error in NPT reference: ";
                      cout << "Current NPT value must be lower than last "
                              "NPT."
                           << endl;
                      cout << "Last NPT: "
                           << Stc::baseToSecond (lastNpt->getNptRef ())
                           << "s" << endl;
                      cout << "Curr NPT: "
                           << Stc::baseToSecond (npt->getNptRef ()) << "s"
                           << endl;
                    }
                  if ((npt->getScaleNumerator () != 0)
                      && (lastNpt->getScaleNumerator () != 0))
                    {
                      if (npt->getNptRef () == lastNpt->getNptRef ())
                        {
                          cout << "Error in NPT references: ";
                          cout << "NPT values are not changing in time."
                               << endl;
                        }
                    }
                  if (npt->getStcRef () < lastNpt->getStcRef ())
                    {
                      cout << "Error in STC reference: ";
                      cout << "Current STC value must be greater than last "
                              "STC."
                           << endl;
                    }
                  delete lastNpt;
                  lastNpt = NULL;
                }
              if ((npt->getScaleNumerator () != 0)
                  && (npt->getScaleDenominator () == 0))
                {
                  cout << "Error in NPT scale: ";
                  cout << "Denominator value must be non-zero." << endl;
                }
              if ((npt->getScaleNumerator () == 0)
                  && (npt->getScaleDenominator () == 0))
                {
                  cout << "Warning on NPT scale: ";
                  cout << "Scale is not available." << endl;
                }
              newNpt = new DsmccNPTReference ();
              nptLen = npt->getStream (&stream);
              newNpt->addData (stream, nptLen);
              lastNptList[npt->getContentId ()] = newNpt;
              return 0;
            }

          if ((npt->getScaleNumerator () != 0)
              && (npt->getScaleDenominator () != 0))
            {
              occurringTimeBaseId = npt->getContentId ();
            }

          // Search for existing time base
          itBase = timeBaseClock.find (npt->getContentId ());

          if (itBase == timeBaseClock.end ())
            {
              // Time base not exists
              newNpt = new DsmccNPTReference ();
              nptLen = npt->getStream (&stream);
              newNpt->addData (stream, nptLen);

              Thread::mutexLock (&schedMutex);
              if (scheduledNpts.count (npt->getContentId ()))
                {
                  delete scheduledNpts[npt->getContentId ()];
                }

              scheduledNpts[npt->getContentId ()] = newNpt;
              Thread::mutexUnlock (&schedMutex);

              unlockConditionSatisfied ();
              if (newNpt->getScaleNumerator ())
                {
                  clog << "DsmccNPTProcessor::decodeDescriptors - "
                          "Scheduling ";
                  clog << "new timebase: Transition to '";
                  clog << (newNpt->getContentId () & 0xFF);
                  clog << "' (NPT STC_reference = '"
                       << newNpt->getStcRef ();
                  clog << "'; System STC = '";
                  clog << getSTCValue () << "')" << endl;
                }
              else
                {
                  clog << "DsmccNPTProcessor::decodeDescriptors - "
                          "Scheduling ";
                  clog << "new timebase: CID ";
                  clog << (newNpt->getContentId () & 0xFF);
                  clog << " will be kept as paused." << endl;
                }
            }
          else
            {
              // time base exists
              clk = itBase->second;

              if ((clk->getScaleNumerator () != npt->getScaleNumerator ())
                  || (clk->getScaleDenominator ()
                      != npt->getScaleDenominator ())
                  || (clk->getContentId () != npt->getContentId ()))
                {
                  // It's a future change
                  newNpt = new DsmccNPTReference ();
                  nptLen = npt->getStream (&stream);
                  newNpt->addData (stream, nptLen);

                  Thread::mutexLock (&schedMutex);
                  if (scheduledNpts.count (npt->getContentId ()))
                    {
                      delete scheduledNpts[npt->getContentId ()];
                    }

                  scheduledNpts[npt->getContentId ()] = newNpt;
                  Thread::mutexUnlock (&schedMutex);

                  unlockConditionSatisfied ();
                  if (npt->getScaleNumerator ())
                    {
                      clog << "DsmccNPTProcessor::decodeDescriptors - ";
                      clog << "Scheduling existent timebase: ";
                      clog << "Transition to ";
                      clog << (newNpt->getContentId () & 0xFF) << endl;
                    }
                  else
                    {
                      clog << "Scheduling existent timebase: CID ";
                      clog << (newNpt->getContentId () & 0xFF);
                      clog << " will be paused." << endl;
                    }
                }
              else
                {
                  // Just an update
                  updateTimeBase (clk, npt);
                }

              // else it's just the current NPT value and should be ignored
              Thread::mutexLock (&lifeMutex);
              itLife = timeBaseLife.find (npt->getContentId ());
              if (itLife != timeBaseLife.end ())
                {
                  // redefine time base lifetime
                  itLife->second->setReference (0);
                }
              Thread::mutexUnlock (&lifeMutex);
            }
        }
      else if (desc->getDescriptorTag () == 0x02)
        {
          // NPT endpoint
          nptEP = (DsmccNPTEndpoint *)desc;

          if (nptPrinter)
            {
              cout << "FOUND NEW NPT ENDPOINT DESCRIPTOR" << endl;
              cout << "START NPT: "
                   << Stc::baseToSecond (nptEP->getStartNPT ()) << "s"
                   << endl;
              cout << "STOP  NPT: "
                   << Stc::baseToSecond (nptEP->getStopNPT ()) << "s"
                   << endl;
            }

          clk = getCurrentTimebase ();
          // set NPT start and stop
          if (clk != NULL)
            {
              if (!clk->getEndpointAvailable ())
                {
                  clk->setStartNpt (nptEP->getStartNPT ());
                  clk->setStopNpt (nptEP->getStopNPT ());
                  clk->setEndpointAvailable (true);
                  clog << "DsmccNPTProcessor::decodeDescriptors - CID ";
                  clog << (clk->getContentId () & 0xFF) << " starts at ";
                  clog << Stc::stcToSecond (clk->getStartNpt () * 300);
                  clog << " and stops at ";
                  clog << Stc::stcToSecond (clk->getStopNpt () * 300)
                       << endl;
                }
            }
        }
      else
        {
          clog << "DsmccNPTProcessor::decodeDescriptors unknown NPT "
                  "descriptor ";
          clog << "tag: " << desc->getDescriptorTag () << endl;

          if (nptPrinter)
            {
              cout << "DsmccNPTProcessor::decodeDescriptors unknown NPT ";
              cout << "descriptor tag: " << desc->getDescriptorTag ()
                   << endl;
            }
        }
      ++it;
    }

  return timeBaseClock.size ();
}

double
DsmccNPTProcessor::getNPTValue (unsigned char contentId)
{
  DsmccTimeBaseClock *clk;

  clk = getTimeBaseClock (contentId);
  if (clk != NULL)
    {
      return clk->getBaseToSecond ();
    }

  return 0;
}

char
DsmccNPTProcessor::getNextNptValue (char cid, double *nextNptValue,
                                    double *sleepTime)
{
  map<unsigned char, DsmccNPTReference *>::iterator it;
  map<unsigned char,
      map<TimeControl *, set<ITimeBaseProvider *> *> *>::iterator i;
  map<TimeControl *, set<ITimeBaseProvider *> *>::iterator j;
  map<unsigned char, Stc *>::iterator k;
  map<unsigned char, DsmccTimeBaseClock *>::iterator l;
  double remaining1 = MAX_NPT_VALUE;
  double remaining2 = MAX_NPT_VALUE;
  double remaining3 = MAX_NPT_VALUE;
  double value, r, minor = 0.0;
  DsmccTimeBaseClock *clk;
  char ret = 0; // 0 == schedNpt, 1 == listener
  uint64_t stcValue;
  Stc *cstc;
  TimeControl *timeControl;

  *sleepTime = 0.0;

  if (timeBaseLife.empty ())
    return -1;

  stcValue = getSTCValue ();
  if (stcValue)
    {
      Thread::mutexLock (&schedMutex);
      it = scheduledNpts.begin ();
      while (it != scheduledNpts.end ())
        {
          r = Stc::baseToSecond (it->second->getStcRef () - stcValue);
          if (r < 0)
            r = 0.0;
          if (r < remaining1)
            {
              remaining1 = r;
            }
          ++it;
        } // remaining time for future changes in NPT or a new timebase.
      Thread::mutexUnlock (&schedMutex);
    }

  clk = getTimeBaseClock (cid);
  if (clk)
    {
      value = clk->getBaseToSecond ();
      lock ();
      i = timeListeners.find (cid);
      if (i != timeListeners.end ())
        {
          j = i->second->begin ();
          while (j != i->second->end ())
            {
              timeControl = j->first;
              if (!timeControl->notified)
                {
                  r = timeControl->time - value;
                  if (r < 0)
                    {
                      r = 0.0;
                    }

                  if (r < remaining2)
                    {
                      remaining2 = r;
                      *nextNptValue = timeControl->time;
                    }
                }
              ++j;
            }
        }
      unlock ();
    } // remaining time for future events.

  Thread::mutexLock (&lifeMutex);
  k = timeBaseLife.begin ();
  while (k != timeBaseLife.end ())
    {
      cstc = k->second;
      l = timeBaseClock.find (k->first);
      if ((l != timeBaseClock.end ()) && l->second)
        {
          value = 60.0 - cstc->getBaseToSecond ();
          if (value < 0)
            value = 0.0;
          if (value < remaining3)
            {
              remaining3 = value;
            }
          if (l->second->getScaleNumerator ()
              && l->second->getEndpointAvailable ())
            {
              r = Stc::baseToSecond (l->second->getStopNpt ())
                  - l->second->getBaseToSecond ();

              if (r < 0)
                {
                  r = 0.0;
                }

              if (r < remaining3)
                {
                  remaining3 = r;
                  // TODO: we need a bug fix here
                  //*nextNptValue = j->first->time;
                }
            }
        }
      ++k;
    }
  Thread::mutexUnlock (&lifeMutex);

  if ((remaining3 < remaining1) && (remaining3 < remaining2))
    {
      minor = remaining3;
    }
  else
    {
      Thread::mutexLock (&schedMutex);
      if (scheduledNpts.empty () && !clk)
        {
          minor = 0.0;
        }
      else
        {
          if (!scheduledNpts.empty () && clk)
            {
              if (remaining1 > remaining2)
                {
                  minor = remaining2;
                  ret = 1;
                }
              else
                {
                  minor = remaining1;
                } // which one is lower?
            }
          else if (!scheduledNpts.empty ())
            {
              if (stcValue)
                {
                  minor = remaining1;
                }
              else
                {
                  minor = 0.0;
                }
            }
          else
            {
              minor = remaining2;
              ret = 1;
            }
        }
      Thread::mutexUnlock (&schedMutex);
    }

  *sleepTime = minor * 1000; // convert to milliseconds

  return ret;
}

bool
DsmccNPTProcessor::processNptValues ()
{
  double nextNptValue, sleepTime;
  bool timedOut;
  char notify;
  bool restart = true;
  char cid;
  DsmccTimeBaseClock *clk;
  Stc *tblife;
  DsmccNPTReference *npt;
  map<unsigned char, DsmccTimeBaseClock *>::iterator itClk;
  map<unsigned char, DsmccNPTReference *>::iterator it;
  map<unsigned char,
      map<TimeControl *, set<ITimeBaseProvider *> *> *>::iterator i;
  map<TimeControl *, set<ITimeBaseProvider *> *>::iterator j;

  cid = getCurrentTimeBaseId ();

  notify = getNextNptValue (cid, &nextNptValue, &sleepTime);
  if (sleepTime > 0.0)
    {
      timedOut = Thread::mSleep ((long int)sleepTime);
      if (!timedOut)
        {
          return false;
        }
    }

  if (notify == 1)
    {
      lock ();
      i = timeListeners.find (cid);
      if (i != timeListeners.end ())
        {
          j = i->second->begin ();
          while (j != i->second->end ())
            {
              if (!j->first->notified)
                {
                  if (xnumeq (j->first->time, nextNptValue))
                    {
                      notifyTimeListeners (cid, nextNptValue);
                      j->first->notified = true;
                      reScheduleIt = true;
                    }
                }
              ++j;
            }
        }
      unlock ();
    }

  while (restart)
    {
      restart = false;
      Thread::mutexLock (&schedMutex);
      it = scheduledNpts.begin ();
      while (it != scheduledNpts.end ())
        {
          npt = it->second;
          if (getSTCValue () >= npt->getStcRef ())
            {
              itClk = timeBaseClock.find (npt->getContentId ());
              if (itClk == timeBaseClock.end ())
                {
                  clk = new DsmccTimeBaseClock ();
                  timeBaseClock[npt->getContentId ()] = clk;
                  tblife = new Stc ();
                  tblife->setReference (0);
                  Thread::mutexLock (&lifeMutex);
                  timeBaseLife[npt->getContentId ()] = tblife;
                  Thread::mutexUnlock (&lifeMutex);
                  clog << "DsmccNPTProcessor::processNptValues - Added "
                          "CID: ";
                  clog << (npt->getContentId () & 0xFF) << endl;
                }
              else
                {
                  clk = timeBaseClock[npt->getContentId ()];
                }
              updateTimeBase (clk, npt);
              delete it->second;
              scheduledNpts.erase (it);
              restart = true;
              lock ();
              notifyIdListeners (currentCid, clk->getContentId ());
              unlock ();
              currentCid = clk->getContentId ();
              reScheduleIt = true;
              clog << "DsmccNPTProcessor::processNptValues - Executing "
                      "scheduled NPT: "
                   << (clk->getContentId () & 0xFF) << " as "
                   << clk->getScaleNumerator () << endl;
              break;
            }
          ++it;
        }
      Thread::mutexUnlock (&schedMutex);
    }

  clearUnusedTimebase ();

  return true;
}

void
DsmccNPTProcessor::run ()
{
  bool hasTimeListeners = false;

  while (running)
    {
      lock ();
      hasTimeListeners = !timeListeners.empty ();
      unlock ();

      Thread::mutexLock (&schedMutex);
      if (scheduledNpts.empty () && !hasTimeListeners)
        {
          Thread::mutexUnlock (&schedMutex);
          if (reScheduleIt)
            {
              reScheduleIt = false;
              processNptValues ();
            }
          else
            {
              waitForUnlockCondition ();
            }
        }
      else
        {
          Thread::mutexUnlock (&schedMutex);
          processNptValues ();
        }
    }
}

GINGA_DATAPROC_END
