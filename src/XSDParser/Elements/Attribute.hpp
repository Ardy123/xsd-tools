/*
 * Attribute.hpp
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

#ifndef ATTRIBUTE_HPP_
#define ATTRIBUTE_HPP_
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <string.h>
#include <tinyxml.h>
#include "./src/XSDParser/Exception.hpp"
#include "./src/XSDParser/Elements/Node.hpp"

namespace XSD {
	namespace Elements {
		class Attribute : public Node {
			XSD_ELEMENT_TAG("attribute")
		private:
			Attribute();
			Types::BaseType* _type() const noexcept(false);;
			static Types::BaseType* _parseType(const Attribute& rAttrib) noexcept(false);;
		public:
			typedef enum {
				OPTIONAL,
				PROHIBITIED,
				REQUIRED
			} AttributeUse;
			Attribute(const TiXmlElement& elm, const Parser& rParser);
			Attribute(const Attribute& rAttrib);
			void ParseChildren(BaseProcessor& rProcessor) const noexcept(false);;
			void ParseElement(BaseProcessor& rProcessor) const noexcept(false);;
			Types::BaseType * GetParentType() const noexcept(false);;
			std::string Name() const noexcept(false);;
			Attribute* RefAttribute() const noexcept(false);;
			Types::BaseType* Type() const noexcept(false);;
			std::string Default() const noexcept(false);;
			std::string Fixed() const noexcept(false);;
			AttributeUse Use() const noexcept(false);;
			bool HasName() const;
			bool HasRef() const;
			bool HasType() const;
			bool HasDefault() const;
			bool HasFixed() const;
			bool HasUse() const;
		};
	}	/* namespace Elements */
} /* namespace XSD */

#endif /* ATTRIBUTE_HPP_ */
