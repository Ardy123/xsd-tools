/*
 * Include.cpp
 *
 *  Created on: Aug 10, 2011
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

#include <memory>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include "./src/XSDParser/Elements/Include.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Parser.hpp"

using namespace XSD;
using namespace XSD::Elements;

Include::Include(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser), m_pSchema(NULL)
{ }

Include::Include(const Include& rCpy)
	: Node(rCpy), m_pSchema(rCpy.m_pSchema)
{ }

/* virtual */
Include::~Include() {
	delete m_pSchema;
}

void
Include::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* does nothing, doesn't have child elements */
}

void
Include::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	rProcessor.ProcessInclude(this);
}

bool
Include::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	return false; /* TODO */
}

const Schema*
Include::QuerySchema() const throw(XMLException) {
	if (NULL == m_pSchema) {
		m_pSchema = m_rParser.Parse(_schemaURI());
	}
	return m_pSchema;
}

bool
Include::HasSchema() const {
	return Node::HasAttribute("schemaLocation");
}

std::string
Include::_schemaURI() const throw(XMLException) {
	std::string uri(Node::GetAttribute<const char*>("schemaLocation"));
	/* handle file URIs differently */
	if (_isFileURI(uri)) {
		std::string retStr("file://");
		boost::filesystem::path path(_extractURIPath(uri));
		if (path.has_root_path()) {
			retStr += (path.string() + _extractQuery(uri));
			return retStr;
		} else {
			boost::filesystem::path schemaPath = (boost::filesystem::absolute(_extractURIPath(m_rDocRoot.URI()))).branch_path();
			(schemaPath /= _extractURIPath(uri)).normalize();
			retStr += schemaPath.string();
			return retStr;
		}
	} else {
		return uri;
	}
}

/* static */std::string
Include::_extractURIPath(const std::string& uri) {
	/* strip off query section */
	std::string noQuery		= uri.substr(0, uri.find("?"));
	/* strip off protocol section */
	const int	protoNdx	= noQuery.find("://");
	std::string noProto		= noQuery.substr((std::string::npos == protoNdx) ? 0 : protoNdx + 3);
	return noProto;
}

/* static */ bool
Include::_isFileURI(const std::string& uri) {
	/* strip off query section */
	std::string noQuery	= uri.substr(0, uri.find("?"));
	bool hasFileProto	= (std::string::npos != noQuery.find("file://"));
	bool hasProto		= (std::string::npos != noQuery.find("://"));
	return (hasFileProto || !hasProto);
}

/* static */ std::string
Include::_extractQuery(const std::string& uri) {
	const int queryNdx = uri.find("?");
	if (std::string::npos == queryNdx)
		return std::string("");
	else
		return uri.substr(queryNdx);
}
