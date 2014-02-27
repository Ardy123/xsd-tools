/*
 * Choice.cpp
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

#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <memory>
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Choice.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/ProcessorBase.hpp"

using namespace XSD;
using namespace XSD::Elements;

Choice::Choice(const TiXmlElement& elm, const Schema& docRoot, const Parser& rParser)
	: Node(elm, docRoot, rParser)
{ }

Choice::Choice(const Choice& rCpy)
	: Node(rCpy)
{ }

void
Choice::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Element) ||
				XSD_ISELEMENT(pNode.get(), Choice) ||
				XSD_ISELEMENT(pNode.get(), Annotation) ||
				XSD_ISELEMENT(pNode.get(), Sequence)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Choice::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	/* verify that 'maxOccurs' is not negative unless its -1 (unbounded) */
	if (-1 > MaxOccurs())
		throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	/* verify that 'minOccurs' is non-negative */
	if (0 > MinOccurs())
		throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	/* process element */
	rProcessor.ProcessChoice(this);
}

Types::BaseType * 
Choice::GetParentType() const throw(XMLException) {
	std::auto_ptr<Node> pParent(Node::Parent());
	return pParent->GetParentType();
}

int
Choice::MaxOccurs() const {
	if (HasMaxOccurs()) {
		if (strcmp(Node::GetAttribute<const char*>("maxOccurs"), "unbounded"))
			return Node::GetAttribute<int>("maxOccurs");
		else
			return -1;
	}
	return 1;
}

int
Choice::MinOccurs() const {
	if (HasMinOccurs()) {
		return Node::GetAttribute<int>("minOccurs");
	}
	return 1;
}

bool
Choice::HasElements() const throw(XMLException) {
	return Node::HasContent(Element::XSDTag());
}

bool
Choice::HasSequence() const throw(XMLException) {
	return Node::HasContent(Sequence::XSDTag());
}

bool
Choice::HasMaxOccurs() const {
	return Node::HasAttribute("maxOccurs");
}

bool
Choice::HasMinOccurs() const {
	return Node::HasAttribute("minOccurs");
}

