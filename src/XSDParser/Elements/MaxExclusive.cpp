/*
 * MaxExclusive.cpp
 *
 *  Created on: Jul 29, 2011
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
#include "./src/XSDParser/Elements/MaxExclusive.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

MaxExclusive::MaxExclusive(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

MaxExclusive::MaxExclusive(const MaxExclusive& cpy)
	: Node(cpy)
{ }

void
MaxExclusive::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* no children allowed */
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
MaxExclusive::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	rProcessor.ProcessMaxExclusive(this);
}

bool
MaxExclusive::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	return true;
}

long double
MaxExclusive::Value() const throw(XMLException) {
	return Node::GetAttribute<long double>("value");
}

bool
MaxExclusive::HasValue() const {
	return Node::HasAttribute("value");
}
