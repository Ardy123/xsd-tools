/*
 * SimpleContent.cpp
 *
 *  Created on: Jul 22, 2011
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
#include <string>
#include <tinyxml.h>
#include "./src/XSDParser/Elements/SimpleContent.hpp"
#include "./src/XSDParser/Elements/Extension.hpp"
#include "./src/XSDParser/Elements/Restriction.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

SimpleContent::SimpleContent(const TiXmlElement& elm, const Parser& rParser)
	: Node(elm, rParser)
{ }

SimpleContent::SimpleContent(const SimpleContent& rType)
	: Node(rType)
{ }

void
SimpleContent::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), Restriction) ||
				XSD_ISELEMENT(pNode.get(), Annotation) ||
				XSD_ISELEMENT(pNode.get(), Extension)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
SimpleContent::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	rProcessor.ProcessSimpleContent(this);
}

Types::BaseType *
SimpleContent::GetParentType() const throw(XMLException) {
	std::auto_ptr<Restriction> pRestriction(Node::SearchXSDChildElm<Restriction>());
	std::auto_ptr<Extension> pExtension(Node::SearchXSDChildElm<Extension>());
	if ((NULL != pRestriction.get()) && (NULL == pExtension.get())) {
		return pRestriction->GetParentType();
	} else if ((NULL == pRestriction.get()) && (NULL != pExtension.get())) {
		return pExtension->GetParentType();
	} else /* simple content can't have multiple child modifiers */
		throw XMLException(Node::GetXMLElm(), XMLException::InvallidChildXMLElement);
}
