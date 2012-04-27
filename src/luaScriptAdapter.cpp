/*
 * luaScriptAdapter.cpp
 *
 *  Created on: 01/27/12
 *      Author: Ardavon Falls
 *   Copyright: (c)2012 Ardavon Falls
 *
 *  This file is part of xsd-tools.
 *
 *  xsd-tools is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  xsd-tools is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <string>
#include <sstream>
#include <exception>
#include <lua5.1/lua.hpp>
#include "./src/luaScriptAdapter.hpp"
#include "./src/util.hpp"

using namespace std;
using namespace Core;

LuaException::LuaException(const std::string& rMsg) {
	stringstream msgStrm(ios_base::out|ios_base::in);
	msgStrm << rMsg << endl;
	m_errorMsg = msgStrm.str();
}

LuaException::LuaException(const std::string& rMsg, int errno) { 
	stringstream msgStrm(ios_base::out|ios_base::in);
	msgStrm << rMsg << "(" << errno << ")" << endl;
	m_errorMsg = msgStrm.str();
}

/* virtual */
LuaException::~LuaException() throw() 
{ }

/* virtual */ const char*
LuaException::what() const throw() {
	return m_errorMsg.c_str();
}

LuaScriptAdapter::LuaScriptAdapter(lua_State* pLuaState)
	: m_pLuaState(pLuaState)
{ }

/* virtual */
LuaScriptAdapter::~LuaScriptAdapter()
{ }

void
LuaScriptAdapter::Open() throw() {
	/* load std lua libs */
	luaL_openlibs(m_pLuaState);
	/* add custom helper hash function */
	lua_pushcfunction(m_pLuaState, LuaScriptAdapter::luaSDBMHash);
	lua_setglobal(m_pLuaState, "sdbm_hash");
}

void
LuaScriptAdapter::Load(const uint8_t* pBuf, size_t bufSz) throw(LuaException) {
	int err = luaL_loadbuffer(m_pLuaState, (const char*)pBuf, bufSz, "TemplateEngine");
	switch (err) {
	case 0: break; /* no error */
	case LUA_ERRSYNTAX:
		throw LuaException("Syntax Error Loading TemplateEngine", err);
		break;
	case LUA_ERRMEM:
		throw LuaException("Memory Error Loading TemplateEngine", err);
		break;
	default: 
		throw LuaException("Lua Error Loading TemplateEngine", err);
		break;
	}
}

void
LuaScriptAdapter::Execute(const std::string& templateName) throw(LuaException) {
	int err = 0;
	/* force a run of the script to load global symbols */
	if (0 != (err = lua_pcall(m_pLuaState, 0, 0, 0)))
		goto LuaScriptAdapterExecute_error;
	/* execute main function */
	lua_getglobal(m_pLuaState, "main");
	lua_pushstring(m_pLuaState, templateName.c_str());
	if (0 != (err = lua_pcall(m_pLuaState, 1, 0, 0)))
		goto LuaScriptAdapterExecute_error;
	return;
LuaScriptAdapterExecute_error:
	switch(err) {
	case 0: break; /* no error */
	case LUA_ERRMEM:
		throw LuaException("Memory Error Executing TemplateEngine", err);
		break;
	case LUA_ERRERR:
		throw LuaException("Error running lua error handler", err);
		break;
	default:
		throw LuaException(lua_tostring(m_pLuaState, -1), err);
		break;
	}
}

void
LuaScriptAdapter::Close() throw() {
	lua_close(m_pLuaState);
}

lua_State*
LuaScriptAdapter::LuaState() throw() {
	return m_pLuaState;
}

/* static */ int
LuaScriptAdapter::luaSDBMHash(lua_State* pLuaState) {
	const char* pStr = lua_tostring(pLuaState, -1);
	lua_pushnumber(pLuaState, Util::SDBMHash(std::string(pStr)));
	return 1;
}
