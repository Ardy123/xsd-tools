/*
 * Parser.cpp
 *
 *  Created on: Jul 3, 2011
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

#include "./src/XSDParser/Parser.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#define PROTO_FILE	"file://"


using namespace XSD;

Parser::Parser()
  : m_typesDb(), m_docLst()
{ }

/* virtual */
Parser::~Parser() {
  for (XmlDocList::iterator i = m_docLst.begin(); i != m_docLst.end(); ++i) {
	delete *i;
  }
}

Elements::Schema*
Parser::Parse(const char* pUri) const throw(XMLException) {
	m_docLst.push_back(new TiXmlDocument());
	TiXmlDocument * pDoc = m_docLst.back();
	pDoc->SetTabSize(4);
	/* parse protocol portion */
	if (pUri == strstr(pUri, PROTO_FILE)) {
		const char* pPath = (pUri + sizeof(PROTO_FILE) - 1);
		if( !pDoc->LoadFile(pPath, TIXML_ENCODING_UTF8) ) {
			throw XMLException(*pDoc);
		}
		return new Elements::Schema(*pDoc->RootElement(), *this, std::string(pUri));
	} else if(NULL == strstr(pUri, "://")) {
		/* if no protocol specified then assume it is a local file */
		if( !pDoc->LoadFile((pUri), TIXML_ENCODING_UTF8) ) {
			throw XMLException(*pDoc);
		}
		return new Elements::Schema(*pDoc->RootElement(), *this, std::string(pUri));
	}
	return NULL;
}

Elements::Schema*
Parser::Parse(const std::string& rFileName) const throw(XMLException) {
	return Parse(rFileName.c_str());
}

const Types::TypesDB&
Parser::QueryTypesDb() const throw() {
	return m_typesDb;
}

