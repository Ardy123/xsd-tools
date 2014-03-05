/*
 * ElementExtracter.cpp
 *
 *  Created on: 3/4/14
 *      Author: Ardavon Falls
 *   Copyright: (c)2012 Ardavon Falls
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

#include "./src/Processors/RestrictionVerify.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/ComplexType.hpp"
#include "./src/XSDParser/Elements/Group.hpp"
#include "./src/XSDParser/Elements/Include.hpp"
#include "./src/XSDParser/Elements/All.hpp"
#include "./src/Processors/ElementExtracter.hpp"

using namespace std;
using namespace Processors;
using namespace XSD::Elements;

RestrictionVerify::RestrictionVerify()
	: LuaProcessorBase(NULL), m_pSubTree(NULL)
{ }

RestrictionVerify::RestrictionVerify(const RestrictionVerify& rProcessor)
 	: LuaProcessorBase(rProcessor), m_pSubTree(rProcessor.m_pSubTree)
 { }
 
 RestrictionVerify::RestrictionVerify(Node* pSubTree)
	: LuaProcessorBase(NULL), m_pSubTree(pSubTree)
{ }
 
/* virtual */
RestrictionVerify::~RestrictionVerify() {
}

bool
RestrictionVerify::Verify(const Restriction* pRestriciton, const ComplexType* pParentType) throw(XSD::XMLException) {
	RestrictionVerify processor(new ComplexType(*pParentType));
	try {
		processor.ProcessRestriction(pRestriciton);
	} catch (XSD::XMLException& e) {
		if (XSD::XMLException::RestrictionTypeMismatch == e.QueryError().m_errorId) {
			return false;
		} else {
			throw;	/* rethrow exception */
		}
	}
	return true;
}

/* virtual */ void 
RestrictionVerify::ProcessElement(const Element* pNode) {
	/* find the element in the parent sub-tree */
	std::auto_ptr<Node> pParentNode(m_pSubTree->FirstChild());
	if (NULL != pParentNode.get()) {
		do {
			if (XSD_ISELEMENT(pParentNode.get(), Element)) {
				const Element * pElement = static_cast<const Element *>(pParentNode.get());
				if (pNode->HasRef() && pElement->HasRef()) {
					std::auto_ptr<Element> pRefElm(pNode->RefElement());
					std::auto_ptr<Element> pSubTreeRefElm(pElement->RefElement());
					/* verify the elements refer to the the same element */
					if (*(pRefElm.get()) == *(pSubTreeRefElm.get())) {
						return;
					}
				} else if (	pNode->HasName() && pElement->HasName() &&
							(0 == pNode->Name().compare(pElement->Name()))) {
					/* verify the elements have the same type */
					std::auto_ptr<XSD::Types::BaseType> pElmType(pNode->Type());
					std::auto_ptr<XSD::Types::BaseType> pSubTreeElmType(pElement->Type());
					if (XSD_ISTYPE(pElmType.get(), XSD::Types::SimpleType) && 
						XSD_ISTYPE(pSubTreeElmType.get(), XSD::Types::SimpleType)) {
						if ((static_cast<XSD::Types::SimpleType*>(pElmType.get()))->m_pValue == 
							(static_cast<XSD::Types::SimpleType*>(pSubTreeElmType.get()))->m_pValue) {
							return;
						}
					} else if (	XSD_ISTYPE(pElmType.get(), XSD::Types::ComplexType) &&
								XSD_ISTYPE(pSubTreeElmType.get(), XSD::Types::ComplexType)) {
						if ((static_cast<XSD::Types::ComplexType*>(pElmType.get()))->m_pValue == 
							(static_cast<XSD::Types::ComplexType*>(pSubTreeElmType.get()))->m_pValue) {
							return;
						}
					} else if (typeid(*(pElmType.get())) == typeid(*(pSubTreeElmType.get()))) {
						return;
					}
				}
			}
		} while (NULL != (pParentNode = std::auto_ptr<Node>(pParentNode->NextSibling())).get());
	}
	throw XSD::XMLException(pNode->GetXMLElm(), XSD::XMLException::RestrictionTypeMismatch);
}

/* virtual */ void 
RestrictionVerify::ProcessRestriction(const XSD::Elements::Restriction* pNode) {
	RestrictionVerify processor(m_pSubTree->FirstChild());
	pNode->ParseChildren(processor);
}

/* virtual */ void 
RestrictionVerify::ProcessSequence(const XSD::Elements::Sequence* pNode) {
	/* search for either a xs:sequence and verify that it is in the sub-tree */
	std::auto_ptr<Node> pParentNode(m_pSubTree);
	if (NULL != pParentNode.get()) {
		do {
			if (XSD_ISELEMENT(pParentNode.get(), Sequence)) {
				const Sequence * pSequence = static_cast<const Sequence *>(pParentNode.get());
				RestrictionVerify processor(new Sequence(*pSequence));
				pNode->ParseChildren(processor);
				return;
			}
		} while (NULL != (pParentNode = std::auto_ptr<Node>(pParentNode->NextSibling())).get());
	}
	throw XSD::XMLException(pParentNode->GetXMLElm(), XSD::XMLException::RestrictionTypeMismatch);
}

/* virtual */ void 
RestrictionVerify::ProcessChoice(const XSD::Elements::Choice* pNode) {
	/* search for either a xs:choice and verify that it is in the sub-tree */
	std::auto_ptr<Node> pParentNode(m_pSubTree);
	if (NULL != pParentNode.get()) {
		do {
			if (XSD_ISELEMENT(pParentNode.get(), Choice)) {
				const Choice * pChoice = static_cast<const Choice *>(pParentNode.get());
				RestrictionVerify processor(new Choice(*pChoice));
				pNode->ParseChildren(processor);
				return;
			}
		} while (NULL != (pParentNode = std::auto_ptr<Node>(pParentNode->NextSibling())).get());
	}
	throw XSD::XMLException(pNode->GetXMLElm(), XSD::XMLException::RestrictionTypeMismatch);
}

/* virtual */ void 
RestrictionVerify::ProcessGroup(const XSD::Elements::Group* pNode) {
	/* search for either a xs:group and verify that it is in the sub-tree */
	std::auto_ptr<Node> pParentNode(m_pSubTree);
	if (NULL != pParentNode.get()) {
		do {
			if (XSD_ISELEMENT(pParentNode.get(), Group)) {
				const Group * pGroup = static_cast<const Group *>(pParentNode.get());
				if (pNode->HasRef() && pGroup->HasName()) {
					std:auto_ptr<Group> pRefGroup(pNode->RefGroup());
					if (*(pRefGroup.get()) == *pGroup) {
						return;
					}
				}
			}
		} while (NULL != (pParentNode = std::auto_ptr<Node>(pParentNode->NextSibling())).get());
	}
	throw XSD::XMLException(pNode->GetXMLElm(), XSD::XMLException::RestrictionTypeMismatch);
}

/* virtual */ void 
RestrictionVerify::ProcessAll(const XSD::Elements::All* pNode) {
	/* search for either a xs:all and verify that it is in the sub-tree */
	std::auto_ptr<Node> pParentNode(m_pSubTree);
	if (NULL != pParentNode.get()) {
		do {
			if (XSD_ISELEMENT(pParentNode.get(), All)) {
				const All * pAll = static_cast<const All *>(pParentNode.get());
				RestrictionVerify processor(new All(*pAll));
				pNode->ParseChildren(processor);
				return;
			}
		} while (NULL != (pParentNode = std::auto_ptr<Node>(pParentNode->NextSibling())).get());
	}
	throw XSD::XMLException(pNode->GetXMLElm(), XSD::XMLException::RestrictionTypeMismatch);
}


