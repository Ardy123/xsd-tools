/*
 * Sequence.cpp
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
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Choice.hpp"
#include "./src/XSDParser/Elements/Group.hpp"
#include "./src/XSDParser/Elements/Any.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

Sequence::Sequence(const TiXmlElement& elm, const Parser& rParser)
	: Node(elm, rParser)
{ }

Sequence::Sequence(const Sequence& rCpy)
	: Node(rCpy)
{ }

void
Sequence::ParseChildren(BaseProcessor& rProcessor) const noexcept(false) {
	/* process children */
	std::unique_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Element) ||
				XSD_ISELEMENT(pNode.get(), Choice) ||
				XSD_ISELEMENT(pNode.get(), Annotation) ||
				XSD_ISELEMENT(pNode.get(), Sequence) ||
				XSD_ISELEMENT(pNode.get(), Any)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::unique_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Sequence::ParseElement(BaseProcessor& rProcessor) const noexcept(false) {
	rProcessor.ProcessSequence(this);
}

Types::BaseType * 
Sequence::GetParentType() const noexcept(false) {
	std::unique_ptr<Node> pParent(Node::Parent());
	return pParent->GetParentType();
}

bool
Sequence::HasElements() const noexcept(false) {
	return this->HasContent(Element::XSDTag());
}
