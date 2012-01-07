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

#include "player/LuaPlayer.h"

#include "player/PlayersComponentSupport.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/io/IInputManager.h"

string cmd_to_str(lua_State* L) {
  const char* documentId = NULL;
  const char* command    = NULL;
  const int NUM_ARGS = 8;
  string str;
  string docId;
  string editCommand;
  string edit[NUM_ARGS];

  lua_getfield(L, 2, "command");
  command = luaL_checkstring(L, -1);

  lua_getfield(L, 2,"baseId");
  if ( !lua_isnil(L, -1) ) {
    const char* baseId = luaL_checkstring(L, -1);

    edit[2].append(baseId);
    lua_pop(L, 1);

  } else {
    edit[2].append(" ");
  }

  /****BASE****/
  if (!strcmp(command, "openBase")) {
    //[ dst | evt | class | command | location ]
    lua_getfield(L, 2,"location");
    const char* location = luaL_checkstring(L, -1);

    edit[1].append("0x00");
    edit[4].append(location);

    lua_pop(L, 1);

  } else if (!strcmp(command, "activateBase")) {
    edit[1].append("0x01");

  } else if (!strcmp(command, "deactivateBase")) {
    edit[1].append("0x02");
  }

  else if (!strcmp(command, "saveBase")) {
    //[ dst | evt | class | command | baseId | location ]
    lua_getfield(L, 2,"location");
    const char* location = luaL_checkstring(L, -1);
    edit[4].append(location);
    lua_pop(L, 1);

    edit[1].append("0x03");

  } else if (!strcmp(command, "closeBase")) {
    edit[1].append("0x04");
  }

  lua_getfield(L, 2,"documentId");
  if ( !lua_isnil(L, -1) ) {
    documentId = luaL_checkstring(L, -1);
    //[ dst | evt | class | command | documentId ]
    edit[3].append(documentId);
    lua_pop(L, 1);

  } else {
    edit[3].append(" ");
  }

  /****DOCUMENT****/
  if (!strcmp(command, "addDocument")) {

    lua_getfield(L, 2, "data");
    string xmlDocument = "'";
    xmlDocument += luaL_checkstring(L, -1);
    xmlDocument += "'";


    edit[1].append("0x05");
    edit[3].append(xmlDocument.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeDocument")) {
    edit[1].append("0x06");

  } else if (!strcmp(command, "saveDocument")) {
    lua_getfield(L, 2,"location");
    const char* location = luaL_checkstring(L, -1);

    edit[1].append("0x07");
    edit[4].append(location);

    lua_pop(L, 1);

  } else if (!strcmp(command, "startDocument")){
    lua_getfield(L, 2,"interfaceId");
    const char* interfaceId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"offset");
    const char* offset = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"refDocumentId");
    const char* refDocumentId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"refNodeId");
    const char* refNodeId = luaL_checkstring(L, -1);

    edit[1].append("0x07");
    edit[4].append(interfaceId);
    edit[5].append(offset);
    edit[6].append(refDocumentId);
    edit[7].append(refNodeId);

    lua_pop(L, 4);

  } else if (!strcmp(command, "stopDocument")) {
    edit[1].append("0x08");

  } else if (!strcmp(command, "pauseDocument")) {
    edit[1].append("0x09");

  } else if (!strcmp(command, "resumeDocument")) {
    edit[1].append("0x0A");

    /****REGION****/
  } else if (!strcmp(command, "addRegion")) {

    lua_getfield(L, 2,"regionBaseId");
    const char* regionBaseId = luaL_checkstring(L, -1);

    lua_getfield(L, 2, "regionId");
    const char* regionId = NULL;

    if ( !lua_isnil(L, -1) ) {
      regionId = luaL_checkstring(L, -1);
    }

    lua_getfield(L, 2,"data");
    string xmlRegion = "'";
    xmlRegion += luaL_checkstring(L, -1);
    xmlRegion += "'";

    edit[1].append("0x0B");
    edit[4].append(regionBaseId);
    edit[6].append(xmlRegion.c_str());

    if (regionId != NULL) {
      edit[5].append(regionId);
      lua_pop(L, 3);

    } else {
      edit[5].append(" ");
      lua_pop(L, 2);
    }


  } else if (!strcmp(command, "removeRegion")) {

    lua_getfield(L, 2,"regionId");
    const char* regionId = luaL_checkstring(L, -1);

    edit[1].append("0x0C");
    edit[4].append(regionId);
    lua_pop(L, 1);

  } else if (!strcmp(command, "addRegionBase")) {

    lua_getfield(L, 2,"data");
    string xmlRegionBase = "'";
    xmlRegionBase += luaL_checkstring(L, -1);
    xmlRegionBase += "'";

    edit[1].append("0x0D");
    edit[4].append(xmlRegionBase.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeRegionBase")) {

    lua_getfield(L, 2,"regionBaseId");
    const char* regionBaseId = luaL_checkstring(L, -1);

    edit[1].append("0x0E");
    edit[4].append(regionBaseId);

    lua_pop(L, 1);

    /****RULE****/
  } else if (!strcmp(command, "addRule")) {
    lua_getfield(L, 2,"data");
    string xmlRule = "'";
    xmlRule += luaL_checkstring(L, -1);
    xmlRule += "'";

    edit[1].append("0x0F");
    edit[4].append(xmlRule.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeRule")) {
    lua_getfield(L, 2,"ruleId");
    const char* ruleId = luaL_checkstring(L, -1);

    edit[1].append("0x10");
    edit[4].append(ruleId);

    lua_pop(L, 1);

  } else if (!strcmp(command, "addRuleBase")) {

    lua_getfield(L, 2,"data");
    string xmlRuleBase = "'";
    xmlRuleBase += luaL_checkstring(L, -1);
    xmlRuleBase += "'";

    edit[1].append("0x11");
    edit[4].append(xmlRuleBase.c_str());
    lua_pop(L, 1);

  } else if (!strcmp(command, "removeRuleBase")) {

    lua_getfield(L, 2,"ruleBaseId");
    const char* ruleBaseId = luaL_checkstring(L, -1);

    edit[1].append("0x12");
    edit[4].append(ruleBaseId);

    lua_pop(L, 1);

    /****CONNECTOR****/
  } else if (!strcmp(command, "addConnector")) {
    lua_getfield(L, 2,"data");
    string xmlCon = "'";
    xmlCon += luaL_checkstring(L, -1);
    xmlCon += "'";

    edit[1].append("0x13");
    edit[4].append(xmlCon.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeConnector")) {
    lua_getfield(L, 2,"connectorId");
    const char* conId = luaL_checkstring(L, -1);

    edit[1].append("0x14");
    edit[4].append(conId);

    lua_pop(L, 1);

  } else if (!strcmp(command, "addConnectorBase")) {

    lua_getfield(L, 2,"data");
    string xmlConnectorBase = "'";
    xmlConnectorBase += luaL_checkstring(L, -1);
    xmlConnectorBase += "'";

    edit[1].append("0x15");
    edit[4].append(xmlConnectorBase.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeConnectorBase")) {
    lua_getfield(L, 2,"connectorBaseId");
    const char* connectorBaseId = luaL_checkstring(L, -1);

    edit[1].append("0x16");
    edit[4].append(connectorBaseId);

    lua_pop(L, 1);

    /****DESCRIPTOR****/
  } else if (!strcmp(command, "addDescriptor")) {
    lua_getfield(L, 2,"data");
    string xmlDesc = "'";
    xmlDesc += luaL_checkstring(L, -1);
    xmlDesc += "'";

    edit[1].append("0x17");
    edit[4].append(xmlDesc.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeDescriptor")) {
    lua_getfield(L, 2,"descriptorId");
    const char* descriptorId = luaL_checkstring(L, -1);

    edit[1].append("0x18");
    edit[4].append(descriptorId);

    lua_pop(L, 1);

  } else if (!strcmp(command, "addDescriptorSwitch")) {
    lua_getfield(L, 2,"data");
    string xmlDescSwitch = "'";
    xmlDescSwitch += luaL_checkstring(L, -1);
    xmlDescSwitch += "'";

    edit[1].append("0x19");
    edit[4].append(xmlDescSwitch.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeDescriptorSwitch")) {
    lua_getfield(L, 2,"descriptorSwitchId");
    const char* descSwitchId = luaL_checkstring(L, -1);

    edit[1].append("0x1A");
    edit[4].append(descSwitchId);

    lua_pop(L, 1);

  } else if (!strcmp(command, "addDescriptorBase")) {
    lua_getfield(L, 2,"data");
    string xmlDescriptorBase = "'";
    xmlDescriptorBase += luaL_checkstring(L, -1);
    xmlDescriptorBase += "'";

    edit[1].append("0x1B");
    edit[4].append(xmlDescriptorBase.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeDescriptorBase")) {
    lua_getfield(L, 2,"descriptorBaseId");
    const char* descBaseId = luaL_checkstring(L, -1);

    edit[1].append("0x1C");
    edit[4].append(descBaseId);

    lua_pop(L, 1);

    /****TRANSITION****/
  } else if (!strcmp(command, "addTransition")) {
    lua_getfield(L, 2,"data");
    string xmlTransition = "'";
    xmlTransition += luaL_checkstring(L, -1);
    xmlTransition += "'";

    edit[1].append("0x1D");
    edit[4].append(xmlTransition.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeTransition")) {
    lua_getfield(L, 2,"transitionId");
    const char* transitionId = luaL_checkstring(L, -1);

    edit[1].append("0x1E");
    edit[4].append(transitionId);

    lua_pop(L, 1);

  } else if (!strcmp(command, "addTransitionBase")) {
    lua_getfield(L, 2,"data");
    string xmlTransitionBase = "'";
    xmlTransitionBase += luaL_checkstring(L, -1);
    xmlTransitionBase += "'";

    edit[1].append("0x1F");
    edit[4].append(xmlTransitionBase.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeTransitionBase")) {
    lua_getfield(L, 2,"transitionBaseId");
    const char* transBaseId = luaL_checkstring(L, -1);

    edit[1].append("0x20");
    edit[4].append(transBaseId);

    lua_pop(L, 1);

    /****IMPORT****/
  } else if (!strcmp(command, "addImportBase")) {
    lua_getfield(L, 2, "docBaseId");
    const char* docBaseId = luaL_checkstring(L, -1);

    lua_getfield(L, 2, "data");
    string xmlImportBase = "'";
    xmlImportBase += luaL_checkstring(L, -1);
    xmlImportBase += "'";

    edit[1].append("0x21");
    edit[4].append(docBaseId);
    edit[5].append(xmlImportBase.c_str());

    lua_pop(L, 2);

  } else if (!strcmp(command, "removeImportBase")) {
    lua_getfield(L, 2,"docBaseId");
    const char* docBaseId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"documentURI");
    const char* docURI = luaL_checkstring(L, -1);

    edit[1].append("0x22");
    edit[4].append(docBaseId);
    edit[5].append(docURI);

    lua_pop(L, 2);

  } else if (!strcmp(command, "addImportedDocumentBase")) {
    lua_getfield(L, 2,"data");
    string xmlImportedBase = "'";
    xmlImportedBase += luaL_checkstring(L, -1);
    xmlImportedBase += "'";

    edit[1].append("0x23");
    edit[4].append(xmlImportedBase.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeImportedDocumentBase")) {
    lua_getfield(L, 2,"importedDocumentBaseId");
    const char* importedDocBaseId = luaL_checkstring(L, -1);

    edit[1].append("0x24");
    edit[4].append(importedDocBaseId);

    lua_pop(L, 1);

  } else if (!strcmp(command, "addImportNCL")) {
    lua_getfield(L, 2,"data");
    string xmlImportNCL = "'";
    xmlImportNCL += luaL_checkstring(L, -1);
    xmlImportNCL += "'";

    edit[1].append("0x25");
    edit[4].append(xmlImportNCL.c_str());

    lua_pop(L, 1);

  } else if (!strcmp(command, "removeImportNCL")) {
    lua_getfield(L, 2,"documentURI");
    const char* documentURI = luaL_checkstring(L, -1);

    edit[1].append("0x26");
    edit[4].append(documentURI);

    lua_pop(L, 1);

    /****NODE****/
  } else if (!strcmp(command, "addNode")) {
    lua_getfield(L, 2,"compositeId");
    const char* compositeId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"data");
    string xmlNode = "'";
    xmlNode += luaL_checkstring(L, -1);
    xmlNode += "'";

    edit[1].append("0x27");
    edit[4].append(compositeId);
    edit[5].append(xmlNode.c_str());

    lua_pop(L, 2);

  } else if (!strcmp(command, "removeNode")) {
    lua_getfield(L, 2,"compositeId");
    const char* compositeId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"nodeId");
    const char* nodeId = luaL_checkstring(L, -1);

    edit[1].append("0x28");
    edit[4].append(compositeId);
    edit[5].append(nodeId);

    lua_pop(L, 2);

    /****INTERFACE****/
  } else if (!strcmp(command, "addInterface")) {
    lua_getfield(L, 2,"nodeId");
    const char* nodeId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"data");
    string xmlInterface = "'";
    xmlInterface += luaL_checkstring(L, -1);
    xmlInterface += "'";


    edit[1].append("0x29");
    edit[4].append(nodeId);
    edit[5].append(xmlInterface.c_str());

    lua_pop(L, 2);

  } else if (!strcmp(command, "removeInterface")) {
    lua_getfield(L, 2,"nodeId");
    const char* nodeId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"interfaceId");
    const char* interfaceId = luaL_checkstring(L, -1);

    edit[1].append("0x2A");
    edit[4].append(nodeId);
    edit[5].append(interfaceId);

    lua_pop(L, 2);

    /****LINK****/
  } else if (!strcmp(command, "addLink")) {
    lua_getfield(L, 2,"compositeId");
    const char* compositeId = luaL_checkstring(L, -1);

    lua_getfield(L, 2,"data");
    string xmlLink = "'";
    xmlLink += luaL_checkstring(L, -1);
    xmlLink += "'";

    edit[1].append("0x2B");
    edit[4].append(compositeId);
    edit[5].append(xmlLink.c_str());

    lua_pop(L, 2);

  } else if (!strcmp(command, "removeLink")) {
    lua_getfield(L, 2, "compositeId");
    const char* compositeId = luaL_checkstring(L, -1);

    lua_getfield(L, 2, "linkId");
    const char* linkId = luaL_checkstring(L, -1);

    edit[1].append("0x2C");
    edit[4].append(compositeId);
    edit[5].append(linkId);

    lua_pop(L, 2);

    /****ATTRIBUTION****/
  } else if (!strcmp(command, "setPropertyValue")) {
    lua_getfield(L, 2, "nodeId");
    const char* nodeId = luaL_checkstring(L, -1);

    lua_getfield(L, 2, "propertyId");
    const char* propertyId = luaL_checkstring(L, -1);

    lua_getfield(L, 2, "value");
    const char* value = luaL_checkstring(L, -1);

    edit[1].append("0x2D");
    edit[4].append(nodeId);
    edit[5].append(propertyId);
    edit[6].append(value);

    lua_pop(L, 3);
  }

  /****MOUNTING COMMAND****/
  //clog << "LuaEvent::editcommnadToString for:" << endl;
  for (int i = 1; i <= 7; i++){
    if (! edit[i].empty()) {

      if (i > 1) {
	editCommand.append(",");
      }
      editCommand.append(edit[i]);
      //clog << i << " = " << editCommand << endl;
    }
  }

  //clog << "LuaEvent::editToString editCommand:" << editCommand << endl;

  return editCommand;
}

void l_dump (lua_State* L, char* point)
{
	int i;
	int top = lua_gettop(L);
	printf("Ponto %s: ", point);
	for (i=1; i<=top; i++)
	{
		int t = lua_type(L, i);
		switch (t)
		{
			case LUA_TSTRING:
				printf("'%s'", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%g", lua_tonumber(L, i));
				break;
			default:
				printf("%s (%p)", lua_typename(L, t), lua_topointer(L, i));
				break;
		}
		printf(" ");
	}
	printf("\n");
}

void t_dump (lua_State* L, int idx)
{
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		printf("%s - %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

// MACROS ASSERTIVAS

/*
 * Construtor:
 * - cria o estado Lua
 * - cria superficie grafica
 * - inicializa estruturas internas de apoio
 * - inicializa bibliotecas disponiveis ao NCLua
 * - associa Player ao seu estado Lua
 */
LuaPlayer::LuaPlayer (string mrl) : Player(mrl), Thread()
{
	void* compObj;

    chdir(getPath(mrl).c_str());    // execucao a partir do diretorio fonte
    this->L = luaL_newstate();      // estado Lua

#if HAVE_DATAPROC
    epgProc = NULL;
#endif //HAVE_DATAPROC

#if HAVE_COMPSUPPORT
	this->im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
    this->surface = ((SurfaceCreator*)(cm->getObject("Surface")))(NULL, 0, 0);

#if HAVE_DATAPROC
    compObj = cm->getObject("EPGProcessor");
    if (compObj != NULL) {
    	this->epgProc = ((epgpCreator*)compObj)();

    } else {
    	clog << "LuaPlayer::LuaPlayer Warning! Can't create EPGProcessor: ";
    	clog << "symbol not found!" << endl;
    }
#endif //HAVE_DATAPROC

#else //HAVE_COMPSUPPORT

    this->im = InputManager::getInstance();

#ifndef _WIN32
	this->surface = new DFBSurface();
#else //_WIN32
	this->surface = new DXSurface();
#endif //_WIN32

#if HAVE_DATAPROC
	this->epgProc = EPGProcessor::getInstance();
#endif //HAVE_DATAPROC

#endif //HAVE_COMPSUPPORT

    this->surface->setCaps(this->surface->getCap("ALPHACHANNEL"));
	this->im->addApplicationInputEventListener(this);

    // ESCOPO
    this->currentScope = "";        // escopo global
    this->scopes = new map<string, struct scopeInfo*>;
    addScope("", TYPE_PRESENTATION, -1, -1);

    // a partir de um estado Lua eh possivel chegar ao seu Player
	lua_pushlightuserdata(this->L, this);         // [ LuaPlayer* ]
	lua_setfield(this->L, LUA_REGISTRYINDEX, LUAPLAYER_PLAYER);    // [ ]

    // INNICIALIZACAO DE BIBLIOTECAS DISPONIVEIS AO NCLUA

    // biblioteca padrao
	luaL_openlibs(this->L);

    // modulo 'event'
	lua_pushcfunction(this->L, luaopen_event);    // [ l_event ]

	if (lua_pcall(this->L, 0, 0, 0) != 0) {       // [ ]
		clog << "LUAPLAYER LuaPlayer 1 ERROR:: ";
		clog << lua_tostring(this->L, -1) << endl;
	}

	this->tcp_running = false;

    // modulo 'canvas'
    this->lock();
	lua_pushcfunction(this->L, luaopen_canvas);   // [ l_canvas ]
	if (lua_pcall(this->L, 0, 0, 0) != 0) {       // [ ]
		clog << "LUAPLAYER LuaPlayer 2 ERROR:: ";
		clog << lua_tostring(this->L, -1) << endl;
	}
    this->unlock();

    // estado inicial
	this->running = true;
	this->played = false;
	this->loaded = false;
	this->isHandler = false;
}

/*
 * DESTRUTOR:
 * - para de receber eventos de teclado
 * - fecha o estado Lua
 * - sinaliza fim da execucao
 */
LuaPlayer::~LuaPlayer ()
{
	this->running = false;
	this->lock();
	lua_pushcfunction(this->L, luaclose_event);   // [ l_event ]
	//lua_call(this->L, 0, 0);                      // [ ]
	lua_close(this->L);
	this->L = NULL;
    this->unlock();
}

// Retorna Player associado ao estado Lua.
LuaPlayer* LuaPlayer::getPlayer (lua_State* L)
{
	// [ ... ]
	lua_getfield(L, LUA_REGISTRYINDEX, LUAPLAYER_PLAYER);
                                                      // [ ... | LuaPlayer* ]
	LuaPlayer* player = (LuaPlayer*) lua_touserdata(L, -1);
	lua_pop(L, 1);                                    // [ ... ]
	return player;
}

void LuaPlayer::load ()
{
    if (this->loaded) return;
    this->loaded = true;

    // disponibiliza a superficie do NCLua atraves do modulo canvas
	ISurface* s = this->getSurface();
	if (s) {
		lua_createcanvas(this->L, s, 0);              // [ canvas ]
		lua_setglobal(this->L, "canvas");             // [ ]
	}

    // carrega o NCLua e checa se houve erros
	if( luaL_loadfile(this->L, this->mrl.c_str()) ) { // [ func ]
		this->notifyListeners(PL_NOTIFY_ABORT, "");
		lua_error(this->L);
		return;
	}
    if( lua_pcall(this->L, 0, 0, 0) != 0 ) { 	      // [ ]
    	clog << "LUAPLAYER load ERROR:: " << lua_tostring(this->L, -1) << endl;
    }
}

void LuaPlayer::run ()
{
    this->lock();
    this->load();
    this->unlock();
}

// METODOS DE COMUNICACAO COM O FORMATADOR: play, stop, abort, pause e resume

void LuaPlayer::post (string action)
{
    struct scopeInfo* scope = (*this->scopes)[this->currentScope];

    // PRESENTATION
	if (scope->type == TYPE_PRESENTATION)
    {
        map<string,string> evt;
            evt["class"]    = "ncl";
            evt["type"]     = "presentation";
            evt["action"]   = action;
            if (scope->initTime == -1)
                evt["label"] = currentScope;
            else {
                evt["begin"] = scope->initTime; //TODO: tostring?
                evt["end"]   = scope->endTime; //TODO: tostring?
            }
        ext_postHash(this->L, evt);
	}
    else
    {
        // TODO: attribution, ...
	}
}

void LuaPlayer::play()
{
	this->lock();
	Player::play();
    this->load();
	this->post("start");
	this->unlock();
}

void LuaPlayer::stop ()
{
	this->lock();
    this->post("stop");

    if (this->currentScope == "")
    {
	    this->im->removeApplicationInputEventListener(this);
		this->forcedNaturalEnd = true;
		this->played = true;
    }

    Player::stop();
	this->unlock();
}

void LuaPlayer::pause()
{
    this->lock();
    this->post("pause");
    Player::pause();
	this->unlock();
}

void LuaPlayer::resume ()
{
    this->lock();
    Player::resume();
    this->post("resume");
	this->unlock();
}

void LuaPlayer::abort()
{
    this->lock();
    this->post("abort");
    this->played = true;
    this->unlock();
    Player::abort();
}

void LuaPlayer::unprotectedSetPropertyValue(string name, string value) {
	Player::setPropertyValue(name, value);
}

void LuaPlayer::setPropertyValue(string name, string value) {
	clog << "LuaPlayer::setPropertyValue ";
    clog << "set property '" << name << "' = '" << value << "'" << endl;

	this->lock();
	Player::setPropertyValue(name, value);

    map<string,string> evt;
        evt["class"]  = "ncl";
        evt["type"]   = "attribution";
        evt["action"] = "start";
        evt["name"]   = name;
        evt["value"]  = value;

    ext_postHash(this->L, evt);
	this->unlock();
}

// TECLADO

bool LuaPlayer::userEventReceived (IInputEvent* evt)
{
	this->lock();

	if (evt->isUserClass()) {
//clog << ">recv " << L << " ref " << evt->getType() << endl;
        lua_State* srcL = (lua_State*) evt->getData();
        if (srcL == L) {
//clog << "<recv " << L << endl;
		    ext_postRef(this->L, evt->getType());
        }

	} else if (evt->isKeyType() && this->isHandler) {
		string key_str = CodeMap::getInstance()->getValue(evt->getKeyCode());
        map<string,string> lua_evt;
        lua_evt["class"] = "key";
        lua_evt["type"]  = evt->isPressedType() ? "press" : "release";
        lua_evt["key"]   = key_str;
        ext_postHash(this->L, lua_evt);
	}

    this->unlock();
	return true;
}

void LuaPlayer::refreshContent() {
	if (notifyContentUpdate) {
		notifyListeners(PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE);
	}
}

void LuaPlayer::addAsSIListener (unsigned char type) {
#if HAVE_DATAPROC
	if (epgProc) {
		this->epgProc->addEPGListener(this, "all", type);
	}
#endif //HAVE_DATAPROC
}

//TODO: generalize to pushSIEvent
void LuaPlayer::pushSIEvent (map<string, struct SIField> event, unsigned char type)
{
#if HAVE_DATAPROC
	struct Field field;
	//map<string, struct SIField> data;
	map<string, struct Field> evt;

	clog << "LuaPlayer::pushSIEvent event.size = '" << event.size() << "'";
	clog << endl;

	if (!event.empty()) {
		clog << "LuaPlayer::pushSIEvent" << endl;
		field.str    = "si";
		evt["class"] = field;

		if (type == IEPGListener::EPG_LISTENER) {
			field.str   = "epg";

		} else if (type == IEPGListener::SI_LISTENER) {
			field.str   = "services";

		} else if (type == IEPGListener::MOSAIC_LISTENER) {
			field.str   = "mosaic";

		} else if (type == IEPGListener::TIME_LISTENER) {
			field.str   = "time";
		}

		evt["type"] = field;
		field.str   = "";
		field.table = event;
		evt["data"] = field;

		ext_postHashRec(this->L, evt, true);
	}
#endif //HAVE_DATAPROC
}

/*
string LuaPlayer::getPropertyValue (string name)
{
	this->lock();
	CHECKRET("getPropertyValue()", "")
	clog << "[LUA] getPropertyValue('" << name.c_str() << "')";

	lua_getglobal(this->L, name.c_str());         // [ var ]
	string ret = luaL_optstring(L, -1, "");
	lua_pop(this->L, 1);                          // [ ]

	clog << " -> '" << ret.c_str() << "'" << endl;
	this->unlock();
	return ret;
}
*/

// MÉTODOS AUXILIARES

bool LuaPlayer::hasPresented() {
    //clog << "LuaPlayer::hasPresented '" << played << "'" << endl;
	return played;
}

bool LuaPlayer::setKeyHandler(bool isHandler) {
    this->isHandler = isHandler;
    return this->isHandler;
}

void LuaPlayer::setScope(string scopeId, short type,
                         double begin, double end)
{
	clog << "LuaPlayer::setScope '" << scopeId << "' type '" << type;
	clog << "' begin '" << begin << "' end '" << end << endl;
	addScope(scopeId, type, begin, end);
}

void LuaPlayer::addScope(string scopeId, short type, double begin, double end)
{
	struct scopeInfo* newScope;

	if (this->scopes->count(scopeId) != 0)
        return;

	newScope = new struct scopeInfo;
	(*this->scopes)[scopeId] = newScope;
	newScope->type     = type;
	newScope->scopeId  = scopeId;
	newScope->initTime = begin;
	newScope->endTime  = end;
}

void LuaPlayer::setCurrentScope(string scopeId) {
	clog << "LuaPlayer::setCurrentScope '" << scopeId << "'" << endl;
	this->currentScope = scopeId;
}

} } } } } }

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createLuaPlayer(const char* mrl, bool hasVisual) {

	return new ::br::pucrio::telemidia::ginga::core::player::LuaPlayer(
			(string)mrl);
}

extern "C" void destroyLuaPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
