/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

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

#ifndef _COMPOSITEEXECUTIONOBJECT_H_
#define _COMPOSITEEXECUTIONOBJECT_H_

extern "C" {
#include <assert.h>
}

#include "ncl/components/Node.h"
#include "ncl/components/ContextNode.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "IEventListener.h"
#include "FormatterEvent.h"
#include "PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "FormatterCausalLink.h"
#include "FormatterLink.h"
#include "LinkListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "NodeNesting.h"
#include "CascadingDescriptor.h"
#include "ExecutionObject.h"

#include <vector>
#include <map>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	class CompositeExecutionObject :
			public ExecutionObject, public LinkListener, public IEventListener,
			public Thread {

		private:
			static const short mSleepTime = 800;
			set<FormatterLink*> links;
			set<Link*> uncompiledLinks;

			set<FormatterEvent*> runningEvents; // child events occurring
			set<FormatterEvent*> pausedEvents;  // child events paused
			short lastTransition;

			map<FormatterLink*, int> pendingLinks;
			bool running;

			map<string, ExecutionObject*> execObjList;

			pthread_mutex_t compositeMutex;
			pthread_mutex_t parentMutex;
			pthread_mutex_t stlMutex;

		public:
			CompositeExecutionObject(
					string id,
					Node *dataObject,
					bool handling,
					ILinkActionListener* seListener);

			CompositeExecutionObject(
					string id,
					Node *dataObject,
					CascadingDescriptor *descriptor,
					bool handling,
					ILinkActionListener* seListener);

			virtual ~CompositeExecutionObject();

		protected:
			void initializeCompositeExecutionObject(
					string id,
					Node *dataObject,
					CascadingDescriptor *descriptor);

		public:
			CompositeExecutionObject* getParentFromDataObject(Node* dataObject);
			void suspendLinkEvaluation(bool suspend);
			bool addExecutionObject(ExecutionObject *execObj);
			bool containsExecutionObject(string execObjId);
			ExecutionObject* getExecutionObject(string execObjId);
			map<string, ExecutionObject*>* getExecutionObjects();
			map<string, ExecutionObject*>* recursivellyGetExecutionObjects();
			int getNumExecutionObjects();
			bool removeExecutionObject(ExecutionObject* execObj);
			set<Link*>* getUncompiledLinks();
			bool containsUncompiledLink(Link* dataLink);
			void removeLinkUncompiled(Link* ncmLink);
			void setLinkCompiled(FormatterLink *formatterLink);
			void addNcmLink(Link *ncmLink);
			void removeNcmLink(Link *ncmLink);
			void setAllLinksAsUncompiled(bool isRecursive);
			void setParentsAsListeners();
			void unsetParentsAsListeners();
			void eventStateChanged(
					void* event, short transition, short previousState);

			short getPriorityType();

			void linkEvaluationStarted(FormatterCausalLink *link);
			void linkEvaluationFinished(FormatterCausalLink *link, bool start);

			bool setPropertyValue(AttributionEvent* event, string value);

		private:
			void checkLinkConditions();
			void run();
			void listRunningObjects();
			void listPendingLinks();

			void lockComposite();
			void unlockComposite();

			void lockSTL();
			void unlockSTL();
	};
}
}
}
}
}
}
}

#endif //_COMPOSITEEXECUTIONOBJECT_H_
