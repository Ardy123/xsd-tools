[@lua
include 'java-json.org/itemstrategy.lua'
include 'java-json.org/listitemstrategy.lua'
include 'java-json.org/types.lua'

local function isEmpty(list)
   return (next(list) == nil)
end

local function isMoreThanOne(list)
   return (next(list, next(list)) ~= nil)
end

local function isSimpleType(type) 
   return ((next(type.attributes) or next(type.content)) == nil)
end

local function isRootElement(type)
  for _, v in pairs(schema) do
	if v == type then return True end
  end  
  return False;
end

local function isCollapsibleType(type)
   return (isRootElement(type) and 
	   isEmpty(type.attributes) and 
	   not isMoreThanOne(type.content))
end

local function collapseType(type)
   return next(type.content)
end

local function isListType(typeName)
   return (typeName:match("^(list)%(.+%)") ~= nil)
end

local function listType(typeName)
   return typeName:match("list%((.+)%)")
end

local function isBaseType(typeName)
  typeName = isListType(typeName) and listType(typeName) or typeName
  return (rawget(types, typeName) ~= nil)
end

function elementParser(name, XSDElement) 
   local str = stringBuffer:new()
   -- if type is collapsible, don't output it or if it is a base type or the schema root
   if isBaseType(name) or isCollapsibleType(XSDElement) or name == '' then
	 return ''
   end
   -- generate class definition
   local fmt = {
      '/* command to split output: csplit - \'/\/\* FILE: /\' {2} */\n',
      '/* FILE: %s.java */\n',
      '/* This file was generated by xsdb */\n',
      'package com.mobitv.generated.json;\n',
      'import java.util.Vector;\n',
      'import org.json.JSONObject;\n',
      'import org.json.JSONArray;\n',
      'import org.json.JSONException;\n',
      'import android.util.Base64;\n',
      'import com.mobitv.generated.json.Marshallable;\n\n'
   }
   str:append(fmt[1])
   str:append(fmt[2]:format(name))
   str:append(fmt[3])
   str:append(fmt[4])
   str:append(fmt[5])
   str:append(fmt[6])
   str:append(fmt[7])
   str:append(fmt[8])
   str:append(fmt[9])
   local fmt = 'public class %s implements Marshalable {\n'
   str:append(fmt:format(name))
   -- generate private members from attributes
   for name, attribute in pairs(XSDElement.attributes) do
      local attribTypeName, attribTypeDef = next(attribute, nil)
      str:append(ItemStrategy.declaration(types[attribTypeName], name))
   end
   -- generate private memebers from content types
   for name, typedef in pairs(XSDElement.content) do
      if isSimpleType(typedef) then
	 if isListType(name) then
	    local lstTypeName = listType(name)
	    str:append(ListItemStrategy.declaration(types[lstTypeName], 'value'))
	 else
	    str:append(ItemStrategy.declaration(types[name], 'value'))
	 end
      elseif isCollapsibleType(typedef) then
		local typeName, collapsedType = collapseType(typedef)
		str:append(ListItemStrategy.declaration(types[typeName], name))
      else
		str:append(ListItemStrategy.declaration(types[name], name))
      end
   end
   -- generate default constructor
   local fmt = '\n\tpublic %s() {\n\t}\n\n'
   str:append(fmt:format(name))
   -- generate unmarshall constructor
   local fmt = {
      '\tpublic %s(JSONObject jObj) throws JSONException {\n',
      '\t}\n\n'
   }
   str:append(fmt[1]:format(name))
   -- unmarshall attributes in constructor
   for name, attribute in pairs(XSDElement.attributes) do
      attribType = next(attribute)		      
      str:append(ItemStrategy.unmarshall(types[attribType], name, '@'..name))
   end
   -- unmarshall content in constructor
   for ctntType, ctntTypeDef in pairs(XSDElement.content) do
      if isSimpleType(ctntTypeDef) then
		if isListType(ctntType) then
		  local lstTypeName = listType(ctntType)
		  str:append(ListItemStrategy.unmarshall(types[lstTypeName],'value', 'value'))
		else
		  str:append(ItemStrategy.unmarshall(types[ctntType], 'value', 'value'))
		end
	  elseif isCollapsibleType(ctntTypeDef) then
		local colTypeName = collapseType(ctntTypeDef)
		str:append(ListItemStrategy.unmarshall(types[colTypeName], ctntType, ctntType))
	  else
		str:append(ListItemStrategy.unmarshall(types[ctntType], ctntType, ctntType))
      end
   end
   str:append('\t}\n\n')
   -- generate 'set'ers from attributes
   for name, attribute in pairs(XSDElement.attributes) do
      attribType = next(attribute)
      str:append(ItemStrategy.seter(types[attribType], name))
   end
   -- generate 'set'ers from content
   for name, typedef in pairs(XSDElement.content) do
	 if isSimpleType(typedef) then
	   if isListType(name) then
		 local lstTypeName = listType(name)
		 str:append(ListItemStrategy.seter(types[lstTypeName], 'value'))
	   else
		 str:append(ItemStrategy.seter(types[name], 'value'))
	   end
	 elseif isCollapsibleType(typedef) then
	   local colTypeName, colType = collapseType(typedef)
	   str:append(ListItemStrategy.seter(types[colTypeName], name))
	 else
	   str:append(ListItemStrategy.seter(types[name], name))
      end
   end
   -- generate 'get'ers from attributes
   for name, attribute in pairs(XSDElement.attributes) do
      attribName, attribType = next(attribute)
      str:append(ItemStrategy.geter(types[attribName], name))
   end
   -- generate 'get'ers from content
   for name, typedef in pairs(XSDElement.content) do
	 if isSimpleType(typedef) then
	   if isListType(name) then
		 local lstTypeName = listType(name)
		 str:append(ListItemStrategy.geter(types[lstTypeName], 'value'))
	   else
		 str:append(ListItemStrategy.geter(types[name], 'value'))
	   end
	 elseif isCollapsibleType(typedef) then
	   local colTypeName, colType = collapseType(typedef)
	   str:append(ListItemStrategy.geter(types[colTypeName], name))
	 else
	   str:append(ListItemStrategy.geter(types[name], name))
	 end
   end
   -- generate marshall funciton
   str:append('\tpublic JSONObject marshall() throws JSONException {\n')
   str:append('\t\tJSONObject retObj = new JSONObject();\n')
   -- marshall attributes
   for name, attribute in pairs(XSDElement.attributes) do
      local attribTypeName, attribTypeDef = next(attribute, nil)
      str:append(ItemStrategy.marshall(types[attribTypeName], name, '@'..name))
   end
   -- marshall content
   for ctntType, ctntTypeDef in pairs(XSDElement.content) do
	 if isSimpleType(ctntTypeDef) then
	   if isListType(ctntType) then
		 local lstTypeName = listType(ctntType)
		 str:append(ListItemStrategy.marshall(types[lstTypeName], 'value', 'value'))
	   else
		 str:append(ItemStrategy.marshall(types[ctntType], 'value', 'value'))
	   end
	 elseif isCollapsibleType(ctntTypeDef) then
	   local colTypeName = collapseType(ctntTypeDef)
	   str:append(ListItemStrategy.marshall(types[colTypeName], ctntType, ctntType))
	 else
	   str:append(ListItemStrategy.marshall(types[ctntType], ctntType, ctntType))
	 end
   end
   str:append('\t\treturn retObj;\n')
   str:append('\t}\n')
   -- add end brace
   str:append('}\n')
   return str:str()
end
]