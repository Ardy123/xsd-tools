/*
 * main.cpp
 *
 *  Created on: May 18, 2011
 *      Author: Ardavon Falls
 *   Copyright: (c)2011 Ardavon Falls
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

#include <exception>
#include <string>
#include <sstream>
#include <iostream>
#include <lua5.1/lua.hpp>
#include "./src/luaScriptAdapter.hpp"
#include "./src/resource.hpp"
#include "./src/XSDParser/Parser.hpp"
#include "./src/Processors/LuaProcessor.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"

using namespace std;

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		cout << "xsdb (c) 2012 Ardavon Falls" << endl;
		cout << "Syntax xsdb <template> <input xsd file>" << endl;
	} else {
		XSD::Parser 				parser;
		XSD::Elements::Schema* 		pDocRoot = NULL;
		const std::string			tmpltName(argv[1]);
		const std::string			xsdName(argv[2]);
		Core::Resource				resource;
		Core::LuaScriptAdapter		luaScriptAdapter(luaL_newstate());
		Processors::LuaProcessor	luaPrcssr(luaScriptAdapter.LuaState());
		size_t 						nginScriptSz = 0;
		const uint8_t* 				pNginScript = resource.GetEngineScript(&nginScriptSz);
		try {
			/* init lua */
			luaScriptAdapter.Open();
			/* parse xsd document */
			pDocRoot = parser.Parse(xsdName);
			pDocRoot->ParseElement(luaPrcssr);
			/* execute lua template processing engine */
			luaScriptAdapter.SetSchemaName(xsdName);
			luaScriptAdapter.Load(pNginScript, nginScriptSz);
			luaScriptAdapter.Execute(resource.GetTemplatePath(tmpltName));
		} catch (XSD::XMLException& e) {
			cerr << "XSD Parsing Error: " << e.what() << endl;
		} catch (Core::LuaException& e) {
			cerr << "Lua Error: " << e.what() << endl;
		} catch (Core::ResourceException& e) {
			cerr << e.what() << endl;
		} catch (std::exception& e) {
			cerr << "General error: " << e.what() << endl;
		}
		/* close out lua */
		luaScriptAdapter.Close();
		/* release root schema node */
		delete pDocRoot;
	}
}
