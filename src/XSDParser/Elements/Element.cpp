/*
 * Element.cpp
 *
 *  Created on: Jun 25, 2011
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
#include "./src/XSDParser/Types.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/ComplexType.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"

using namespace XSD;
using namespace XSD::Elements;

Element::Element(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: Node(elm, rRoot, rParser)
{ }

Element::Element(const Element& elm)
	: Node(elm)
{ }

void
Element::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), SimpleType) ||
				XSD_ISELEMENT(pNode.get(), Annotation) ||
				XSD_ISELEMENT(pNode.get(), ComplexType)) {
				pNode->ParseElement(rProcessor);
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Element::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	/* don't process if node is abstract */
	if (Abstract()) return;
	/* if an element is a substitution group verify types */
	if (HasSubstitutionGroup()) {
		VerifySubstitutionGroup();
	}
	/* a name is only allowed when it's parent element is a schema */
	if (HasName()) {
		if (HasRef())
			throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	} else if (Node::IsRootNode()) {
		throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	}
	/* if the node is a reference, check its reference */
	if (HasRef()) {
		std::auto_ptr<XSD::Elements::Element> pRefElm(RefElement());
		if (pRefElm->Abstract()) return;
		/* a name is only allowed when it's parent element is a schema */
		if (!pRefElm->IsRootNode())
			throw XMLException(pRefElm->m_rXmlElm, XMLException::InvalidAttribute);
		/* if an element is a substitution group verify types */
		if (pRefElm->HasSubstitutionGroup()) {
			VerifySubstitutionGroup();
		}
	}
	return rProcessor.ProcessElement(this);
}

bool
Element::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	if (this->HasChildType()) {
		/* process children - forward search to break down type*/
		std::auto_ptr<Node> pNode(Node::FirstChild());
		if (NULL != pNode.get()) {
			do {
				if (XSD_ISELEMENT(pNode.get(), SimpleType) ||
					XSD_ISELEMENT(pNode.get(), ComplexType)) {
					if (false == pNode->isTypeRelated(pType)) {
						throw XMLException(this->m_rXmlElm, XMLException::SubstitutionGroupTypeMismatch);
						return false;
					} else
						return true;
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
		}
	} else if (this->HasType()) {
		std::auto_ptr<Types::BaseType> pType(this->Type());
		return pType->isTypeRelated(pType.get());
	}
	return false;
}

bool
Element::Abstract() const {
	if (Node::HasAttribute("abstract"))
		return Node::GetAttribute<bool>("abstract");
	else
		return false;
}

std::string
Element::Name() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("name"));
}

Element*
Element::SubstitutionGroup() const throw(XMLException) {
	return Node::FindXSDElm<Element>(Node::GetAttribute<const char*>("substitutionGroup"));
}

bool
Element::VerifySubstitutionGroup() const throw(XMLException) {
	std::auto_ptr<Element> pSubElm(this->SubstitutionGroup());
	if (this->HasChildType()) {
		/* process children */
		std::auto_ptr<Node> pNode(Node::FirstChild());
		if (NULL != pNode.get()) {
			do {
				if (XSD_ISELEMENT(pNode.get(), SimpleType)) {
					const Elements::SimpleType* pSmplTypeElm = static_cast<const SimpleType*>(pNode.get());
					std::auto_ptr<Types::SimpleType> pSmplType(new Types::SimpleType(new Elements::SimpleType(*pSmplTypeElm)));
					return pSubElm->isTypeRelated(pSmplType.get());
				} else if (XSD_ISELEMENT(pNode.get(), ComplexType)) {
					const Elements::ComplexType* pCmplxTypeElm = static_cast<const ComplexType*>(pNode.get());
					std::auto_ptr<Types::ComplexType> pCmplxType(new Types::ComplexType(new Elements::ComplexType(*pCmplxTypeElm)));
					return pSubElm->isTypeRelated(pCmplxType.get());
				} else
					throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
			} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
		}
	} else if (this->HasType()) {
		std::auto_ptr<Types::BaseType> pType(this->Type());
		return pSubElm->isTypeRelated(pType.get());
	}
	return true;
}

Types::BaseType*
Element::Type() const throw(XMLException) {
	if (HasRef()) {
		std::auto_ptr<XSD::Elements::Element> pRefElm(RefElement());
		return _ParseType(*pRefElm.get());
	} else {
		return _ParseType(*this);
	}
}

Element*
Element::RefElement() const throw(XMLException) {
	return Node::FindXSDRef<Element>("ref");
}

int
Element::MaxOccurs() const {
	if (strcmp(Node::GetAttribute<const char*>("maxOccurs"), "unbounded"))
		return Node::GetAttribute<int>("maxOccurs");
	return -1;
}

bool
Element::HasName() const {
	return Node::HasAttribute("name");
}

bool
Element::HasSubstitutionGroup() const {
	return Node::HasAttribute("substitutionGroup");
}

bool
Element::HasRef() const {
	return Node::HasAttribute("ref");
}

bool
Element::HasType() const {
	return Node::HasAttribute("type");
}
bool
Element::HasChildType() const {
	bool hasSmplType = Node::HasContent(SimpleType::XSDTag());
	bool hasCmplxType = Node::HasContent(ComplexType::XSDTag());
	return (hasSmplType | hasCmplxType);
}

bool
Element::HasMaxOccurs() const {
	return Node::HasAttribute("maxOccurs");
}

Types::BaseType*
Element::_Type() const throw(XMLException) {
	Types::BaseType* pRetType = Node::GetAttribute<Types::BaseType*>("type");
	if (XSD_ISTYPE(pRetType, Types::Unknown)) {
		delete pRetType;
		return new Types::String();
	} else if(XSD_ISTYPE(pRetType, Types::Unsupported))
		throw XMLException(m_rXmlElm, XMLException::UndefiniedXSDType);
	return pRetType;
}

/* static */ Types::BaseType*
Element::_ParseType(const Element& rElm) throw(XMLException) {
	if (rElm.HasChildType() &&
		(rElm.HasContent(SimpleType::XSDTag()) || rElm.HasContent(ComplexType::XSDTag()))) {
		if (rElm.HasContent(SimpleType::XSDTag())) {
			return new Types::SimpleType(rElm.FindXSDChildElm<SimpleType>());
		} else {
			return new Types::ComplexType(rElm.FindXSDChildElm<ComplexType>());
		}
	} else if (rElm.HasType()) {
		return rElm._Type();
	} else if (rElm.HasSubstitutionGroup()) {
		std::auto_ptr<Element> pElm(rElm.SubstitutionGroup());
		return _ParseType(*(pElm.get()));
	} else {
		return new Types::String();
	}
}
