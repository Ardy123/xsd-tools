/*
 * Length.hpp
 *
 *  Created on: Feb 21, 2014
 *      Author: Ardavon Falls
 *   Copyright: (c)2014 Ardavon Falls
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

#ifndef LENGTH_HPP_
#define LENGTH_HPP_

#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <tinyxml.h>
#include <stdint.h>
#include "./src/XSDParser/Elements/Node.hpp"
namespace XSD {
	namespace Elements {
		class Length : public Node {
			XSD_ELEMENT_TAG("length")
		private:
			Length();
		public:
			Length(const TiXmlElement& elm, const Parser& rParser);
			Length(const Length& cpy);
			void ParseChildren(BaseProcessor& rProcessor) const noexcept(false);;
			void ParseElement(BaseProcessor& rProcessor) const noexcept(false);;
			Types::BaseType * GetParentType() const noexcept(false);;
			int64_t Value() const noexcept(false);;
			bool HasValue() const;
		};
	}	/* namespace Elements */
}	/* namespace XSD */

#endif /* LENGTH_HPP_ */
