/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen� Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�o na expectativa de que seja util, porem, SEM
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

#include "system/thread/Thread.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace thread {
	Thread::Thread() {
		isDeleting = false;

		pthread_mutex_init(&threadMutex, NULL);

		isThreadSleeping = false;
		pthread_mutex_init(&threadFlagMutex, NULL);
		pthread_mutex_init(&threadFlagMutexLockUntilSignal, NULL);
		pthread_mutex_init(&threadIdMutex, NULL);

		isWaiting = false;
		pthread_cond_init(&threadFlagConditionVariable, NULL);
		pthread_cond_init(&threadFlagCVLockUntilSignal, NULL);

		pthread_attr_init(&tattr);
		pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
		pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);
	}

	Thread::~Thread() {
		isDeleting = true;

		wakeUp();
		pthread_cond_signal(&threadFlagConditionVariable);
		pthread_cond_destroy(&threadFlagConditionVariable);

		unlockConditionSatisfied();
		pthread_cond_signal(&threadFlagCVLockUntilSignal);
		pthread_cond_destroy(&threadFlagCVLockUntilSignal);

		pthread_mutex_lock(&threadMutex);
		pthread_mutex_unlock(&threadMutex);
		pthread_mutex_destroy(&threadMutex);

		pthread_mutex_lock(&threadFlagMutex);
		pthread_mutex_unlock(&threadFlagMutex);
		pthread_mutex_destroy(&threadFlagMutex);

		pthread_mutex_lock(&threadFlagMutexLockUntilSignal);
		pthread_mutex_unlock(&threadFlagMutexLockUntilSignal);
		pthread_mutex_destroy(&threadFlagMutexLockUntilSignal);

		pthread_mutex_lock(&threadIdMutex);
		pthread_mutex_unlock(&threadIdMutex);
		pthread_mutex_destroy(&threadIdMutex);

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
			pthread_mutex_lock(&threadIdMutex);
			pthread_create(&threadId_, &tattr, Thread::function, this);
			pthread_detach(threadId_);
			pthread_mutex_unlock(&threadIdMutex);
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

		pthread_mutex_lock(&threadFlagMutex);
		isThreadSleeping = true;
		res = pthread_cond_timedwait(
			    &threadFlagConditionVariable,
			    &threadFlagMutex,
			    (const struct timespec*)(&timeOut));

		isThreadSleeping = false;
		pthread_mutex_unlock(&threadFlagMutex);
		return (res == ETIMEDOUT);
	}

	void Thread::wakeUp() {
		while (isThreadSleeping) {
			pthread_cond_signal(&threadFlagConditionVariable);
		}
	}

	void Thread::lock() {
		pthread_mutex_lock(&threadMutex);
	}

	void Thread::unlock() {
		pthread_mutex_unlock(&threadMutex);
	}

	void Thread::waitForUnlockCondition() {
		isWaiting = true;
		pthread_mutex_lock(&threadFlagMutexLockUntilSignal);
		pthread_cond_wait(
			    &threadFlagCVLockUntilSignal, &threadFlagMutexLockUntilSignal);

		isWaiting = false;
		pthread_mutex_unlock(&threadFlagMutexLockUntilSignal);
	}

	bool Thread::unlockConditionSatisfied() {
		if (isWaiting) {
			pthread_cond_signal(&threadFlagCVLockUntilSignal);
			return true;
		}
		return false;
	}

	void Thread::mutexInit(pthread_mutex_t* mutex, bool recursive) {
        pthread_mutexattr_t attr;

        if (recursive) {
        	assert(pthread_mutexattr_init (&attr) == 0);
        	assert(pthread_mutexattr_settype(
        			&attr, PTHREAD_MUTEX_RECURSIVE) == 0);

            assert(pthread_mutex_init (mutex, &attr) == 0);
            assert(pthread_mutexattr_destroy (&attr) == 0);

        } else {
            assert(pthread_mutex_init (mutex, NULL) == 0);
        }
	}
}
}
}
}
}
}
}
