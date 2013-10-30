/*
 * Document.cpp
 *
 *  Created on: Jun 26, 2011
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
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/Include.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

Schema::Schema(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser, const std::string& name)
	: Node(elm, rRoot, rParser), m_rDocument(*const_cast<TiXmlDocument*>(elm.ToDocument())), m_documentURI(name)
{ }

Schema::Schema(const TiXmlElement& elm, const Parser& rParser, const std::string& name )
	: Node(elm, *this, rParser), m_rDocument(*const_cast<TiXmlDocument*>(elm.ToDocument())), m_documentURI(name)
{ }

Schema::Schema(const Schema& rDoc)
	: Node(rDoc.m_rXmlElm, rDoc.m_rDocRoot, rDoc.m_rParser), m_rDocument(rDoc.m_rDocument), m_documentURI(rDoc.m_documentURI)
{ }

/* virtual */
Schema::~Schema() {
	delete &m_rDocument;
}

void
Schema::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Element) ||
				XSD_ISELEMENT(pNode.get(), Annotation) ||
				XSD_ISELEMENT(pNode.get(), Include)) {
				pNode->ParseElement(rProcessor);
			}
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Schema::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	rProcessor.ProcessSchema(this);
}

std::string
Schema::Name() const throw(XMLException) {
	return _extractName(m_documentURI);
}

const std::string&
Schema::URI() const throw(XMLException) {
	return m_documentURI;
}

bool
Schema::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	return false; /* TODO */
}

bool
Schema::isRootSchema() const {
	return (m_rDocRoot == *this);
}

/* static */ std::string
Schema::_extractName(const std::string& uri) {
	const int queryNdx	= uri.find("?");
	const int resExtNdx	= uri.rfind(".", queryNdx);
	const int resNdx	= uri.rfind("/", queryNdx);
	return uri.substr(resNdx + 1, (std::string::npos != resExtNdx) ? (resExtNdx - resNdx) - 1 : resExtNdx);
}