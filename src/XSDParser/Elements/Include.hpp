/*
 * Include.hpp
 *
 *  Created on: Aug 10, 2011
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

#ifndef INCLUDE_HPP_
#define INCLUDE_HPP_
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif /* TIXML_USE_STL */
#include <string>
#include <tinyxml.h>
#include "./src/XSDParser/Exception.hpp"
#include "./src/XSDParser/Elements/Node.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"

namespace XSD {
	namespace Elements {
		class Include : public Node {
			XSD_ELEMENT_TAG("include")
		private:
			mutable Schema*	m_pSchema;
			Parser	m_parser;
			Include();
			std::string _schemaURI() const throw(XMLException);
			static std::string _extractURIPath(const std::string& uri);
			static bool _isFileURI(const std::string& uri);
			static std::string _extractQuery(const std::string& uri);
		public:
			Include(const TiXmlElement& elm, const Parser& rParser);
			Include(const Include& elm);
			virtual ~Include();
			void ParseChildren(BaseProcessor& rProcessor) const throw(XMLException);
			void ParseElement(BaseProcessor& rProcessor) const throw(XMLException);
			Types::BaseType * GetParentType() const throw(XMLException);
			const Schema* QuerySchema() const throw(XMLException);
			bool HasSchema() const;
		};
	}	/* namespace Elements */
}	/* namespace XSD */

#endif /* INCLUDE_HPP_ */
