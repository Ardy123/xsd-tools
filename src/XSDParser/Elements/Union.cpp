/*
 * Union.cpp
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
#include <string>
#include <tinyxml.h>
#include <boost/tokenizer.hpp>
#include "./src/XSDParser/Elements/Union.hpp"
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

typedef boost::char_separator<std::string::value_type> SpaceCommaSeperator;
typedef boost::tokenizer<SpaceCommaSeperator> SpaceCommaTokenizer;

Union::Union(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

Union::Union(const Union& rCpy)
	: Node(rCpy)
{ }

void
Union::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), SimpleType) ||
				XSD_ISELEMENT(pNode.get(), Annotation)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Union::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	rProcessor.ProcessUnion(this);
}

Types::BaseType * 
Union::GetParentType() const throw(XMLException) {
	return new Types::String();
}

Union::TypeLst*
Union::MemberTypes() const throw(XMLException) {
	Union::TypeLst* retLst	= new TypeLst();
	/* tokenize each item in the list */
	const std::string typeLst(this->GetAttribute<const char*>("memberTypes"));
	SpaceCommaSeperator sep(", ");
	SpaceCommaTokenizer tokens(typeLst, sep);
	for (SpaceCommaTokenizer::iterator itr = tokens.begin();
		 itr != tokens.end();
		 ++itr) {
		const std::string& token = *itr;
		retLst->push_back(Node::LookupType(token.c_str()));
	}
	return retLst;
}

bool
Union::HasMemberTypes() const {
	return Node::HasAttribute("memberTypes");
}
