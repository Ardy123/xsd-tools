# xsd-tools
![ubuntu latest x86_64 workflow](https://github.com/QVXLabs/xsd-tools/actions/workflows/cmake.yml/badge.svg)

## Overview ##
xsd-tools is a set of tools for generating code from xml xsd schema documents, mainly around generating marshalling & unmarshalling code. It is designed such that it can be easily extended by any user to enable code generation for any language. By default it has a support for a simple python-sax marshalling/unmarshalling code generator as well as a C based expat marshalling/unmarshalling code generator.

It processes XSD schema documents and invokes a template file which outputs code. The templates files use Lua for scripting withing the template file. Custom user templates can be easily be created to extend the tool to generate different output code.

## Features ##
  * XSD schema parsing
  * Easily Extendable
  * Simple Lua based templates for customizing code generation.
  * Open Source!

_Look at the Wiki section for more information._

NOTE: Currently the tools are not namespace aware.

## Sample Output ##
```
# xsdb python-sax test/xsd-positive/testA002.xsd


import cStringIO
import xml.sax

class _handler(xml.sax.ContentHandler):
        _elemTbl = {
                'myElem':lambda: xml_myElem(),
                'foo':lambda: xml_foo(),
        }
        def __init__(self):
                xml.sax.ContentHandler.__init__(self)
                return
        # methods inherited from xml.sax.ContentHandler
        def startDocument(self):
                self._elemStk = [("testA021", {}, [])]
                return
        def startElementNS(self, name, qname, SAXAttributes):
                element = _handler._elemTbl[name[1]]()
                elementRecord = (element, self._convertToDictionary(SAXAttributes),  [])
                self._elemStk[-1][2].append(element)    
                self._elemStk.append(elementRecord)        
                return
        def characters(self, content): 
                if self._elemStk[-1][2] and isinstance(self._elemStk[-1][2][-1], basestring):
                        self._elemStk[-1][2][-1] = self._elemStk[-1][2][-1] + content;
                else:
                        self._elemStk[-1][2].append(content)
                return
        def endElementNS(self, name, qname):
                elementRecord = self._elemStk.pop()
                elementRecord[0].unmarshall(elementRecord[1], elementRecord[2])
                return
        @staticmethod
        def _convertToDictionary(SAXAttributes):
                return {k: SAXAttributes.getValueByQName(k) for k in SAXAttributes.getQNames() }

class _xmlelement:
        def __init__(self):
                self._content = []
                return
        def getContent(self):
                return self._content

class xml_myElem(_xmlelement):
        def __init__(self):
                _xmlelement.__init__(self)
                return
        def marshall(self, stream):
                stream.write('<myElem')
                stream.write('>')
                for node in self.getContent():
                        if isinstance(node, _xmlelement):
                                node.marshall(stream)
                        else:
                                stream.write(str(node))
                stream.write('</myElem>')
                return
        def unmarshall(self, attributes, content):
                self._content = content
                return

class xml_foo(_xmlelement):
        def __init__(self):
                _xmlelement.__init__(self)
                return
        def marshall(self, stream):
                stream.write('<foo')
                stream.write('>')
                for node in self.getContent():
                        if isinstance(node, _xmlelement):
                                node.marshall(stream)
                        else:
                                stream.write(str(node))
                stream.write('</foo>')
                return
        def unmarshall(self, attributes, content):
                self._content = content
                return



class xml_testA002(_handler, _xmlelement):
        def __init__(self):
                _handler.__init__(self)
                _xmlelement.__init__(self)
                return
        def marshall(self, stream):
                stream.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n")
                for elm in self.getContent():
                        elm.marshall(stream)
                return
        def unmarshall(self, stream):
                parser = xml.sax.make_parser()
                parser.setFeature(xml.sax.handler.feature_namespaces, 1)
                parser.setContentHandler(self)
                parser.parse(stream)
                self._content = self._elemStk[-1][2]
                return
```
## Usage Guide ##

To use xsd-tools, invoke xsdb using the following syntax from the console
<br><br>
xsdb <template file> <xsd schema file><br>
<br>
<h3>example</h3>
<pre><code># xsdb python-sax testA022.xsd <br>
</code></pre>

## Install Instructions ##

### Requirements ###
The following packages/libs must be available on your system.
  * liblua5.1-0-dev
  * libtinyxml-dev
  * libboost-dev
  * libboost-system-dev
  * libboost-filesystem-dev
  * python
  * scons

On ubuntu/kubuntu systems this can be done by running the following line from a shell.
```
   # sudo apt-get install lua5.1 liblua5.1-0-dev libtinyxml-dev libboost-dev  libboost-system-dev libboost-filesystem-dev scons
```
### Building ###
from a prompt type
```
   # scons
```
### Installing ###
from a prompt type
```
   # sudo scons install
```
### Uninstalling ###
from a prompt type
```
   # sudo scons uninstall
```
