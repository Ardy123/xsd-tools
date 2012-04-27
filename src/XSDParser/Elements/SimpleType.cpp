/*
 * SimpleType.cpp
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
#define TIXML_USE_STL
#include <tinyxml.h>
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/Union.hpp"
#include "./src/XSDParser/Elements/Restriction.hpp"
#include "./src/XSDParser/Elements/List.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

SimpleType::SimpleType(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

SimpleType::SimpleType(const SimpleType& rType)
	: Node(rType)
{ }

void
SimpleType::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Restriction) ||
				XSD_ISELEMENT(pNode.get(), List) ||
				XSD_ISELEMENT(pNode.get(), Annotation) ||
				XSD_ISELEMENT(pNode.get(), Union)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
SimpleType::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	if (HasName() && !Node::IsRootNode())
		throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	rProcessor.ProcessSimpleType(this);
}

bool
SimpleType::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	/* break down simpleType to base types */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Restriction) ||
				XSD_ISELEMENT(pNode.get(), List) ||
				XSD_ISELEMENT(pNode.get(), Union)) {
				if (pNode->isTypeRelated(pType))
					return true;
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
	return false;
}

std::string
SimpleType::Name() const throw(XMLException) {
	return std::string(this->GetAttribute<const char*>("name"));
}

bool
SimpleType::HasName() const {
	return Node::HasAttribute("name");
}

bool
SimpleType::HasRestriction() const {
	return Node::HasContent(Restriction::XSDTag());
}

bool
SimpleType::HasUnion() const {
	return Node::HasContent(Union::XSDTag());
}

bool
SimpleType::HasList() const {
	return Node::HasContent(List::XSDTag());
}
