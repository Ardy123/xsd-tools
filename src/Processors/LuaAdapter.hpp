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
 *  along with xsd-tools.  If not, see <http://www.gnu.org/licenses/>.
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
	class LuaSchema;
	/* The lua adapter classes enable the stack of recursive processors to maintain an object stack that represents
	   the output schema. Each time the processor adds a new item (element type def or attribute) on to the schema 
	   table, it creates a new processor which intern references one of these objects */
	/* core class */
	class LuaAdapter {
	public:
		LuaAdapter(lua_State * pLuaState);
		virtual ~LuaAdapter();
		LuaSchema * Schema();
	protected:
		lua_State * _getLuaState();
		void _setLuaState(lua_State * pLuaState);
	private:
		lua_State *	m_pLuaState;
		LuaAdapter(const LuaAdapter&);
	};
	/* lua content class */
	class LuaContent : public LuaAdapter {
		friend class LuaType;
	public:
		virtual ~LuaContent();
		LuaType * Type(const std::string& rTypeName);
	protected:
		LuaContent();
		LuaContent(lua_State* pLuaState);
	};
	/* lua schema class */
	class LuaSchema : public LuaContent {
		friend class LuaAdapter;
	public:
		virtual ~LuaSchema();
	protected:
		LuaSchema(lua_State* pLuaState);
	};

	/* lua type class */
	class LuaType : public LuaAdapter {
		friend class LuaSchema;
		friend class LuaContent;
		friend class LuaAttribute;
	public:
		virtual ~LuaType();
		LuaAttribute * Attribute(	const std::string& rName, 
									const XSD::Types::BaseType& rType,
									const std::string * pDefault,
									const std::string * pFixed,
									const std::string * pUse);
		LuaContent * Content();
	protected:
		LuaType(lua_State * pLuaState, const std::string& rTypeName);
	};
	/* lua attribute class */
	class LuaAttribute : public LuaAdapter {
		friend class LuaType;
	public:
		virtual ~LuaAttribute();
	protected:
		LuaAttribute(	lua_State * pLuaState,
						const std::string& rAttribName,
						const XSD::Types::BaseType& rType,
						const std::string * pDefault,
						const std::string * pFixed,
						const std::string * pUse
					);
	};
}
#endif /* LUAADAPTER_HPP_ */
