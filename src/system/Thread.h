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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef __THREAD_H__
#define __THREAD_H__

#include "SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;


BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_THREAD_BEGIN


  class Thread {
  protected:
    bool isDeleting;

    pthread_mutex_t threadMutex;

    bool isThreadSleeping;
    pthread_mutex_t threadFlagMutex;

    bool isWaiting;
    pthread_mutex_t threadFlagMutexLockUntilSignal;
    pthread_cond_t threadFlagCVLockUntilSignal;

  private:
    pthread_mutex_t threadIdMutex;

    pthread_attr_t tattr;

  protected:
    virtual void run()=0;

  private:
    pthread_t threadId_;
    static void* function(void* ptr);

  public:
    Thread();
    virtual ~Thread();
    virtual void startThread();
    bool sleep(long int seconds);
    bool mSleep(long int milliseconds);
    void lock();
    void unlock();
    void waitForUnlockCondition();
    bool unlockConditionSatisfied();

    /* Mutex functions*/
    static void mutexInit(pthread_mutex_t* mutex, bool recursive=false);
    static void mutexDestroy(pthread_mutex_t* mutex);
    static void mutexLock(pthread_mutex_t* mutex);
    static void mutexUnlock(pthread_mutex_t* mutex);

    /* Cond functions */
    static void condInit(
                         pthread_cond_t* cond, const pthread_condattr_t * attr);

    static void condDestroy(pthread_cond_t* cond);
    static void condWait(pthread_cond_t* cond, pthread_mutex_t* mutex);
    static void condSignal(pthread_cond_t* cond);

    /* Create functions */
    static void startNewThread(void* (*funcion) (void*), void* ptr);

  };

BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_THREAD_END
#endif //__THREAD_H__
