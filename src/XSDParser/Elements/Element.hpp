/*
 * Element.hpp
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

#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <string>
#include <tinyxml.h>
#include "./src/XSDParser/Exception.hpp"
#include "./src/XSDParser/Elements/Node.hpp"

namespace XSD {
	namespace Elements {
		class Element : public Node {
			XSD_ELEMENT_TAG("element")
		private:
			Element();
			Types::BaseType* _Type() const throw(XMLException);
			static Types::BaseType* _ParseType(const Element& rElm) throw(XMLException);
		public:
			Element(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser);
			Element( const Element& elm);
			void ParseChildren(BaseProcessor& rProcessor) const throw(XMLException);
			void ParseElement(BaseProcessor& rProcessor) const throw(XMLException);
			Types::BaseType * GetParentType(void) const throw(XMLException);
			bool Abstract() const;
			std::string Name() const throw(XMLException);
			Element* SubstitutionGroup() const throw(XMLException);
			bool VerifySubstitutionGroup() const throw(XMLException);
			Types::BaseType* Type() const throw(XMLException);
			Element* RefElement() const throw(XMLException);
			int MaxOccurs() const;
			bool HasName() const;
			bool HasSubstitutionGroup() const;
			bool HasRef() const;
			bool HasType() const;
			bool HasChildType() const;
			bool HasMaxOccurs() const;
		};
	}
}
#endif /* ELEMENT_HPP_ */
