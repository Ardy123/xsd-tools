/*
 * Choice.hpp
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

#ifndef CHOICE_HPP_
#define CHOICE_HPP_
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <tinyxml.h>
#include "./src/XSDParser/Elements/Node.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"

namespace XSD {
	namespace Elements {
		class Choice : public Node {
			XSD_ELEMENT_TAG("choice")
		private:
			Choice();
		public:
			Choice(const TiXmlElement& elm, const Schema& docRoot, const Parser& rParser);
			Choice(const Choice& rCpy);
			void ParseChildren(BaseProcessor& rProcessor) const throw(XMLException);
			void ParseElement(BaseProcessor& rProcessor) const throw(XMLException);
			Types::BaseType * GetParentType() const throw(XMLException);
			int MaxOccurs() const;
			int MinOccurs() const;
			bool HasElements() const throw(XMLException);
			bool HasSequence() const throw(XMLException);
			bool HasMaxOccurs() const;
			bool HasMinOccurs() const;
		};
	}	/* namespace Elements */
}	/* namespace XSD */
#endif /* CHOICE_HPP_ */
