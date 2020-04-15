=================================================
                  xsdtools
=================================================
This software is GPLv3. Please look at LICENSE.txt
for more information. Please note that xsd-tools is 
distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE.  See the GNU General Public 
License for more details.

xsdtools (c)2012 Ardavon Falls

Requirments
 - lua5.1
 - liblua5.1-0-dev
 - libtinyxml-dev
 - libboost-dev
 - libboost-system-dev
 - libboost-filesystem-dev
 - libexpat1-dev
 - maven
 - python
 - scons

Instrucitons to build
1) from a prompt type
    scons

2) to install type
    sudo scons install

3) to uninstall type
    sudo scons uninstall

Information regarding templates
 * default templates are installed to /usr/share/xsdtools/template/
 * user defined templates should be stored at ~/.xsdtools/templates/
 * user directory is searched first for a template before the searching the default directory
