/*
 * Any.cpp
 *
 *  Created on: Jul 10, 2011
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
#include "./src/XSDParser/Elements/Any.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

Any::Any(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

Any::Any(const Any& cpy)
	: Node(cpy)
{ }

void
Any::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Annotation))
				pNode->ParseElement(rProcessor);
			else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			break;
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Any::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	/* verify that 'maxOccurs' is not negative unless its -1 (unbounded) */
	if (-1 > MaxOccurs())
		throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	/* verify that 'minOccurs' is non-negative */
	if (0 > MinOccurs())
		throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	/* process element */
	rProcessor.ProcessAny(this);
}

Types::BaseType * 
Any::GetParentType() const throw(XMLException) {
	std::auto_ptr<Node> pParent(Node::Parent());
	return pParent->GetParentType();
}

int
Any::MaxOccurs() const {
	if (HasMaxOccurs()) {
		if (strcmp(Node::GetAttribute<const char*>("maxOccurs"), "unbounded"))
			return Node::GetAttribute<int>("maxOccurs");
		else
			return -1;
	}
	return 1;
}

int
Any::MinOccurs() const {
	if (HasMinOccurs()) {
		return Node::GetAttribute<int>("minOccurs");
	}
	return 1;
}

std::string
Any::Namespace() const {
	if (HasNamespace()) {
		return std::string(Node::GetAttribute<const char*>("namespace"));
	}
	return std::string("##any");
}

Any::ContentValidation
Any::ProcessContents() const {
	if (HasProcessContents()) {
		std::string processContents(Node::GetAttribute<const char*>("processContents"));
		if (!processContents.compare("lax")) {
			return Any::LAX;
		} else if (!processContents.compare("skip")) {
			return Any::SKIP;
		} else if (!processContents.compare("strict")) {
			return Any::STRICT;
		} else
			throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	}
	return Any::STRICT;
}

bool
Any::HasMaxOccurs() const {
	return Node::HasAttribute("maxOccurs");
}

bool
Any::HasMinOccurs() const {
	return Node::HasAttribute("minOccurs");
}

bool
Any::HasNamespace() const {
	return Node::HasAttribute("namespace");
}

bool
Any::HasProcessContents() const {
	return Node::HasAttribute("processContents");
}
