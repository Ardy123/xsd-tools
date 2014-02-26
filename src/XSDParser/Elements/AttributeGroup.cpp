/*
 * AttributeGroup.cpp
 *
 *  Created on: Aug 4, 2011
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
#include <string.h>
#include <string>
#include <tinyxml.h>
#include "./src/XSDParser/Elements/AttributeGroup.hpp"
#include "./src/XSDParser/Elements/Attribute.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

AttributeGroup::AttributeGroup(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

AttributeGroup::AttributeGroup(const AttributeGroup& cpy)
	: Node(cpy)
{ }

void
AttributeGroup::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Attribute) ||
				XSD_ISELEMENT(pNode.get(), Annotation)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
AttributeGroup::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	/* a ref and a name are not allowed */
	if (HasName() && HasRef())
		throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	/* a name is only allowed when it's parent element is a schema */
	if (HasName()) {
		if (m_rXmlElm.Parent()->Type() != TiXmlNode::TINYXML_DOCUMENT)
			throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	}
	if (HasRef()) {
		std::auto_ptr<AttributeGroup> pRefGroup(RefGroup());
		/* a name is only allowed when it's parent element is a schema */
		if (pRefGroup->m_rXmlElm.Parent()->Type() != TiXmlNode::TINYXML_DOCUMENT)
			throw XMLException(pRefGroup->m_rXmlElm, XMLException::InvalidAttribute);
	}
	rProcessor.ProcessAttributeGroup(this);
}

Types::BaseType * 
AttributeGroup::GetParentType() const throw(XMLException) {
	std::auto_ptr<Node> pParent(Node::Parent());
	return pParent->GetParentType();
}

std::string
AttributeGroup::Name() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("name"));
}

AttributeGroup*
AttributeGroup::RefGroup() const throw(XMLException) {
	return Node::FindXSDRef<AttributeGroup>("ref");
}

bool
AttributeGroup::HasName() const {
	return Node::HasAttribute("name");
}

bool
AttributeGroup::HasRef() const {
	return Node::HasAttribute("ref");
}
