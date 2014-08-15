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
	std::string uri(pUri);
	return Parse(uri);
}

Elements::Schema*
Parser::Parse(const std::string& rUri) const throw(XMLException) {
	/* search if the document has already been parsed */
	if (HasDocument(rUri)) {
		return new Elements::Schema(*(GetDocument(rUri)->RootElement()), *this, rUri);
	} else {
		m_docLst.push_back(new DocumentRecord(new TiXmlDocument(), rUri));
		TiXmlDocument * pDoc = m_docLst.back()->m_pDocument;
		pDoc->SetTabSize(4);
		/* parse protocol portion */
		if (0 == rUri.find(PROTO_FILE)) {
			std::string path = rUri.substr(sizeof(PROTO_FILE) - 1);
			if( !pDoc->LoadFile(path, TIXML_ENCODING_UTF8) ) {
				throw XMLException(*pDoc);
			}
			return new Elements::Schema(*pDoc->RootElement(), *this, rUri);
		} else if (std::string::npos == rUri.find("://")) {
			/* if no protocol specified then assume it is a local file */
			if( !pDoc->LoadFile(rUri, TIXML_ENCODING_UTF8) ) {
				throw XMLException(*pDoc);
			}
			return new Elements::Schema(*pDoc->RootElement(), *this, rUri);
		}
	}
	return NULL;
}

const Types::TypesDB&
Parser::QueryTypesDb() const throw() {
	return m_typesDb;
}

bool 
Parser::HasDocument(const TiXmlDocument& document) const {
	for (XmlDocList::iterator i = m_docLst.begin(); i != m_docLst.end(); ++i) {
		if ((*i)->m_pDocument == &document) 
			return true;
	}
	return false;
}

bool 
Parser::HasDocument(const std::string& rUri) const {
	for (XmlDocList::iterator i = m_docLst.begin(); i != m_docLst.end(); ++i) {
		if ((*i)->m_uri == rUri) 
			return true;
	}
	return false;
}

std::string 
Parser::GetUri(const TiXmlDocument& document) const {
	for (XmlDocList::iterator i = m_docLst.begin(); i != m_docLst.end(); ++i) {
		if ((*i)->m_pDocument == &document) 
			return (*i)->m_uri;
	}
	return std::string("unknown");
}

const TiXmlDocument *
Parser::GetDocument(const std::string& rUri) const {
	for (XmlDocList::iterator i = m_docLst.begin(); i != m_docLst.end(); ++i) {
		if ((*i)->m_uri == rUri) 
			return (*i)->m_pDocument;
	}
	return NULL;
}

bool 
Parser::isRootDocument(const TiXmlDocument& document) const {
	return ((*(m_docLst.begin()))->m_pDocument == &document);
}

