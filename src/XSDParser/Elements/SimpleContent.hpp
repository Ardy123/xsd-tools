/*
. * SimpleContent.hpp
 *
 *  Created on: Jul 22, 2011
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

#ifndef SIMPLECONTENT_HPP_
#define SIMPLECONTENT_HPP_
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <tinyxml.h>
#include "./src/XSDParser/Exception.hpp"
#include "./src/XSDParser/Elements/Node.hpp"
#include "./src/XSDParser/Elements/Restriction.hpp"

namespace XSD {
	namespace Elements {
		class SimpleContent : public Node {
			XSD_ELEMENT_TAG("simpleContent")
		private:
			SimpleContent();
		public:
			SimpleContent(const TiXmlElement& elm, const Schema& rRoot, const Parser& rParser);
			SimpleContent(const SimpleContent& rType);
			void ParseChildren(BaseProcessor& rProcessor) const throw(XMLException);
			void ParseElement(BaseProcessor& rProcessor) const throw(XMLException);
			bool isTypeRelated(const Types::BaseType* pType) const throw(XMLException);
		};
	}	/* namespace Elements */
}	/* namespace XSD */

#endif /* SIMPLECONTENT_HPP_ */
