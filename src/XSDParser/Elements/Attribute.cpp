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
		/* if the attribute is a child of the schema root, it must have a name */
		std::auto_ptr<Node> pParent(Node::Parent());
		if (XSD_ISELEMENT(pParent.get(), SimpleType))
			throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	} else if (HasRef()) {
		/* an attribute cannot have a name and ref */
		throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	}
	if (HasType()) {
		/* an attribute cannot have a type field and a type defined as child */
		if (Node::HasContent(SimpleType::XSDTag()))
			throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	} else if (!Node::HasContent(SimpleType::XSDTag())) {
		/* an attribute must have a type defined as a child if a type is not defined */
		throw XMLException(m_rXmlElm, XMLException::MissingChildXMLElement);
	}
	if (HasDefault() && HasFixed()) {
		/* attributes cannot have fixed values and default values at the same time */
		throw XMLException(m_rXmlElm, XMLException::InvalidAttribute);
	}
	/* if the node is a reference, check its reference */
	if (HasRef()) {
		std::auto_ptr<Node> pRefElm(RefAttribute());
		pRefElm->ParseElement(rProcessor);
	} else {
		rProcessor.ProcessAttribute(this);
	}
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

std::string 
Attribute::Fixed() const throw(XMLException) {
	return std::string(Node::GetAttribute<const char*>("fixed"));
}

Attribute::AttributeUse 
Attribute::Use() const throw(XMLException) {
	std::string use(Node::GetAttribute<const char*>("use"));
	if (!use.compare("optional")) {
		return Attribute::OPTIONAL;
	} else if (!use.compare("prohibited")) {
		return Attribute::PROHIBITIED;
	} else if (!use.compare("required")) {
		return Attribute::REQUIRED;
	}
	throw XMLException(Node::GetXMLElm(), XMLException::InvalidAttributeValue);
	return Attribute::OPTIONAL;
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

bool
Attribute::HasFixed() const {
	return this->HasAttribute("fixed");
}

bool
Attribute::HasUse() const {
	return this->HasAttribute("use");
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
