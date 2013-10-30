/*
 * Enumeration.cpp
 *
 *  Created on: Aug 3, 2011
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

#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <memory>
#include <string.h>
#include <string>
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Enumeration.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

Enumeration::Enumeration(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

Enumeration::Enumeration(const Enumeration& cpy)
	: Node(cpy)
{ }

void
Enumeration::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* no children allowed */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Annotation))
				pNode->ParseChildren(rProcessor);
			else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			break;
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Enumeration::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	rProcessor.ProcessEnumeration(this);
}

bool
Enumeration::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	return true;
}

std::string
Enumeration::Value() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("value"));
}

bool
Enumeration::HasValue() const {
	return Node::HasAttribute("value");
}