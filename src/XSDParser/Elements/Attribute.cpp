/*
 * Attribute.cpp
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
#include <typeinfo>
#include <memory>
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Attribute.hpp"
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/ProcessorBase.hpp"

using namespace XSD;
using namespace XSD::Elements;

Attribute::Attribute(const TiXmlElement& elm, const Schema& docRoot, const Parser& rParser)
	: Node(elm, docRoot, rParser)
{ }

Attribute::Attribute(const Attribute& rAttrib)
	: Node(rAttrib)
{ }

void
Attribute::ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) {
	/* process children */
	std::auto_ptr<Node> pNode(Node::FirstChild());
	if (NULL != pNode.get()) {
		do {
			if (XSD_ISELEMENT(pNode.get(), SimpleType) ||
				XSD_ISELEMENT(pNode.get(), Annotation)) {
				rProcessor.ProcessSimpleType(static_cast<SimpleType*>(pNode.get()));
			} else
				throw XMLException(pNode->GetXMLElm(), XMLException::InvallidChildXMLElement);
		} while (NULL != (pNode = std::auto_ptr<Node>(pNode->NextSibling())).get());
	}
}

void
Attribute::ParseElement(BaseProcessor& rProcessor) const throw(XMLException) {
	if (!HasName()) {
		if (0 == strcmp(m_rXmlElm.Parent()->Value(), Schema::XSDTag()))
			throw XMLException(m_rXmlElm, XMLException::MissingAttribute);
	}
	if (HasType()) {
		if (Node::HasContent(SimpleType::XSDTag()))
			throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	} else if (!Node::HasContent(SimpleType::XSDTag())) {
		throw XMLException(m_rXmlElm, XMLException::MissingChildXMLElement);
	}
	/* if the node is a reference, check its reference */
	if (HasRef()) {
		std::auto_ptr<XSD::Elements::Attribute> pRefElm(RefAttribute());
		if (!HasName()) {
			if (0 == strcmp(m_rXmlElm.Parent()->Value(), Schema::XSDTag()))
				throw XMLException(m_rXmlElm, XMLException::MissingAttribute);
		}
		if (pRefElm->HasType()) {
			if (pRefElm->HasContent(SimpleType::XSDTag()))
				throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
		} else if (!Node::HasContent(SimpleType::XSDTag())) {
			throw XMLException(m_rXmlElm, XMLException::MissingChildXMLElement);
		}
	}
	rProcessor.ProcessAttribute(this);
}

bool
Attribute::isTypeRelated(const Types::BaseType* pType) const throw(XMLException) {
	return false; /* TODO */
}

std::string
Attribute::Name() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("name"));
}

Attribute*
Attribute::RefAttribute() const throw(XMLException) {
	return Node::FindXSDRef<Attribute>("ref");
}

Types::BaseType*
Attribute::Type() const throw(XMLException) {
	if (HasRef()) {
		std::auto_ptr<XSD::Elements::Attribute> pRefAttrib(RefAttribute());
		return _parseType(*pRefAttrib);
	} else {
		return _parseType(*this);
	}
}

std::string
Attribute::Default() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("default"));
}

bool
Attribute::HasName() const {
	return this->HasAttribute("name");
}

bool
Attribute::HasRef() const {
	return this->HasAttribute("ref");
}

bool
Attribute::HasType() const {
	return this->HasAttribute("type");
}

bool
Attribute::HasDefault() const {
	return this->HasAttribute("default");
}

Types::BaseType*
Attribute::_type() const throw(XMLException) {
	Types::BaseType* pType = Node::GetAttribute<Types::BaseType*>("type");
	if (XSD_ISTYPE(pType, Types::Unknown)) {
		delete pType;
		return new Types::String();
	} else if (XSD_ISTYPE(pType, Types::ComplexType) ||
			   XSD_ISTYPE(pType, Types::Unsupported))
		throw  XMLException(m_rXmlElm, XMLException::UndefiniedXSDType);
	return pType;
}

/* static */ Types::BaseType*
Attribute::_parseType(const Attribute& rAttrib) throw(XMLException) {
	if (rAttrib.HasContent(SimpleType::XSDTag()))
		return new Types::SimpleType(rAttrib.FindXSDChildElm<SimpleType>());
	else
		return rAttrib._type();
}
