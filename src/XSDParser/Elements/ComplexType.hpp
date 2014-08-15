/*
 * ComplexType.hpp
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

#ifndef COMPLEXTYPE_HPP_
#define COMPLEXTYPE_HPP_
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <tinyxml.h>
#include "./src/XSDParser/Exception.hpp"
#include "./src/XSDParser/Elements/Node.hpp"
#include "./src/XSDParser/Elements/Attribute.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Choice.hpp"

namespace XSD {
	namespace Elements {
		class ComplexType : public Node {
			XSD_ELEMENT_TAG("complexType")
		private:
			ComplexType();
		public:
			ComplexType(const TiXmlElement& elm, const Parser& rParser);
			ComplexType(const ComplexType& rType);
			void ParseChildren(BaseProcessor& rProcessor) const throw(XMLException);
			void ParseElement(BaseProcessor& rProcessor) const throw(XMLException);
			Types::BaseType * GetParentType(void) const throw(XMLException);
			bool Abstract() const;
			bool Mixed() const;
			std::string Name() const throw(XMLException);
			bool HasSequence() const;
			bool HasAttribute() const;
			bool HasChoice() const;
			bool HasName() const;
		};
	}	/* namespace Elements */
}	/* namespace XSD */

#endif /* COMPLEXTYPE_HPP_ */
