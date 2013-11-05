[@lua
ItemStrategy = {
   declaration = function(type, var)
			if type.metaInfo['primative'] then
			  local fmt = '\tprivate %s _%s;\n'
			  return fmt:format(type.typename, var)
			else
			  local fmt = '\tprivate %s _%s = new %s();\n'
			  return fmt:format(type.typename, var, type.typename)
			end
		 end,
   marshall = function(type, var, tag)
		 local fmt = '\t\tretObj.%s(\"%s\", _%s);\n'
		 return fmt:format(type.marshall, tag, var)
	      end,
   unmarshall = function(type, var, tag)
		   local fmt = '\t\t_%s= jObj.%s(\"%s\");\n'
		   return fmt:format(var, type.unmarshall, tag)
		end,
   geter = function(type, var)
	      local str = {}
	      local fmt = {
		 '\tpublic %s get%s() {\n',
		 '\t\treturn _%s;\n',
		 '\t}\n'
	      }
	      str[1] = fmt[1]:format(type.typename, var)
	      str[2] = fmt[2]:format(var)
	      str[3] = fmt[3]
	      return table.concat(str)
	   end,
   seter = function(type, var)
	      local str = {}
	      local fmt = {
		 '\tpublic void set%s(%s %s) {\n',
		 '\t\t_%s = %s;\n',
		 '\t}\n'
	      }
	      str[1] = fmt[1]:format(var, type.typename, var)
	      str[2] = fmt[2]:format(var, var)
	      str[3] = fmt[3]
	      return table.concat(str)	      
	   end
}
]