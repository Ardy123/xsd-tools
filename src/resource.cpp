/*
 * resource.cpp
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
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#ifdef __APPLE__
	#include <mach-o/dyld.h>
	#include <mach-o/getsect.h>
#endif /* __APPLE__ */
#include "src/resource.hpp"

using namespace std;
using namespace Core;

#ifdef __APPLE__
	static size_t _binary_luascript_luac_size;
	static char * _binary_luascript_luac = 
		getsectdata("__DATA", "__luascript_luac", &_binary_luascript_luac_size)  
		+ _dyld_get_image_vmaddr_slide(0); 
#else /* __APPLE__ */
	extern "C" {
    	extern char _binary_luascript_luac_start;
    	extern char _binary_luascript_luac_end;
	}
	static char * _binary_luascript_luac = &_binary_luascript_luac_start;
	static size_t _binary_luascript_luac_size = &_binary_luascript_luac_end - 
												&_binary_luascript_luac_start;
#endif /* __APPLE__ */

static const string gscHOMEPATH("~/.xsdtools/templates/");
static const string gscGLOBALPATH("/usr/share/xsdtools/templates/");

ResourceException::ResourceException(const string& rMsg)
	: m_errorMsg(rMsg)
{ }

/* virtual */
ResourceException::~ResourceException() throw ()
{ }

/* virtual */ const char*
ResourceException::what() const throw() {
	return m_errorMsg.c_str();
}

Resource::Resource() 
{ }

/* virtual */
Resource::~Resource() throw()
{ }

const uint8_t *
Resource::GetEngineScript(size_t* pRetSz) throw() {
	*pRetSz = _binary_luascript_luac_size;
	return reinterpret_cast<uint8_t*> (_binary_luascript_luac);
}

string
Resource::GetTemplatePath(const std::string& templateName) throw(ResourceException) {
	/* test if the template name exists */
	if (0 == access(templateName.c_str(), R_OK))
		return templateName;
	/* check the home directory for template file */
	struct passwd* pw = getpwuid(getuid());
	string homeFilePath(pw->pw_dir);
	homeFilePath += gscHOMEPATH.substr(1) + templateName;
	if (0 == access(homeFilePath.c_str(), R_OK))
		return homeFilePath;
	/* check the global directory for template file */
	string globalFilePath(gscGLOBALPATH);
	globalFilePath += templateName;
	if (0 == access(globalFilePath.c_str(), R_OK))
		return globalFilePath;
	else {
		string errorMessage("Could not open template ");
		throw ResourceException(errorMessage + templateName);
	}
	return std::string("");
}
