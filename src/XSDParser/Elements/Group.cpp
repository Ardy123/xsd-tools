/*
 * Group.cpp
 *
 *  Created on: Jul 8, 2011
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
 *  along with Xsd-Tools.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <memory>
#include <string.h>
#include <string>
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Group.hpp"
#include "./src/XSDParser/Elements/Choice.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Elements/All.hpp"

using namespace XSD;
using namespace XSD::Elements;

Group::Group(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

Group::Group(const Group& cpy)
	: Node(cpy)
{ }

void
Group::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Choice) ||
				XSD_ISELEMENT(pNode.get(), Annotation) ||
				XSD_ISELEMENT(pNode.get(), All) ||
				XSD_ISELEMENT(pNode.get(), Sequence)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Group::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	/* a ref and a name are not allowed */
	if (HasName() && HasRef())
		throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	/* a name is only allowed when it's parent element is a schema */
	if (HasName()) {
		if (m_rXmlElm.Parent()->Type() != TiXmlNode::TINYXML_DOCUMENT)
			throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	}
	if (HasRef()) {
		std::auto_ptr<Group> pRefGroup(RefGroup());
		/* a name is only allowed when it's parent element is a schema */
		if (pRefGroup->m_rXmlElm.Parent()->Type() != TiXmlNode::TINYXML_DOCUMENT)
			throw XMLException(pRefGroup->m_rXmlElm, XMLException::InvalidAttribute);
	}
	/* verify that 'maxOccurs' is not negative unless its -1 (unbounded) */
	if (-1 > MaxOccurs())
		throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	/* verify that 'minOccurs' is non-negative */
	if (0 > MinOccurs())
		throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	/* process element */
	rProcessor.ProcessGroup(this);
}

Types::BaseType * 
Group::GetParentType() const throw(XMLException) {
	std::auto_ptr<Node> pParent(Node::Parent());
	return pParent->GetParentType();
}

int
Group::MaxOccurs() const {
	if (HasMaxOccurs()) {
		if (strcmp(Node::GetAttribute<const char*>("maxOccurs"), "unbounded"))
			return Node::GetAttribute<int>("maxOccurs");
		else
			return -1;
	}
	return 1;
}

int
Group::MinOccurs() const {
	if (HasMinOccurs()) {
		return Node::GetAttribute<int>("minOccurs");
	}
	return 1;
}

std::string
Group::Name() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("name"));
}

Group*
Group::RefGroup() const throw(XMLException) {
	return Node::FindXSDRef<Group>("ref");
}

bool
Group::HasMaxOccurs() const {
	return Node::HasAttribute("maxOccurs");
}

bool
Group::HasMinOccurs() const {
	return Node::HasAttribute("minOccurs");
}

bool
Group::HasName() const {
	return Node::HasAttribute("name");
}

bool
Group::HasRef() const {
	return Node::HasAttribute("ref");
}
