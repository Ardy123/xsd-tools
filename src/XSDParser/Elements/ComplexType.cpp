/*
 * ComplexType.cpp
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
 *  along with Xsd-Tools.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <memory>
#include <string>
#include <tinyxml.h>
#include "./src/XSDParser/Elements/ComplexType.hpp"
#include "./src/XSDParser/Exception.hpp"
#include "./src/XSDParser/Elements/Node.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Attribute.hpp"
#include "./src/XSDParser/Elements/AttributeGroup.hpp"
#include "./src/XSDParser/Elements/Choice.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/SimpleContent.hpp"
#include "./src/XSDParser/Elements/ComplexContent.hpp"
#include "./src/XSDParser/Elements/Group.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Elements/All.hpp"

using namespace XSD;
using namespace XSD::Elements;

ComplexType::ComplexType(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

ComplexType::ComplexType(const ComplexType& rType)
	: Node(rType)
{ }

void
ComplexType::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		enum { CMODEL_UNKNOWN, CMODEL_TRUE, CMODEL_FALSE };
		int contentModel = CMODEL_UNKNOWN;
		do {
			if (XSD_ISELEMENT(pNode.get(), ComplexContent) ||
				XSD_ISELEMENT(pNode.get(), SimpleContent)) {
				if (CMODEL_FALSE != contentModel) {
					contentModel = CMODEL_TRUE;
					pNode->ParseElement(rProcessor);
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} else if ( XSD_ISELEMENT(pNode.get(), Group) ||
						XSD_ISELEMENT(pNode.get(), All) ||
						XSD_ISELEMENT(pNode.get(), Choice) ||
						XSD_ISELEMENT(pNode.get(), Sequence) ||
						XSD_ISELEMENT(pNode.get(), Attribute) ||
						XSD_ISELEMENT(pNode.get(), AttributeGroup)) {
				if (CMODEL_TRUE != contentModel) {
					contentModel = CMODEL_FALSE;
					pNode->ParseElement(rProcessor);
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} else if (XSD_ISELEMENT(pNode.get(), Annotation)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
ComplexType::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	if (HasName() && !Node::IsRootNode()) {
		throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	}
	rProcessor.ProcessComplexType(this);
}

Types::BaseType * 
ComplexType::GetParentType(void) const throw(XMLException) {
	std::auto_ptr<SimpleContent> pSimpleContent(Node::SearchXSDChildElm<SimpleContent>());
	std::auto_ptr<ComplexContent> pComplexContent(Node::SearchXSDChildElm<ComplexContent>());
	if ((NULL != pSimpleContent.get()) && (NULL == pComplexContent.get())) {
		return pSimpleContent->GetParentType();
	} else if ((NULL == pSimpleContent.get()) && (NULL != pComplexContent.get())) {
		return pComplexContent->GetParentType();
	} else if ((NULL != pSimpleContent.get()) && (NULL != pComplexContent.get())) {
		/* complex type can't have both simple and complex content modifiers */
		throw XMLException(Node::GetXMLElm(), XMLException::InvallidChildXMLElement);
	} else {
		return NULL;
	}
}

bool
ComplexType::Abstract() const {
	if (Node::HasAttribute("abstract")) {
		return Node::GetAttribute<bool>("abstract");
	}
	return false;
}

bool
ComplexType::Mixed() const {
	if (Node::HasAttribute("mixed")) {
		return Node::GetAttribute<bool>("mixed");
	}
	return false;
}

std::string
ComplexType::Name() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("name"));
}

bool
ComplexType::HasSequence() const {
	return Node::HasContent(Sequence::XSDTag());
}

bool
ComplexType::HasAttribute() const {
	return Node::HasContent(Attribute::XSDTag());
}

bool
ComplexType::HasChoice() const {
	return Node::HasContent(Choice::XSDTag());
}

bool
ComplexType::HasName() const {
	return Node::HasAttribute("name");
}
