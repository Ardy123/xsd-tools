/*
 * LuaProcessor.cpp
 *
 *  Created on: Jun 29, 2011
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

#include <sstream>
#include <memory>
#include "./src/XSDParser/Parser.hpp"
#include "./src/XSDParser/Elements/Schema.hpp"
#include "./src/XSDParser/Elements/Element.hpp"
#include "./src/XSDParser/Elements/Sequence.hpp"
#include "./src/XSDParser/Elements/Union.hpp"
#include "./src/XSDParser/Elements/Restriction.hpp"
#include "./src/XSDParser/Elements/Choice.hpp"
#include "./src/XSDParser/Elements/Attribute.hpp"
#include "./src/XSDParser/Elements/List.hpp"
#include "./src/XSDParser/Elements/SimpleType.hpp"
#include "./src/XSDParser/Elements/ComplexType.hpp"
#include "./src/XSDParser/Elements/Group.hpp"
#include "./src/XSDParser/Elements/Any.hpp"
#include "./src/XSDParser/Elements/ComplexContent.hpp"
#include "./src/XSDParser/Elements/Extension.hpp"
#include "./src/XSDParser/Elements/SimpleContent.hpp"
#include "./src/XSDParser/Elements/MinExclusive.hpp"
#include "./src/XSDParser/Elements/MaxExclusive.hpp"
#include "./src/XSDParser/Elements/MinInclusive.hpp"
#include "./src/XSDParser/Elements/MaxInclusive.hpp"
#include "./src/XSDParser/Elements/MinLength.hpp"
#include "./src/XSDParser/Elements/MaxLength.hpp"
#include "./src/XSDParser/Elements/FractionDigits.hpp"
#include "./src/XSDParser/Elements/Pattern.hpp"
#include "./src/XSDParser/Elements/TotalDigits.hpp"
#include "./src/XSDParser/Elements/WhiteSpace.hpp"
#include "./src/XSDParser/Elements/AttributeGroup.hpp"
#include "./src/XSDParser/Elements/Include.hpp"
#include "./src/XSDParser/Elements/Annotation.hpp"
#include "./src/XSDParser/Elements/Documentation.hpp"
#include "./src/XSDParser/Elements/All.hpp"
#include "./src/Processors/LuaProcessor.hpp"
#include "./src/Processors/LuaAdapter.hpp"
#include "./src/Processors/SimpleTypeExtracter.hpp"
#include "./src/Processors/ArrayType.hpp"

using namespace std;
using namespace Processors;

LuaProcessor::LuaProcessor(lua_State* pLuaState)
	: LuaProcessorBase("", pLuaState)
{ }

LuaProcessor::LuaProcessor(const LuaProcessor& rProcessor)
	: LuaProcessorBase(rProcessor)
{ }

LuaProcessor::LuaProcessor(const std::string& rLuaTypeName, lua_State* pLuaState)
	: LuaProcessorBase(rLuaTypeName, pLuaState)
{ }
/* virtual */
LuaProcessor::~LuaProcessor() {
}

/* virtual */ void
LuaProcessor::ProcessSchema(const XSD::Elements::Schema* pNode) {
	if (pNode->isRootSchema()) {
		/* create root schema object */
		LuaSchema schema = LuaAdapter(_luaState()).Schema();
		schema.SetName(pNode->Name());
		schema.AddType(pNode->Name());
		/* parse schema children */
		LuaProcessor luaPrcssr(pNode->Name(), _luaState());
		pNode->ParseChildren(luaPrcssr);
	} else
		pNode->ParseChildren(*this);
}

/* virtual */ void
LuaProcessor::ProcessElement(const XSD::Elements::Element* pNode) {
	LuaSchema schema = LuaAdapter(_luaState()).Schema();
	/* don't process element if it can't occur */
	if (pNode->HasMaxOccurs() && (0 == pNode->MaxOccurs()))
		return;
	/* output the element */
	if (!pNode->HasRef()) {
		/* create new type record */
		LuaType newType = schema.AddType(pNode->Name());
		/* add this struct as a dependency list of the parent struct */
		schema.GetType(_luaTypeName()).AddDependent(newType);
		/* process element type */
		auto_ptr<XSD::Types::BaseType> pElmType(pNode->Type());
		LuaProcessor luaPrcssr(pNode->Name(), _luaState());
		luaPrcssr._parseType(*pElmType);
	} else {
		auto_ptr<XSD::Elements::Element> pRefElm(pNode->RefElement());
		/* update the struct's dependency tree */
		LuaType refLuaType = schema.GetType(pRefElm->Name());
		schema.GetType(_luaTypeName()).AddDependent(refLuaType);
	}
}

/* virtual */ void
LuaProcessor::ProcessUnion(const XSD::Elements::Union* pNode) {
	_parseType(XSD::Types::String());
}

/* virtual */ void
LuaProcessor::ProcessRestriction(const XSD::Elements::Restriction* pNode ) { 
	if (pNode->isParentComplexContent() || pNode->isParentSimpleContent())
		pNode->ParseChildren(*this);
	else {
		auto_ptr<XSD::Types::BaseType> pType(pNode->Base());
		_parseType(*pType);
	}
}

/* virtual */ void
LuaProcessor::ProcessList(const XSD::Elements::List* pNode) {
	auto_ptr<XSD::Types::BaseType> pTypeLst(pNode->ItemType());
	/* extract xsd native type from simple type */
	SimpleTypeExtracter typeXtr;
	/* create array type */
	_parseType(Types::ArrayType(typeXtr.Extract(*pTypeLst)));
}

/* virtual */ void
LuaProcessor::ProcessAttribute(const XSD::Elements::Attribute* pNode) {
	if (pNode->HasRef()) {
		auto_ptr<XSD::Elements::Attribute> pRefAtt(pNode->RefAttribute());
		ProcessAttribute(pRefAtt.get());
	} else {
		auto_ptr<XSD::Types::BaseType> pType(pNode->Type());
		/* extract xsd native type from simple type */
		SimpleTypeExtracter typeXtr;
		/* add attribute */
		LuaSchema schema = LuaAdapter(_luaState()).Schema();
		LuaAttribute attrib = schema.GetType(_luaTypeName()).AddAttribute(pNode->Name(), typeXtr.Extract(*pType));
		if (pNode->HasDefault()) {
			attrib.SetDefault(pNode->Default());
		}
	}
}

/* virtual */ void
LuaProcessor::ProcessGroup(const XSD::Elements::Group* pNode) {
	/* only parse groups when they are referred */
	if (pNode->HasRef()) {
		auto_ptr<XSD::Elements::Group> pGroup(pNode->RefGroup());
		pGroup->ParseChildren(*this);
	}
}

/* virtual */ void
LuaProcessor::ProcessAny(const XSD::Elements::Any* pNode) {
	/* an any type can be any element in the schema as a child */
	/* TODO */
	pNode->ParseChildren(*this);
}

/* virtual */ void
LuaProcessor::ProcessExtension(const XSD::Elements::Extension* pNode) {
	auto_ptr<XSD::Types::BaseType> pBase(pNode->Base());
	_parseType(*pBase);
	pNode->ParseChildren(*this);
}

/* virtual */ void
LuaProcessor::ProcessAttributeGroup(const XSD::Elements::AttributeGroup* pNode) {
	/* only parse attribute groups when they are referred */
	if (pNode->HasRef()) {
		auto_ptr<XSD::Elements::AttributeGroup> pGroup(pNode->RefGroup());
		pGroup->ParseChildren(*this);
	}
}

/* virtual */ void
LuaProcessor::ProcessInclude(const XSD::Elements::Include* pNode) {
	const XSD::Elements::Schema * pSchema = pNode->QuerySchema();
	pSchema->ParseChildren(*this);
}

/* virtual */ void
LuaProcessor::_parseType(const XSD::Types::BaseType& rXSDType) {
	if(XSD_ISTYPE(&rXSDType, XSD::Types::SimpleType)) {
		const XSD::Types::SimpleType* pSimpleType = static_cast<const XSD::Types::SimpleType*>(&rXSDType);
		pSimpleType->m_pValue->ParseElement(*this);
	} else if(XSD_ISTYPE(&rXSDType, XSD::Types::ComplexType)) {
		const XSD::Types::ComplexType* pComplexType = static_cast<const XSD::Types::ComplexType*>(&rXSDType);
		pComplexType->m_pValue->ParseElement(*this);
	} else {
		/* handle array types the same as basic types */
		LuaSchema schema = LuaAdapter(_luaState()).Schema();
		schema.GetType(_luaTypeName()).AddContent(rXSDType);
	}
}
