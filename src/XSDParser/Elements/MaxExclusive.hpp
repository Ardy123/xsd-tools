/*
 * MaxExclusive.hpp
 *
 *  Created on: Jul 29, 2011
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

#ifndef MAXEXCLUSIVE_HPP_
#define MAXEXCLUSIVE_HPP_
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Node.hpp"
namespace XSD {
	namespace Elements {
		class MaxExclusive : public Node {
			XSD_ELEMENT_TAG("maxExclusive")
		private:
			MaxExclusive();
		public:
			MaxExclusive(const TiXmlElement& elm, const Parser& rParser);
			MaxExclusive(const MaxExclusive& cpy);
			void ParseChildren(BaseProcessor& rProcessor) const throw(XMLException);
			void ParseElement(BaseProcessor& rProcessor) const throw(XMLException);
			Types::BaseType * GetParentType() const throw(XMLException);
			long double Value() const throw(XMLException);
			bool HasValue() const;
		};
	}	/* namespace Elements */
}	/* namespace XSD */

#endif /* MAXEXCLUSIVE_HPP_ */
