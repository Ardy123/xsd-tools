/*
 * LuaAdapter.hpp
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

#ifndef LUAADAPTER_HPP_
#define LUAADAPTER_HPP_
#include <string>
#include <lua5.1/lua.hpp>
#include "./src/XSDParser/Types.hpp"

namespace Processors {
	/* forward declare classes */
	class LuaAdapter;
	class LuaType;
	class LuaAttribute;
	class LuaContent;
	/* helper classes to manipulate Lua objects */
	class LuaSchema {
		friend class LuaAdapter;
	public:
		virtual ~LuaSchema();
		void SetName(const std::string& rName);
		LuaType AddType(const std::string& rTypeName);
		LuaType GetType(const std::string& rTypeName);
	protected:
		LuaSchema(lua_State* pLuaState);
		lua_State*	m_pLuaState;
	};
	class LuaType {
		friend class LuaSchema;
	public:
		virtual ~LuaType();
		LuaAttribute AddAttribute(const std::string& rName, const XSD::Types::BaseType& rType);
		LuaAttribute GetAttribute(const std::string& rName);
		LuaContent AddContent(const XSD::Types::BaseType& rType);
		LuaContent GetContent();
		void AddDependent(const LuaType& rLuaType);
	protected:
		LuaType(lua_State* pLuaState, const std::string& rTypeName);
		lua_State* 			m_pLuaState;
		const std::string	m_typeName;
	};
	class LuaContent {
		friend class LuaType;
	public:
		virtual ~LuaContent();
	protected:
		lua_State* 			m_pLuaState;
		const std::string 	m_parentName;
		LuaContent(lua_State* pLuaState, const std::string& rParentName);
	};
	class LuaAttribute : public LuaContent {
		friend class LuaType;
	public:
		virtual ~LuaAttribute();
		void SetDefault(const std::string& rValue);
	protected:
		LuaAttribute(lua_State* pLuaState, const std::string& rAttribName, const std::string& rParentName);
		const std::string	m_attribName;
	};
	/* core class */
	class LuaAdapter {
	public:
		LuaAdapter(lua_State* pLuaState);
		virtual ~LuaAdapter();
		LuaSchema Schema();
	private:
		lua_State*	m_pLuaState;
		LuaAdapter(const LuaAdapter&);
	};
}
#endif /* LUAADAPTER_HPP_ */
