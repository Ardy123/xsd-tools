/*
 * Restriction.cpp
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
#include "./src/XSDParser/Elements/Restriction.hpp"
#include "./src/XSDParser/Elements/Group.hpp"
#include "./src/XSDParser/Elements/Choice.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Attribute.hpp"
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/ComplexType.hpp"
#include "./src/XSDParser/Elements/ComplexContent.hpp"
#include "./src/XSDParser/Elements/SimpleContent.hpp"
#include "./src/XSDParser/Elements/MinExclusive.hpp"
#include "./src/XSDParser/Elements/MaxExclusive.hpp"
#include "./src/XSDParser/Elements/MinInclusive.hpp"
#include "./src/XSDParser/Elements/MaxInclusive.hpp"
#include "./src/XSDParser/Elements/MinLength.hpp"
#include "./src/XSDParser/Elements/MaxLength.hpp"
#include "./src/XSDParser/Elements/Enumeration.hpp"
#include "./src/XSDParser/Elements/FractionDigits.hpp"
#include "./src/XSDParser/Elements/Pattern.hpp"
#include "./src/XSDParser/Elements/TotalDigits.hpp"
#include "./src/XSDParser/Elements/WhiteSpace.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Elements/All.hpp"

using namespace XSD;
using namespace XSD::Elements;

Restriction::Restriction(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

Restriction::Restriction(const Restriction& rCpy)
	: Node(rCpy)
{ }

void
Restriction::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	if (isParentComplexContent()) {
		/* process children */
		std::auto_ptr<Node> pNode(Node::FirstChild());
		if (NULL != pNode.get()) {
			do {
				if (XSD_ISELEMENT(pNode.get(), Group) ||
					XSD_ISELEMENT(pNode.get(), Choice) ||
					XSD_ISELEMENT(pNode.get(), Sequence) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::Attribute) ||
					XSD_ISELEMENT(pNode.get(), All) ||
					XSD_ISELEMENT(pNode.get(), Annotation) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::ComplexContent)) {
					pNode->ParseElement(rProcessor);
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
		}
	} else if (isParentSimpleContent()){
		/* process children */
		std::auto_ptr<Node> pNode(Node::FirstChild());
		if (NULL != pNode.get()) {
			do {
				if (XSD_ISELEMENT(pNode.get(), XSD::Elements::Attribute) ||
					XSD_ISELEMENT(pNode.get(), Annotation)) {
					pNode->ParseElement(rProcessor);
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
		}
	} else {
		/* process children */
		std::auto_ptr<Node> pNode(Node::FirstChild());
		std::auto_ptr<Types::BaseType> pBase(Base());
		if (NULL != pNode.get()) {
			do {
				if (XSD_ISELEMENT(pNode.get(), XSD::Elements::MinExclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxExclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MinInclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxInclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::FractionDigits) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::TotalDigits)) {
					/* verify that parent restriction type is numeric */
					Types::Decimal	allowedBaseType;
					if (!pBase->isTypeRelated(&allowedBaseType))
						throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
					else {
						/* process element */
						pNode->ParseElement(rProcessor);
					}
				} else if ( XSD_ISELEMENT(pNode.get(), XSD::Elements::MinLength) ||
							XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxLength) ||
							XSD_ISELEMENT(pNode.get(), XSD::Elements::Pattern) ||
							XSD_ISELEMENT(pNode.get(), XSD::Elements::WhiteSpace)) {
					/* verify that parent restriction type is a string */
					Types::String	allowedBaseType;
					if (!pBase->isTypeRelated(&allowedBaseType))
						throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
					else {
						/* process element */
						pNode->ParseElement(rProcessor);
					}
				} else if (XSD_ISELEMENT(pNode.get(), XSD::Elements::Enumeration) ||
						   XSD_ISELEMENT(pNode.get(), XSD::Elements::Annotation)) {
					/* process element */
					pNode->ParseElement(rProcessor);
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
		}
	}
}

void
Restriction::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	/* make sure the base element is there */
	if (!Node::HasAttribute("base"))
		throw XMLException(GetXMLElm(), XMLException::MissingAttribute);
	/* make sure that if the parent is a simpleContent element that the base
	 * type is a simple type or base type */
	if (isParentSimpleContent()) {
		std::auto_ptr<Types::BaseType> pType(Base());
		if (XSD_ISTYPE(pType.get(), Types::ComplexType))
			throw XMLException(GetXMLElm(), XMLException::RestrictionTypeMismatch);
	}
	/* make sure the restriction is related to the base element */
	std::auto_ptr<Types::BaseType> pBase(Base());
	if (XSD_ISTYPE(pBase.get(), Types::ComplexType)) {
		Types::ComplexType* pCmplxType = static_cast<Types::ComplexType*>(pBase.get());
		if (!_isElmRelated(&Node::GetXMLElm(), &pCmplxType->m_pValue->GetXMLElm()))
			throw XMLException(GetXMLElm(), XMLException::RestrictionTypeMismatch);
	}
	/* process element */
	rProcessor.ProcessRestriction(this);
}

bool
Restriction::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	std::auto_ptr<Types::BaseType> pRstrctnType(Base());
	if (XSD_ISTYPE(pRstrctnType.get(), Types::SimpleType)) {
		/* continue forward search for base type */
		return pRstrctnType->isTypeRelated(pType);
	} else if (XSD_ISTYPE(pRstrctnType.get(), Types::ComplexType)) {
		/* TODO: Iterate across children & compare against type */
	} else {
		if (XSD_ISTYPE(pType, Types::SimpleType) ||
			XSD_ISTYPE(pType, Types::ComplexType)) {
			/* break down type to compare */
			return pType->isTypeRelated(pRstrctnType.get());
		} else
			return pRstrctnType->isTypeRelated(pType);
	}
	return false;

}

/*static */ bool
Restriction::_isElmRelated(const TiXmlElement* pRstrctn, const TiXmlElement* pBase) throw(XMLException) {
	const TiXmlElement* pRstrctnChld = pRstrctn->FirstChildElement();
	for (; pRstrctnChld ; pRstrctnChld = pRstrctnChld->NextSiblingElement()) {
		/* find node in pBase */
		const TiXmlElement* pFndElm = _findElm(pBase, pRstrctnChld);
		if (pFndElm) {
			if (pRstrctnChld->FirstChildElement())
				if (_isElmRelated(pRstrctnChld, pFndElm) == false )
					return false;
		} else
			return false;
	}
	return true;
}

/* static */ const TiXmlElement*
Restriction::_findElm(const TiXmlElement* pTreeBase, const TiXmlElement* pNode) throw(XMLException) {
	const TiXmlElement* pBaseChld = pTreeBase->FirstChildElement(pNode->Value());
	bool cmpHasName = (pNode->Attribute("name") != NULL);
	bool cmpHasType = (pNode->Attribute("type") != NULL);
	for ( ; pBaseChld ; pBaseChld = pBaseChld->NextSiblingElement(pNode->Value())) {
		if (cmpHasName) {
			if (pBaseChld->Attribute("name") != NULL) {
				if (0 == strcmp(pNode->Attribute("name"), pBaseChld->Attribute("name")))
					return pBaseChld;
				else
					continue;
			} else {
				continue;
			}
		}
		if (cmpHasType) {
			if (pBaseChld->Attribute("type") != NULL) {
				if (0 == strcmp(pNode->Attribute("type"), pBaseChld->Attribute("type")))
					return pBaseChld;
				else
					continue;
			} else {
				continue;
			}
		}
		return pBaseChld; /* !cmpHasName && !cmpHasType */
	}
	return NULL;
}

bool
Restriction::isParentComplexContent() const throw(XMLException) {
	const TiXmlElement* pNode = Node::GetXMLElm().Parent()->ToElement();
	if (pNode) {
		if (0 == strcmp(pNode->Value(), ComplexContent::XSDTag()))
			return true;
	}
	return false;
}

bool
Restriction::isParentSimpleContent() const throw(XMLException) {
	const TiXmlElement* pNode = Node::GetXMLElm().Parent()->ToElement();
	if (pNode) {
		if (0 == strcmp(pNode->Value(), SimpleContent::XSDTag()))
			return true;
	}
	return false;
}

Types::BaseType*
Restriction::Base() const throw(XMLException) {
	return Node::GetAttribute<Types::BaseType*>("base");
}