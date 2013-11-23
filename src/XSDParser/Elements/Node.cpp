/*
 * Node.cpp
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

#include <memory>
#include <boost/algorithm/string/predicate.hpp>
#include "./src/XSDParser/Elements/Node.hpp"
#include "./src/XSDParser/Elements/Attribute.hpp"
#include "./src/XSDParser/Elements/Choice.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/List.hpp"
#include "./src/XSDParser/Elements/Restriction.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Union.hpp"
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/ComplexType.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/Group.hpp"
#include "./src/XSDParser/Elements/Any.hpp"
#include "./src/XSDParser/Elements/ComplexContent.hpp"
#include "./src/XSDParser/Elements/Extension.hpp"
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
#include "./src/XSDParser/Elements/AttributeGroup.hpp"
#include "./src/XSDParser/Elements/Include.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Elements/Documentation.hpp"
#include "./src/XSDParser/Elements/All.hpp"

using namespace XSD;
using namespace XSD::Elements;

/* template specialization first */
namespace XSD{
	namespace Elements {
		template<> bool
		Node::GetAttribute<bool>(const char* pAttrib) const throw(XMLException) {
			bool retVal;
			std::string attribStr(_Attribute(pAttrib));
			std::transform(attribStr.begin(), attribStr.end(), attribStr.begin(),::tolower);
			std::stringstream sstrm(attribStr);
			sstrm >> std::boolalpha >> retVal;
			return retVal;
		}

		template<> const char*
		Node::GetAttribute<const char*>(const char* pAttrib) const throw (XMLException) {
			const char* pRetVal = m_rXmlElm.Attribute(pAttrib);
			if (!pRetVal) throw XMLException(m_rXmlElm, XMLException::MissingAttribute);
			return pRetVal;
		}

		template<> Types::BaseType*
		Node::GetAttribute<Types::BaseType*>(const char* pAttrib) const throw(XMLException) {
			const char* pTypeStr = GetAttribute<const char*>(pAttrib);
			if (!pTypeStr) throw XMLException(m_rXmlElm, XMLException::MissingAttribute);
			return _Type(pTypeStr);
		}
	}
}

/* Non-specialized methods */
Node::Node(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser)
	: m_rXmlElm(elm), m_rDocRoot(rRoot), m_rParser(rParser)
{ }

Node::Node(const Node& rCpy)
	: m_rXmlElm(rCpy.m_rXmlElm), m_rDocRoot(rCpy.m_rDocRoot), m_rParser(rCpy.m_rParser)
{}

const TiXmlElement*
Node::_FindChildXMLElement(const char* pXMLElmTag, const char* pAttrib, const char* pName) const throw(XMLException) {
	/* search all nodes in root document */
	const TiXmlElement* pElm = m_rXmlElm.FirstChildElement(pXMLElmTag);
	for ( ;
		pElm && pElm->Attribute(pAttrib) && !boost::iequals(std::string(pElm->Attribute(pAttrib)), std::string(pName));
		pElm = pElm->NextSiblingElement(pXMLElmTag) ) {}
	return pElm;
}

Node*
Node::_ConstructNode(const TiXmlElement* pElm, const Schema& rRoot, const Parser& rParser) const {
	const std::string elementName = _StripNamespace(pElm->ValueStr());
	if (boost::iequals(std::string(Attribute::XSDTag()), elementName)) {
		return new Attribute(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Choice::XSDTag()), elementName)) {
		return new Choice(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Element::XSDTag()), elementName)) {
		return new Element(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(List::XSDTag()), elementName)) {
		return new List(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Restriction::XSDTag()), elementName)) {
		return new Restriction(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Sequence::XSDTag()), elementName)) {
		return new Sequence(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Union::XSDTag()), elementName)) {
		return new Union(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Schema::XSDTag()), elementName)) {
		return new Schema(*pElm, rRoot, rParser, std::string(""));
	} else if (boost::iequals(std::string(SimpleType::XSDTag()), elementName)) {
		return new SimpleType(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(ComplexType::XSDTag()), elementName)) {
		return new ComplexType(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Group::XSDTag()), elementName)) {
		return new Group(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Any::XSDTag()), elementName)) {
		return new Any(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(ComplexContent::XSDTag()), elementName)) {
		return new ComplexContent(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Extension::XSDTag()), elementName)) {
		return new Extension(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(SimpleContent::XSDTag()), elementName)) {
		return new SimpleContent(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MinExclusive::XSDTag()), elementName)) {
		return new MinExclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MaxExclusive::XSDTag()), elementName)) {
		return new MaxExclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MinInclusive::XSDTag()), elementName)) {
		return new MinInclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MaxInclusive::XSDTag()), elementName)) {
		return new MaxInclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MinLength::XSDTag()), elementName)) {
		return new MinLength(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MaxLength::XSDTag()), elementName)) {
		return new MaxLength(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Enumeration::XSDTag()), elementName)) {
		return new Enumeration(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(FractionDigits::XSDTag()), elementName)) {
		return new FractionDigits(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Pattern::XSDTag()), elementName)) {
		return new Pattern(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(TotalDigits::XSDTag()), elementName)) {
		return new TotalDigits(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(WhiteSpace::XSDTag()), elementName)) {
		return new WhiteSpace(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(AttributeGroup::XSDTag()), elementName)) {
		return new AttributeGroup(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Include::XSDTag()), elementName)) {
		return new Include(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Annotation::XSDTag()), elementName)) {
		return new Annotation(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Documentation::XSDTag()), elementName)) {
		return new Documentation(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(All::XSDTag()), elementName)) {
		return new All(*pElm, rRoot, rParser);
	} else
		throw XMLException(*pElm, XMLException::InvallidChildXMLElement);
	return NULL;
}

Node*
Node::_FindXSDElm(const char* pName, const char* pTypeName) const throw(XMLException) {
	std::string elementName = QualifyElementName(pTypeName);
	/* search root document */
	Node* pRetNode = NULL;
	const TiXmlElement* pElm = m_rDocRoot._FindChildXMLElement(elementName.c_str(), "name", pName);
	/* search all included documents */
	if (!pElm) {
		const TiXmlElement* pIncludElm = m_rDocRoot.m_rXmlElm.FirstChildElement(XSD::Elements::Include::XSDTag());
		for ( ; pIncludElm; pIncludElm = pIncludElm->NextSiblingElement(XSD::Elements::Include::XSDTag()) ) {
			std::auto_ptr<Include> pNode(static_cast<Include*>(_ConstructNode(pIncludElm, m_rDocRoot, m_rParser)));
			const Schema* pSchema = pNode->QuerySchema();
			if (NULL != (pElm = pSchema->_FindChildXMLElement(elementName.c_str(), "name", pName))) {
				pRetNode = _ConstructNode(pElm, *pSchema, pSchema->m_rParser);
				break;
			}
		}
	} else
		pRetNode = _ConstructNode(pElm, m_rDocRoot, m_rParser);
	return pRetNode;
}

Node*
Node::_FindXSDNode(const char* pName, const char* pTypeName) const throw(XMLException) {
	Node* pNode = _FindXSDElm(pName, pTypeName);
	if (!pNode)
		throw XMLException(m_rXmlElm, XMLException::MissingElement);
	return pNode;
}

Node*
Node::_FindChildXSDNode(const char* pXMLTag) const throw(XMLException) {
	std::string elementName = QualifyElementName(pXMLTag);
	/* search all nodes in root document */
	const TiXmlElement* pElm = m_rXmlElm.FirstChildElement(elementName.c_str());
	if (NULL == pElm)
		throw XMLException(*pElm, XMLException::MissingChildXMLElement);
	return _ConstructNode(pElm, m_rDocRoot, m_rParser);
}

Node*
Node::_FindXSDRef(const char* pRefAttribStr, const char* pTypeName) const throw (XMLException) {
	if (HasAttribute(pRefAttribStr)) {
		std::auto_ptr<Node> pRefElm(_FindXSDNode(GetAttribute<const char*>(pRefAttribStr), pTypeName));
		return pRefElm->_FindXSDRef(pRefAttribStr, pTypeName);
	} else
		return _ConstructNode(&m_rXmlElm, m_rDocRoot, m_rParser);
}

std::string
Node::_Attribute(const char* pAttrib) const throw (XMLException) {
	if (HasAttribute(pAttrib)) {
		return std::string(m_rXmlElm.Attribute(pAttrib));
	} else
		throw XMLException(m_rXmlElm, XMLException::MissingAttribute);
	return std::string("");
}

Types::BaseType*
Node::_Type(const char* pType) const throw(XMLException) {
	/* search for type name */
	std::string typeName = _StripNamespace(std::string(pType));
	Types::BaseType* pRetType = m_rParser.QueryTypesDb().FindType(typeName.c_str());
	if (typeid(*pRetType) == typeid(Types::Unknown)) {
		Node* pSimpleType	= _FindXSDElm(typeName.c_str(), SimpleType::XSDTag());
		Node* pComplexType	= _FindXSDElm(typeName.c_str(), ComplexType::XSDTag());
		if (NULL != pSimpleType) {
			delete pRetType;
			pRetType = new Types::SimpleType(static_cast<SimpleType*>(pSimpleType));
		} else if (NULL != pComplexType) {
			delete pRetType;
			pRetType=  new Types::ComplexType(static_cast<ComplexType*>(pComplexType));
		} else {
			delete pRetType;
			throw XMLException(m_rXmlElm, XMLException::MissingElement);
		}
	}
	return pRetType;
}

const std::string 
Node::_StripNamespace(const std::string& rQName) const throw(XMLException) {
	std::string xmlNamespace = m_rDocRoot.Namespace();
	/* verify that the namespace is in the qualified name */
	if (0 == rQName.find(xmlNamespace)) {
		if (xmlNamespace.length() > 0) {
			return rQName.substr(xmlNamespace.length() + 1);
		} else {
			return rQName;
		}
	}
	return rQName;
}

Node* 
Node::Parent() const throw(XMLException) {
	const TiXmlElement* pElm = m_rXmlElm.Parent()->ToElement();
	return pElm ? _ConstructNode(pElm, m_rDocRoot, m_rParser) : NULL;
}

Node*
Node::FirstChild() const throw(XMLException) {
	const TiXmlElement* pElm = m_rXmlElm.FirstChildElement();
	return pElm ? _ConstructNode(pElm, m_rDocRoot, m_rParser) : NULL;
}
Node*
Node::NextSibling() const throw(XMLException) {
	const TiXmlElement* pElm = m_rXmlElm.NextSiblingElement();
	return pElm ? _ConstructNode(pElm, m_rDocRoot, m_rParser) : NULL;
}

const TiXmlElement*
Node::ContentElement(const char* pElemName) const throw(XMLException) {
	std::string elementName = QualifyElementName(pElemName);
	const TiXmlElement* pElm = m_rXmlElm.FirstChildElement(elementName.c_str());
	if (!pElm) throw XMLException(m_rXmlElm,XMLException::MissingChildXMLElement);
	return pElm;
}

bool
Node::HasAttribute(const char* pAttrib) const throw() {
	const char* pAttribVal = m_rXmlElm.Attribute(pAttrib);
	return (NULL != pAttribVal && *pAttribVal != 0);
}

bool
Node::HasContent(const char* pElemName) const throw() {
	std::string elementName = QualifyElementName(pElemName);
	return (NULL != m_rXmlElm.FirstChildElement(elementName.c_str()));
}

bool
Node::HasContent() const throw() {
	/* search for a text node */
	const TiXmlNode* pXmlNode = m_rXmlElm.FirstChild();
	for ( ; NULL != pXmlNode; pXmlNode = pXmlNode->NextSibling()) {
		if (TiXmlNode::TINYXML_TEXT == pXmlNode->Type())
			return true;
	}
	return false;
}

bool
Node::IsRootNode() const throw () {
	const TiXmlNode* 	pParent	= m_rXmlElm.Parent();
	const TiXmlElement* pElm 	= (pParent) ? pParent->ToElement() : NULL;
	return pElm && (pElm == &m_rDocRoot.m_rXmlElm);
}

bool
Node::operator == (const Node& elm) const {
	return &m_rXmlElm == &elm.m_rXmlElm;
}

bool
Node::operator == (const Node& elm) {
	return &m_rXmlElm == &elm.m_rXmlElm;
}

std::string 
Node::QualifyElementName(const char* pElemName) const throw() {
	std::string elementName(pElemName);
	std::string xmlNamespace = m_rDocRoot.Namespace();
	/* verify that the namespace is in the qualified name */
	if (0 != elementName.find(xmlNamespace)) {
		return xmlNamespace + ":" + elementName;
	}
	return elementName;
}
