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

#include "Thread.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace thread {
	Thread::Thread() {
		isDeleting = false;

		Thread::mutexInit(&threadMutex);

		isThreadSleeping = false;
		Thread::mutexInit(&threadFlagMutex);
		Thread::mutexInit(&threadFlagMutexLockUntilSignal);
		Thread::mutexInit(&threadIdMutex);

		isWaiting = false;
		Thread::condInit(&threadFlagConditionVariable, NULL);
		Thread::condInit(&threadFlagCVLockUntilSignal, NULL);

		pthread_attr_init(&tattr);
		pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
		pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);
	}

	Thread::~Thread() {
		isDeleting = true;

		wakeUp();
		Thread::condSignal(&threadFlagConditionVariable);
		Thread::condDestroy(&threadFlagConditionVariable);

		unlockConditionSatisfied();
		Thread::condSignal(&threadFlagCVLockUntilSignal);
		Thread::condDestroy(&threadFlagCVLockUntilSignal);

		Thread::mutexLock(&threadMutex);
		Thread::mutexUnlock(&threadMutex);
		Thread::mutexDestroy(&threadMutex);

		Thread::mutexLock(&threadFlagMutex);
		Thread::mutexUnlock(&threadFlagMutex);
		Thread::mutexDestroy(&threadFlagMutex);

		Thread::mutexLock(&threadFlagMutexLockUntilSignal);
		Thread::mutexUnlock(&threadFlagMutexLockUntilSignal);
		Thread::mutexDestroy(&threadFlagMutexLockUntilSignal);

		Thread::mutexLock(&threadIdMutex);
		Thread::mutexUnlock(&threadIdMutex);
		Thread::mutexDestroy(&threadIdMutex);

		pthread_attr_destroy(&tattr);
	}

	void* Thread::function(void* ptr) {
		if (ptr == NULL || static_cast<Thread*>(ptr)->isDeleting) {
			return NULL;
		}

		static_cast<Thread*>(ptr)->run();
		pthread_exit(ptr);
		return NULL;
	}

	void Thread::startThread() {
		if (!isDeleting) {
			Thread::mutexLock(&threadIdMutex);
			pthread_create(&threadId_, &tattr, Thread::function, this);
			pthread_detach(threadId_);
			Thread::mutexUnlock(&threadIdMutex);
		}
	}

	bool Thread::sleep(long int seconds) {
		return this->mSleep(seconds * 1000);
	}

	bool Thread::mSleep(long int milliseconds) {
		int res;
		struct timeval time;
		struct timespec timeOut;

		SystemCompat::gettimeofday(&time, NULL);

		timeOut.tv_sec = time.tv_sec + (long int)(milliseconds / 1000);
		long int micro;
		micro = ((milliseconds%1000) * 1000) + time.tv_usec;
		if (micro > 1000000) {
			timeOut.tv_sec++;
			micro -= 1000000;
		}

		timeOut.tv_nsec = micro * 1000;

		//clog << "teste = '" << teste <<  "'" << endl;
		/*timeOut.tv_nsec = (time.tv_usec * 1000) +
			    (long int)(milliseconds % 1000);*/

		Thread::mutexLock(&threadFlagMutex);
		isThreadSleeping = true;
		res = pthread_cond_timedwait(
			    &threadFlagConditionVariable,
			    &threadFlagMutex,
			    (const struct timespec*)(&timeOut));

		isThreadSleeping = false;
		Thread::mutexUnlock(&threadFlagMutex);
		return (res == ETIMEDOUT);
	}

	bool Thread::wakeUp() {
		bool wasSleeping = false;
		while (isThreadSleeping) {
			Thread::condSignal(&threadFlagConditionVariable);
			wasSleeping = true;
		}

		return wasSleeping;
	}

	void Thread::lock() {
		Thread::mutexLock(&threadMutex);
	}

	void Thread::unlock() {
		Thread::mutexUnlock(&threadMutex);
	}

	void Thread::waitForUnlockCondition() {
		isWaiting = true;
		Thread::mutexLock(&threadFlagMutexLockUntilSignal);
		Thread::condWait(
			    &threadFlagCVLockUntilSignal, &threadFlagMutexLockUntilSignal);

		isWaiting = false;
		Thread::mutexUnlock(&threadFlagMutexLockUntilSignal);
	}

	bool Thread::unlockConditionSatisfied() {
		if (isWaiting) {
			Thread::condSignal(&threadFlagCVLockUntilSignal);
			return true;
		}
		return false;
	}

	void Thread::mutexInit(pthread_mutex_t* mutex, bool recursive) {
        pthread_mutexattr_t attr;

        if (recursive) {
        	assert(pthread_mutexattr_init(&attr) == 0);
        	assert(pthread_mutexattr_settype(
        			&attr, PTHREAD_MUTEX_RECURSIVE) == 0);

            assert(pthread_mutex_init(mutex, &attr) == 0);
            assert(pthread_mutexattr_destroy (&attr) == 0);

        } else {
            assert(pthread_mutex_init(mutex, NULL) == 0);
        }
	}

	void Thread::mutexDestroy(pthread_mutex_t* mutex) {
		assert(mutex != NULL);
		assert(pthread_mutex_destroy(mutex) == 0);
	}

	void Thread::mutexLock(pthread_mutex_t* mutex) {
		assert(mutex != NULL);
		assert(pthread_mutex_lock(mutex) == 0);
	}

	void Thread::mutexUnlock(pthread_mutex_t* mutex) {
		assert(mutex != NULL);
		assert(pthread_mutex_unlock(mutex) == 0);
	}

	void Thread::condInit(
			pthread_cond_t* cond, const pthread_condattr_t * attr) {

		assert(cond != NULL);
		assert(pthread_cond_init(cond, NULL) == 0);
	}

	void Thread::condDestroy(pthread_cond_t* cond) {
		assert(cond != NULL);
		assert(pthread_cond_destroy(cond) == 0);
	}

	void Thread::condWait(pthread_cond_t* cond, pthread_mutex_t* mutex) {
		assert(cond != NULL);
		assert(mutex != NULL);
		assert(pthread_cond_wait(cond, mutex) == 0);
	}

	void Thread::condSignal(pthread_cond_t* cond) {
		assert(cond != NULL);
		assert(pthread_cond_signal(cond) == 0);
	}

	void Thread::startNewThread(void* (*foo) (void*), void* ptr) {
		pthread_attr_t tattr;
		pthread_t threadId_;

		pthread_attr_init(&tattr);
		pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
		pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

		pthread_create(&threadId_, &tattr, foo, ptr);
		pthread_detach(threadId_);
	}
}
}
}
}
}
}
}
