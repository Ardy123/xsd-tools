# Copyright: (c)2012 Ardavon Falls
#
# This file is part of xsd-tools.
#
# xsd-tools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# xsd-tools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

import glob
import difflib
import os
import shutil
import sys
import conio

consoleIO = conio.conio()

def _handleError(stdErr, errCode):
	fail = (errCode and 0 != len(stdErr))
	if fail:
		consoleIO.stdout(consoleIO.FAIL, stdErr)
	return fail

def _extractSchemaName(filename):
	return os.path.splitext(os.path.split(filename)[1])[0]

def genBinding(srcPrefix, xsdfile, rsltPath, template):
	schemaName = _extractSchemaName(xsdfile)
	rsltPath   = rsltPath + schemaName + '-xsdb/'
	# use tool to generate marshalling code
	cmd = '../xsdb ' + template + ' ' + xsdfile + ' | csplit - \'/\/\* FILE: /\' {2}'
	out, err, retCode = consoleIO.call(cmd)
	if _handleError(err, retCode):
		return False
	# move generated code to desired location
	if not os.path.exists(rsltPath):
		os.makedirs(rsltPath)
	os.remove('xx00')
	shutil.move('xx01', rsltPath + srcPrefix + 'common.h')
	shutil.move('xx02', rsltPath + srcPrefix + schemaName +'.h')
	shutil.move('xx03', rsltPath + srcPrefix + schemaName +'.c')
	return True

def genBindingTest(xsdfile, rsltPath, template):
	schemaName = _extractSchemaName(xsdfile)
	rsltTstFile= rsltPath + schemaName + '-bin.c'
	# use tool to generate test code
	cmd ='../xsdb ' + template + ' ' + xsdfile + ' >' + rsltTstFile
	out, err, retCode = consoleIO.call(cmd)
	if _handleError(err, retCode):
		return False
	return True

def genMakefile(xsdfiles, rsltPath):
	# try and open and create makefile
	try:
		makefile = open(rsltPath + 'makefile', "w")
	except IOError, e:
		consoleIO.stdout(consoleIO.FAIL, "\texception: " + e)
		return False;
	# output 'all' section
	makefile.write('all: ')
	for target in xsdfiles:
		target = os.path.splitext(os.path.split(target)[1])[0]
		makefile.write(' ' + target)
	makefile.write('\n\n')
	# output each target section
	for target in xsdfiles:
		target = os.path.splitext(os.path.split(target)[1])[0]
		makefile.write(target + ':\n')
		makefile.write('\t$(MAKE) -f test.mk TEST='+target + '\n')
	makefile.write('\n')
	# output clean section
	makefile.write('clean:\n')
	for target in xsdfiles:
		target = os.path.splitext(os.path.split(target)[1])[0]
		makefile.write('\t$(MAKE) -f test.mk TEST='+target + ' clean\n')
	makefile.write('\n')
	# close file and return
	makefile.close()
	return True
	
def buildTests(rsltPath):
	cmd = 'make'
	consoleIO.stdout(consoleIO.ENDC, 'compiling test cases: ')
	std, err, retcode = consoleIO.call(cmd, rsltPath)
	if _handleError(err, retcode):
		return False
	else:
		consoleIO.stdout(consoleIO.OKGREEN, "Pass\n")
	return True
	
def execTests(xsdfiles, rsltPath):
	for exe in xsdfiles:
		exefile = os.path.splitext(os.path.split(exe)[1])[0]
		consoleIO.stdout(consoleIO.ENDC, "executing test " + exefile + ": ")
		cmd = rsltPath + exefile
		std, err, retcode = consoleIO.call(cmd)
		if _handleError(retcode, err):
			return False
		else:
			consoleIO.stdout(consoleIO.OKGREEN, "Pass\n")
	return True;

def genPreconditionList(preconditionPath):
	return glob.glob(preconditionPath + "*.xsd")
	
def genTestCases(srcPrefix, preconditionPath, postConditionPath, bindingTemplate, testTemplate):
	preconditionFileLst = genPreconditionList(preconditionPath)
	for preconditionFile in preconditionFileLst:
		consoleIO.stdout(consoleIO.ENDC, "generating test code from "+ preconditionFile + ": ")
		if not genBinding(srcPrefix, preconditionFile, postConditionPath, bindingTemplate):
			consoleIO.stdout(consoleIO.FAIL, " Fail\n")
			return False
		if not genBindingTest(preconditionFile, postConditionPath, testTemplate):
			consoleIO.stdout(consoleIO.FAIL, " Fail\n")
			return False
		else:
			consoleIO.stdout(consoleIO.OKGREEN, "Pass\n")
	return True

def cleanTestDir(rsltPath):
	shutil.move(rsltPath + 'test.mk', '/tmp/test.mk')
	shutil.rmtree(rsltPath)
	os.makedirs(rsltPath)
	shutil.move('/tmp/test.mk', rsltPath + 'test.mk')
	
def runTest(srcPrefix, testPath, bindingTemplate, op):
	templateName = os.path.split(bindingTemplate)[1]
	dstPath =  templateName + "/"
	testTemplate = bindingTemplate + "-test"
	# clean test case build directory
	cleanTestDir(dstPath)
	if "clean" == op:
		return
	# aquire list of test cases
	xsdLst = genPreconditionList(testPath)
	# generate tests
	if not genTestCases(srcPrefix, testPath, dstPath, bindingTemplate, testTemplate):
		return
	if not genMakefile(xsdLst, dstPath):
		return
	# make tests
	if not buildTests(dstPath):
		consoleIO.stdout(consoleIO.FAIL, "Fail\n")
		return
	# run tests
	if not execTests(xsdLst, dstPath):
		consoleIO.stdout(consoleIO.FAIL, "Fail\n")
	return