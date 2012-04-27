/*
 * LuaAdapter.cpp
 *
 *  Created on: 01/22/12
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
 
#include "./src/Processors/LuaAdapter.hpp"

using namespace std;
using namespace Processors;

/* helper functions */
static void _LuaStackSchemaPush(lua_State* pLuaState);
static void _LuaStackTypePush(lua_State* pLuaState, const string& rTypeName);
static void _LuaStackAttribPush(lua_State* pLuaState, const string& rAttribName);
static void _LuaStackNewTypePush(lua_State* pLuaState, const string& rTypeName);

/* Class method definitions */
LuaContent::LuaContent(lua_State* pLuaState, const std::string& rParentName) 
	: m_pLuaState(pLuaState), m_parentName(rParentName)
{ }

/* virtual */
LuaContent::~LuaContent()
{ }

LuaAttribute::LuaAttribute(lua_State* pLuaState, const string& rAttribName, const string& rParentName)
	: LuaContent(pLuaState, rParentName), m_attribName(rAttribName)
{ }

/* virtual */
LuaAttribute::~LuaAttribute() 
{ }

void
LuaAttribute::SetDefault(const std::string& rValue) {
	_LuaStackSchemaPush(m_pLuaState);
	_LuaStackTypePush(m_pLuaState, m_parentName);
	_LuaStackAttribPush(m_pLuaState, m_attribName);
	lua_pushstring(m_pLuaState, rValue.c_str());
	lua_setfield(m_pLuaState, -2, "default");
	lua_settop(m_pLuaState, 0);
}

LuaType::LuaType(lua_State* pLuaState, const std::string& rTypeName)
	: m_pLuaState(pLuaState), m_typeName(rTypeName)
{ }

/* virtual */
LuaType::~LuaType() 
{ }

LuaAttribute 
LuaType::AddAttribute(const string& rName, const XSD::Types::BaseType& rType) {
	_LuaStackSchemaPush(m_pLuaState);
	_LuaStackTypePush(m_pLuaState, m_typeName);
	lua_getfield(m_pLuaState, -1, "attributes");
	lua_newtable(m_pLuaState);
	lua_setfield(m_pLuaState, -2, rName.c_str());
	lua_getfield(m_pLuaState, -1, rName.c_str());
	lua_pushstring(m_pLuaState, rType.Name());
	lua_setfield(m_pLuaState, -2, "type");
	lua_settop(m_pLuaState, 0);
	return GetAttribute(rName);
}

LuaAttribute 
LuaType::GetAttribute(const std::string& rName) {
	return LuaAttribute(m_pLuaState, rName, m_typeName);
}

LuaContent
LuaType::AddContent(const XSD::Types::BaseType& rType) {
	_LuaStackSchemaPush(m_pLuaState);
	_LuaStackTypePush(m_pLuaState, m_typeName);
	lua_getfield(m_pLuaState, -1, "content");
	lua_pushstring(m_pLuaState, rType.Name());
	lua_setfield(m_pLuaState, -2, "type");
	lua_settop(m_pLuaState, 0);
	return GetContent();
}

LuaContent 
LuaType::GetContent() {
	return LuaContent(m_pLuaState, m_typeName);
}

void
LuaType::AddDependent(const LuaType& rLuaType) {
	_LuaStackSchemaPush(m_pLuaState);
	_LuaStackTypePush(m_pLuaState, m_typeName);
	lua_getfield(m_pLuaState, -1, "dependents");
	lua_getfield(m_pLuaState, -3, rLuaType.m_typeName.c_str());
	if (lua_isnil(m_pLuaState, -1)) {
		_LuaStackNewTypePush(m_pLuaState, rLuaType.m_typeName);
		lua_settop(m_pLuaState, 0);
		AddDependent(rLuaType);
		return;
	}
	lua_setfield(m_pLuaState, -2, rLuaType.m_typeName.c_str());
	lua_settop(m_pLuaState, 0);
}

LuaSchema::LuaSchema(lua_State* pLuaState)
	: m_pLuaState(pLuaState) {
	/* test if global table has been made & make it if not */
	_LuaStackSchemaPush(m_pLuaState);
	if (lua_isnil(m_pLuaState, -1)) {
		lua_newtable(m_pLuaState);
		lua_pushstring(m_pLuaState, "");
		lua_setfield(m_pLuaState, -2, "root");
		lua_newtable(m_pLuaState);
		lua_setfield(m_pLuaState, -2, "types");
		lua_setglobal(m_pLuaState, "schema");
	}
	lua_settop(m_pLuaState, 0);
}

/* virtual */
LuaSchema::~LuaSchema()
{ }

void
LuaSchema::SetName(const std::string& rName) {
	_LuaStackSchemaPush(m_pLuaState);
	lua_pushstring(m_pLuaState, rName.c_str());
	lua_setfield(m_pLuaState, -2, "root");
	lua_settop(m_pLuaState, 0);
}

LuaType
LuaSchema::AddType(const std::string& rTypeName) {
	_LuaStackSchemaPush(m_pLuaState);
	_LuaStackTypePush(m_pLuaState, rTypeName);
	/* test if type already exists */
	if (lua_isnil(m_pLuaState, -1)) {
		_LuaStackNewTypePush(m_pLuaState, rTypeName);
	}
	lua_settop(m_pLuaState, 0);
	return GetType(rTypeName);
}

LuaType
LuaSchema::GetType(const std::string& rTypeName) {
	return LuaType(m_pLuaState, rTypeName);
}

LuaAdapter::LuaAdapter(lua_State* pLuaState)
	: m_pLuaState(pLuaState)
{ }

/* virtual */
LuaAdapter::~LuaAdapter()
{ }

LuaSchema
LuaAdapter::Schema() {
	return LuaSchema(m_pLuaState);
}
		
static void _LuaStackSchemaPush(lua_State* pLuaState) {
	lua_getglobal(pLuaState, "schema");
}

static void _LuaStackTypePush(lua_State* pLuaState, const string& rTypeName) {
	lua_getfield(pLuaState, -1, "types");
	lua_getfield(pLuaState, -1, rTypeName.c_str());
}

static void _LuaStackAttribPush(lua_State* pLuaState, const string& rAttribName) {
	lua_getfield(pLuaState, -1, "attributes");
	lua_getfield(pLuaState, -1, rAttribName.c_str());
}

static void _LuaStackNewTypePush(lua_State* pLuaState, const string& rTypeName) {
	_LuaStackSchemaPush(pLuaState);
	lua_getfield(pLuaState, -1, "types");
	lua_newtable(pLuaState);
	lua_setfield(pLuaState, -2, rTypeName.c_str());
	lua_getfield(pLuaState, -1, rTypeName.c_str());
	lua_newtable(pLuaState);
	lua_setfield(pLuaState, -2, "attributes");
	lua_newtable(pLuaState);
	lua_setfield(pLuaState, -2, "content");
	lua_newtable(pLuaState);
	lua_setfield(pLuaState, -2, "dependents");
	lua_pushstring(pLuaState, rTypeName.c_str());
	lua_setfield(pLuaState, -2, "name");
}
