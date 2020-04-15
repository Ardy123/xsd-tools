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
#include "./src/XSDParser/Elements/Length.hpp"
#include "./src/XSDParser/Elements/Enumeration.hpp"
#include "./src/XSDParser/Elements/FractionDigits.hpp"
#include "./src/XSDParser/Elements/Pattern.hpp"
#include "./src/XSDParser/Elements/TotalDigits.hpp"
#include "./src/XSDParser/Elements/WhiteSpace.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Elements/All.hpp"
#include "./src/Processors/RestrictionVerify.hpp"

using namespace XSD;
using namespace XSD::Elements;

Restriction::Restriction(const TiXmlElement& elm, const Parser& rParser)
	: Node(elm, rParser)
{ }

Restriction::Restriction(const Restriction& rCpy)
	: Node(rCpy)
{ }

void
Restriction::ParseChildren(BaseProcessor& rProcessor) const noexcept(false) {
	if (isParentComplexContent()) {
		/* process children */
		std::unique_ptr<Node> pNode(Node::FirstChild());
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
			} while (NULL != (pNode = std::unique_ptr<Node>(pNode->NextSibling())).get());
		}
	} else if (isParentSimpleContent()){
		/* process children */
		std::unique_ptr<Node> pNode(Node::FirstChild());
		if (NULL != pNode.get()) {
			do {
				if (XSD_ISELEMENT(pNode.get(), XSD::Elements::Attribute) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MinExclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxExclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MinInclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxInclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::FractionDigits) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::TotalDigits) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MinLength) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxLength) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::Length) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::Pattern) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::WhiteSpace) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::Enumeration) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::Annotation)) {
					  pNode->ParseElement(rProcessor);
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} while (NULL != (pNode = std::unique_ptr<Node>(pNode->NextSibling())).get());
		}
	} else {
		/* process children */
		std::unique_ptr<Node> pNode(Node::FirstChild());
		std::unique_ptr<Types::BaseType> pBase(Base());
		if (NULL != pNode.get()) {
			do {
				if (XSD_ISELEMENT(pNode.get(), XSD::Elements::MinExclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxExclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MinInclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::MaxInclusive) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::FractionDigits) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::TotalDigits) ||
					XSD_ISELEMENT(pNode.get(), XSD::Elements::Annotation)) {
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
							XSD_ISELEMENT(pNode.get(), XSD::Elements::Length) ||
							XSD_ISELEMENT(pNode.get(), XSD::Elements::Pattern) ||
							XSD_ISELEMENT(pNode.get(), XSD::Elements::WhiteSpace) ||
							XSD_ISELEMENT(pNode.get(), XSD::Elements::Annotation)) {
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
			} while (NULL != (pNode = std::unique_ptr<Node>(pNode->NextSibling())).get());
		}
	}
}

void
Restriction::ParseElement(BaseProcessor& rProcessor) const noexcept(false) {
	/* make sure the base element is there */
	if (!Node::HasAttribute("base"))
		throw XMLException(GetXMLElm(), XMLException::MissingAttribute);
	/* make sure that if the parent is a simpleContent element that the base
	 * type is a simple type or base type */
	if (isParentSimpleContent()) {
		std::unique_ptr<Types::BaseType> pType(Base());
		if (XSD_ISTYPE(pType.get(), Types::ComplexType))
			throw XMLException(GetXMLElm(), XMLException::RestrictionTypeMismatch);
	}
	/* make sure the restriction is related to the base element */
	std::unique_ptr<Types::BaseType> pBase(Base());
	if (XSD_ISTYPE(pBase.get(), Types::ComplexType)) {
		Types::ComplexType* pCmplxType = static_cast<Types::ComplexType*>(pBase.get());
		Processors::RestrictionVerify verifyRestriciton;
		if (!verifyRestriciton.Verify(this, pCmplxType->m_pValue))
			throw XMLException(GetXMLElm(), XMLException::RestrictionTypeMismatch);
		/*
		if (!_isElmRelated(this, &pCmplxType->m_pValue->GetXMLElm()))
			throw XMLException(GetXMLElm(), XMLException::RestrictionTypeMismatch);
		*/
	}
	/* process element */
	rProcessor.ProcessRestriction(this);
}

Types::BaseType * 
Restriction::GetParentType() const noexcept(false) {
	return this->Base();
}

/*static * bool
Restriction::_isElmRelated(const Node* pRstrctn, const TiXmlElement* pBase) const noexcept(false) {
	std::unique_ptr<Node> pRstrctnChld(pRstrctn->FirstChild());
	* iterate through children comparing against parent type *
	if (NULL != pRstrctnChld.get()) {
	  do {
			if (!(XSD_ISELEMENT(pRstrctnChld.get(), Annotation))) {
				* find node in parent XSD type *
				const TiXmlElement* pFndElm = _findElm(pBase, &(pRstrctnChld.get()->GetXMLElm()));
				if (pFndElm) {
					if (_isElmRelated(pRstrctnChld.get(), pFndElm) == false )
						return false;
				} else
					return false;
			}
		} while (NULL != (pRstrctnChld = std::unique_ptr<Node>(pRstrctnChld->NextSibling())).get());
	}
	return true;
}

* static * const TiXmlElement*
Restriction::_findElm(const TiXmlElement* pTreeBase, const TiXmlElement* pNode) noexcept(false) {
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
		return pBaseChld; * !cmpHasName && !cmpHasType *
	}
	return NULL;
}
*/
bool
Restriction::isParentComplexContent() const noexcept(false) {
	std::unique_ptr<Node> pParent(Node::Parent());
	return XSD_ISELEMENT(pParent.get(), XSD::Elements::ComplexContent);
}

bool
Restriction::isParentSimpleContent() const noexcept(false) {
	std::unique_ptr<Node> pParent(Node::Parent());
	return XSD_ISELEMENT(pParent.get(), XSD::Elements::SimpleContent);
}

Types::BaseType*
Restriction::Base() const noexcept(false) {
	return Node::GetAttribute<Types::BaseType*>("base");
}
