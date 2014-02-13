#!/bin/sh
tar -czvf SOURCES/xsd-tools.tgz -C ../../.. xsd-tools/SConstruct xsd-tools/templates xsd-tools/site_scons xsd-tools/src
rpmbuild -v -bb --clean SPECS/xsd-tools.spec