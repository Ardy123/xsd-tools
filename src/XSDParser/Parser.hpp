/*
 * Parser.hpp
 *
 *  Created on: Jun 27, 2011
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
#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include <vector>
#include "./src/XSDParser/TypesDB.hpp"
#include "./src/XSDParser/Exception.hpp"

namespace XSD {
	namespace Elements {
		class Schema;
	}
	class Parser {
	private:
		typedef std::vector<TiXmlDocument *> XmlDocList;
		Types::TypesDB 		m_typesDb;
	    mutable XmlDocList	m_docLst;
	public:
		Parser();
		virtual ~Parser();
		Elements::Schema* Parse(const char* uri) const throw(XMLException);
		Elements::Schema* Parse(const std::string& rURI) const throw(XMLException);
		const Types::TypesDB& QueryTypesDb() const throw();
	};
}	/* namespace XSD */

#endif /* PARSER_HPP_ */
