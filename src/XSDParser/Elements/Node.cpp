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
	if (boost::iequals(std::string(Attribute::XSDTag()), pElm->ValueStr())) {
		return new Attribute(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Choice::XSDTag()), pElm->ValueStr())) {
		return new Choice(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Element::XSDTag()), pElm->ValueStr())) {
		return new Element(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(List::XSDTag()), pElm->ValueStr())) {
		return new List(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Restriction::XSDTag()), pElm->ValueStr())) {
		return new Restriction(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Sequence::XSDTag()), pElm->ValueStr())) {
		return new Sequence(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Union::XSDTag()), pElm->ValueStr())) {
		return new Union(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Schema::XSDTag()), pElm->ValueStr())) {
		return new Schema(*pElm, rRoot, rParser, std::string(""));
	} else if (boost::iequals(std::string(SimpleType::XSDTag()), pElm->ValueStr())) {
		return new SimpleType(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(ComplexType::XSDTag()), pElm->ValueStr())) {
		return new ComplexType(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Group::XSDTag()), pElm->ValueStr())) {
		return new Group(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Any::XSDTag()), pElm->ValueStr())) {
		return new Any(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(ComplexContent::XSDTag()), pElm->ValueStr())) {
		return new ComplexContent(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Extension::XSDTag()), pElm->ValueStr())) {
		return new Extension(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(SimpleContent::XSDTag()), pElm->ValueStr())) {
		return new SimpleContent(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MinExclusive::XSDTag()), pElm->ValueStr())) {
		return new MinExclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MaxExclusive::XSDTag()), pElm->ValueStr())) {
		return new MaxExclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MinInclusive::XSDTag()), pElm->ValueStr())) {
		return new MinInclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MaxInclusive::XSDTag()), pElm->ValueStr())) {
		return new MaxInclusive(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MinLength::XSDTag()), pElm->ValueStr())) {
		return new MinLength(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(MaxLength::XSDTag()), pElm->ValueStr())) {
		return new MaxLength(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Enumeration::XSDTag()), pElm->ValueStr())) {
		return new Enumeration(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(FractionDigits::XSDTag()), pElm->ValueStr())) {
		return new FractionDigits(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Pattern::XSDTag()), pElm->ValueStr())) {
		return new Pattern(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(TotalDigits::XSDTag()), pElm->ValueStr())) {
		return new TotalDigits(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(WhiteSpace::XSDTag()), pElm->ValueStr())) {
		return new WhiteSpace(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(AttributeGroup::XSDTag()), pElm->ValueStr())) {
		return new AttributeGroup(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Include::XSDTag()), pElm->ValueStr())) {
		return new Include(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Annotation::XSDTag()), pElm->ValueStr())) {
		return new Annotation(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(Documentation::XSDTag()), pElm->ValueStr())) {
		return new Documentation(*pElm, rRoot, rParser);
	} else if (boost::iequals(std::string(All::XSDTag()), pElm->ValueStr())) {
		return new All(*pElm, rRoot, rParser);
	} else
		throw XMLException(*pElm, XMLException::InvallidChildXMLElement);
	return NULL;
}

Node*
Node::_FindXSDElm(const char* pName, const char* pTypeName) const throw(XMLException) {
	/* search root document */
	Node* pRetNode = NULL;
	const TiXmlElement* pElm = m_rDocRoot._FindChildXMLElement(pTypeName, "name", pName);
	/* search all included documents */
	if (!pElm) {
		const TiXmlElement* pIncludElm = m_rDocRoot.m_rXmlElm.FirstChildElement(XSD::Elements::Include::XSDTag());
		for ( ; pIncludElm; pIncludElm = pIncludElm->NextSiblingElement(XSD::Elements::Include::XSDTag()) ) {
			std::auto_ptr<Include> pNode(static_cast<Include*>(_ConstructNode(pIncludElm, m_rDocRoot, m_rParser)));
			const Schema* pSchema = pNode->QuerySchema();
			if (NULL != (pElm = pSchema->_FindChildXMLElement(pTypeName, "name", pName))) {
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
	/* search all nodes in root document */
	const TiXmlElement* pElm = m_rXmlElm.FirstChildElement(pXMLTag);
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
	Types::BaseType* pRetType = m_rParser.QueryTypesDb().FindType(pType);
	if (typeid(*pRetType) == typeid(Types::Unknown)) {
		Node* pSimpleType	= _FindXSDElm(pType, SimpleType::XSDTag());
		Node* pComplexType	= _FindXSDElm(pType, ComplexType::XSDTag());
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
	const TiXmlElement* pElm = m_rXmlElm.FirstChildElement(pElemName);
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
	return (NULL != m_rXmlElm.FirstChildElement(pElemName));
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
