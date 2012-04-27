/*
 * Node.hpp
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

#ifndef NODE_HPP_
#define NODE_HPP_
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <typeinfo>
#include <memory>
#include <boost/algorithm/string/predicate.hpp>
#include <tinyxml.h>
#include "./src/XSDParser/Exception.hpp"
#include "./src/XSDParser/Types.hpp"
#include "./src/XSDParser/Parser.hpp"
#include "./src/XSDParser/ProcessorBase.hpp"

#define XSD_ISELEMENT(TYPE_PTR,TYPE)	typeid(*TYPE_PTR) == typeid(TYPE)
#define XSD_ELEMENT_TAG(NAME)			public: static const char* XSDTag() throw() { return NAME; }

namespace XSD { namespace Elements { class Schema; } }

namespace XSD {
	namespace Elements {
		class Node {
		private:
			Node();
			const TiXmlElement* ContentElement(const char* pElemName) const throw(XMLException);
			const TiXmlElement* _FindChildXMLElement(const char* pXMLElmTag, const char* pAttrib, const char* pName) const throw(XMLException);
			Node* _FindXSDElm(const char* pName, const char* pTypeName) const throw(XMLException);
			Node* _ConstructNode(const TiXmlElement* pElm, const Schema& rRoot, const Parser& rParser) const;
			Node* _FindXSDNode(const char* pName, const char* pTypeName) const throw(XMLException);
			Node* _FindChildXSDNode(const char* pXMLTag) const throw(XMLException);
			Node* _FindXSDRef(const char* pRefAttribStr, const char* pTypeName) const throw (XMLException);
			std::string _Attribute(const char* pAttrib) const throw (XMLException);
			Types::BaseType* _Type(const char* pType) const throw(XMLException);
		protected:
			const TiXmlElement&		m_rXmlElm;
			const Schema&			m_rDocRoot;
			const Parser&			m_rParser;
			Node(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser);
			Node(const Node& rCpy);
			Types::BaseType* LookupType(const char* pType) const throw(XMLException) { return _Type(pType); }
			Node* FirstChild() const throw(XMLException);
			bool HasAttribute(const char* pAttrib) const throw();
			bool HasContent(const char* pElemName) const throw();
			bool HasContent() const throw();
			bool IsRootNode() const throw ();
			template<typename T> T GetAttribute(const char* pAttrib) const throw(XMLException) {
				T retVal;
				std::stringstream sstrm(_Attribute(pAttrib));
				sstrm >> retVal;
				return retVal;
			}
			template<typename T> T* FindXSDElm(const char* pName) const throw(XMLException) {
				return static_cast<T*>(_FindXSDNode(pName, T::XSDTag()));
			}
			template<typename T> T* FindXSDRef(const char* pRefAttribStr) const throw (XMLException) {
				return static_cast<T*>(_FindXSDRef(pRefAttribStr, T::XSDTag()));
			}
			template<typename T> T* FindXSDChildElm() const throw(XMLException) {
				return static_cast<T*>(_FindChildXSDNode(T::XSDTag()));
			}
		public:
			virtual ~Node() {}
			virtual void ParseChildren(BaseProcessor& rProcessor) const throw(XMLException) = 0;
			virtual void ParseElement(BaseProcessor& rProcessor) const throw(XMLException) = 0;
			virtual bool isTypeRelated(const Types::BaseType* pType) const throw(XMLException) = 0;
			bool operator == (const Node& elm) const;
			bool operator == (const Node& elm);
			inline const TiXmlElement& GetXMLElm() const { return m_rXmlElm;}
			Node* NextSibling() const throw(XMLException);
		};
		template<> bool Node::GetAttribute<bool>(const char* pAttrib) const throw(XMLException);
		template<> const char* Node::GetAttribute<const char*>(const char* pAttrib) const throw (XMLException);
		template<> Types::BaseType* Node::GetAttribute<Types::BaseType*>(const char* pAttrib) const throw (XMLException);
	}	/* namespace Elements */
}	/* namespace XSD */

//#include "./src/XSDParser/Elements/Node.inc"
#endif /* NODE_HPP_ */
