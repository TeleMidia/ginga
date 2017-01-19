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

#include "ginga.h"
#include "Thread.h"

GINGA_SYSTEM_BEGIN

Thread::Thread ()
{
  isDeleting = false;

  Thread::mutexInit (&threadMutex);

  isThreadSleeping = false;
  Thread::mutexInit (&threadFlagMutex);
  Thread::mutexInit (&threadFlagMutexLockUntilSignal);
  Thread::mutexInit (&threadIdMutex);

  isWaiting = false;
  Thread::condInit (&threadFlagCVLockUntilSignal, NULL);

  pthread_attr_init (&tattr);
  pthread_attr_setdetachstate (&tattr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setscope (&tattr, PTHREAD_SCOPE_SYSTEM);
}

Thread::~Thread ()
{
  isDeleting = true;

  unlockConditionSatisfied ();
  Thread::condSignal (&threadFlagCVLockUntilSignal);
  Thread::condDestroy (&threadFlagCVLockUntilSignal);

  Thread::mutexLock (&threadMutex);
  Thread::mutexUnlock (&threadMutex);
  Thread::mutexDestroy (&threadMutex);

  Thread::mutexLock (&threadFlagMutex);
  Thread::mutexUnlock (&threadFlagMutex);
  Thread::mutexDestroy (&threadFlagMutex);

  Thread::mutexLock (&threadFlagMutexLockUntilSignal);
  Thread::mutexUnlock (&threadFlagMutexLockUntilSignal);
  Thread::mutexDestroy (&threadFlagMutexLockUntilSignal);

  Thread::mutexLock (&threadIdMutex);
  Thread::mutexUnlock (&threadIdMutex);
  Thread::mutexDestroy (&threadIdMutex);

  pthread_attr_destroy (&tattr);
}

void *
Thread::function (void *ptr)
{
  if (ptr == NULL || static_cast<Thread *> (ptr)->isDeleting)
    return NULL;

  static_cast<Thread *> (ptr)->run ();
  pthread_exit (ptr);
  return NULL;
}

void
Thread::startThread ()
{
  if (isDeleting)
    return;

  Thread::mutexLock (&threadIdMutex);
  pthread_create (&threadId_, &tattr, Thread::function, this);
  pthread_detach (threadId_);
  Thread::mutexUnlock (&threadIdMutex);
}

bool
Thread::sleep (long int s)
{
  g_usleep (s * 1000000);
  return true;
}

bool
Thread::mSleep (long int ms)
{
  g_usleep (ms * 1000);
  return true;
}

void
Thread::lock ()
{
  Thread::mutexLock (&threadMutex);
}

void
Thread::unlock ()
{
  Thread::mutexUnlock (&threadMutex);
}

void
Thread::waitForUnlockCondition ()
{
  isWaiting = true;
  Thread::mutexLock (&threadFlagMutexLockUntilSignal);
  Thread::condWait (&threadFlagCVLockUntilSignal,
                    &threadFlagMutexLockUntilSignal);
  isWaiting = false;
  Thread::mutexUnlock (&threadFlagMutexLockUntilSignal);
}

bool
Thread::unlockConditionSatisfied ()
{
  if (isWaiting)
    {
      Thread::condSignal (&threadFlagCVLockUntilSignal);
      return true;
    }
  return false;
}

void
Thread::mutexInit (pthread_mutex_t *mutex, bool recursive)
{
  pthread_mutexattr_t attr;
  if (recursive)
    {
      g_assert (pthread_mutexattr_init (&attr) == 0);
      g_assert (pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE)
                == 0);
      g_assert (pthread_mutex_init (mutex, &attr) == 0);
      g_assert (pthread_mutexattr_destroy (&attr) == 0);
    }
  else
    {
      g_assert (pthread_mutex_init (mutex, NULL) == 0);
    }
}

void
Thread::mutexDestroy (pthread_mutex_t *mutex)
{
  assert (mutex != NULL);
  assert (pthread_mutex_destroy (mutex) == 0);
}

void
Thread::mutexLock (pthread_mutex_t *mutex)
{
  g_assert (pthread_mutex_lock (mutex) == 0);
}

void
Thread::mutexUnlock (pthread_mutex_t *mutex)
{
  g_assert (pthread_mutex_unlock (mutex) == 0);
}

void
Thread::condInit (pthread_cond_t *cond, const pthread_condattr_t *attr)
{
  g_assert (pthread_cond_init (cond, NULL) == 0);
}

void
Thread::condDestroy (pthread_cond_t *cond)
{
  g_assert (pthread_cond_destroy (cond) == 0);
}

void
Thread::condWait (pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  g_assert (pthread_cond_wait (cond, mutex) == 0);
}

void
Thread::condSignal (pthread_cond_t *cond)
{
  g_assert (pthread_cond_signal (cond) == 0);
}

void
Thread::startNewThread (void *(*foo) (void *), void *ptr)
{
  pthread_attr_t tattr;
  pthread_t threadId_;

  pthread_attr_init (&tattr);
  pthread_attr_setdetachstate (&tattr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setscope (&tattr, PTHREAD_SCOPE_SYSTEM);

  pthread_create (&threadId_, &tattr, foo, ptr);
  pthread_detach (threadId_);
}

GINGA_SYSTEM_END
