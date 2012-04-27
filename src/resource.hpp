/*
 * resource.hpp
 *
 *  Created on: 01/29/12
 *      Author: Ardavon Falls
 *   Copyright: (c)2012 Ardavon Falls
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
#ifndef RESOURCE_HPP_
#define RESOURCE_HPP_

#include <stdint.h>
#include <string>

namespace Core {
	class ResourceException : public std::exception {
	public:
		ResourceException(const std::string& rMsg);
		virtual ~ResourceException() throw ();
		virtual const char* what() const throw();
	private:
		std::string	m_errorMsg;
	};
	class Resource {
	public:
		Resource();
		virtual ~Resource() throw();
		const uint8_t* GetEngineScript(size_t* pRetSz) throw();
		std::string GetTemplatePath(const std::string& templateName) throw(ResourceException);
	};
}
#endif /* RESOURCE_HPP_ */
