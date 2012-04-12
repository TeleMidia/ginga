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

#ifndef SmilPlayer_H_
#define SmilPlayer_H_

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "mb/IInputManager.h"
#include "mb/interface/IInputEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "mb/ILocalScreenManager.h"
#include "mb/interface/IWindow.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "Player.h"

#include <string>
using namespace std;

// ambulant player includes
#include "ambulant/version.h"
#include "ambulant/common/player.h"
#include "ambulant/common/plugin_engine.h"
#include "ambulant/net/url.h"
#include "ambulant/lib/logger.h"

/* Workaround to ambulant header files that are needed but not installed */
#include "ambulant/player_gtk/gtk_mainloop.h"

//GTK
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

//XXX temp source code from npambulant
//some fake gtk_gui functions needed by gtk_mainloop
void gtk_gui::internal_message(int, char*) {}
GtkWidget* gtk_gui::get_document_container() { return m_documentcontainer; }
//XXXX FIXME fake gtk_gui constructor 1st arg is used as GtkWindow, 2nd arg as smilfile
gtk_gui::gtk_gui(const char* s, const char* s2) {
	cout << "gtk_gui::gtk_gui " << endl;
	memset (this, 0, sizeof(gtk_gui));

	m_toplevelcontainer = (GtkWindow*) s;
	m_documentcontainer = gtk_drawing_area_new();
	gtk_widget_hide(m_documentcontainer);
//XXXX FIXME vbox only needed to give	m_documentcontainer a parent widget at *draw() callback time
	m_guicontainer = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(m_toplevelcontainer), GTK_WIDGET (m_guicontainer));
	gtk_box_pack_start (GTK_BOX(m_guicontainer), m_documentcontainer, TRUE, TRUE, 0);
//XXXX not used:  m_guicontainer = menubar = NULL;
//XXXX FIXME <EMBED src="xxx" ../> attr value is 2nd contructor arg.
	m_smilfilename = s2;
	main_loop = g_main_loop_new(NULL, FALSE);

	cout << "gtk_gui::gtk_gui all done" << endl;
}

gtk_gui::~gtk_gui() {
	g_object_unref (G_OBJECT (main_loop));
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class SmilPlayer :
			public Player, public IInputEventListener, public Thread {

		private:
			IInputManager* im;
			int x, y, w, h;
			ambulant::common::player* ambulantPlayer;

			//TODO: make it independent from GTK
			gtk_mainloop* mainLoop;
			GtkWidget* gtkwidget;
			gtk_gui* gui;

			lib::document* smilDoc;
			lib::node* smilRootNode;
			const lib::node_context* smilNodeContext;
			common::state_component* smilStateCmp;

		public:
			SmilPlayer(GingaScreenID screenId, string mrl);
			virtual ~SmilPlayer();

		private:
			void initPrefs();
			void initGui();

		public:
			ISurface* getSurface();

			bool userEventReceived(IInputEvent* ev);

			void setNotifyContentUpdate(bool notify);

			void play();
			void stop();
			void pause();
			void resume();

			bool setOutWindow(GingaWindowID windowId);
			void setBounds(int x, int y, int w, int h);

			void setPropertyValue(string name, string value);

			bool setKeyHandler(bool isHandler);

		protected:
			void run();
	};
}
}
}
}
}
}

#endif /*SmilPlayer_H_*/
