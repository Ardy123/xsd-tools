[@lua
ListItemStrategy = {
   declaration = function(type, var)
		    local fmt = '\tprivate Vector<%s> _%s = new Vector<%s>();\n'
		    return fmt:format(type.typename, var, type.typename)
		 end,
   marshall = function(type, var, tag)
		 local str = {}
		 local fmt = {
		    '\t\t{\n',
		    '\t\t\tJSONArrayAdapter jArray = new JSONArrayAdapter(new JSONarray());\n',
		    '\t\t\tfor(%s obj : _%s) {\n',
		    '\t\t\t\tjArray.%s(obj);\n',
		    '\t\t\tretObj.put(jArray.getJSONArray());\n',
		    '\t\t}\n'
		 }
		 str[1] = fmt[1]
		 str[2] = fmt[2]
		 str[3] = fmt[3]:format(type.typename, var)
		 str[4] = fmt[4]:format(type.marshall)
		 str[5] = fmt[5]
		 str[6] = fmt[6]
		 return table.concat(str)
	      end,
   unmarshall = function(type, var, tag)
		   local str = {}
		   if type.metaInfo['primative'] then
			local fmt = {
				'\t\t{\n',
				'\t\t\tJSONArrayAdapter jArray = jObj.getList(\"%s\");\n',
				'\t\t\tfor (int i = 0; i < jArray.length(); ++i) {\n',
				'\t\t\t\t_%s.add(jArray.%s(ndx));\n',
				'\t\t}\n'
			}
			str[1] = fmt[1]
			str[2] = fmt[2]:format(tag)
			str[3] = fmt[3]
			str[4] = fmt[4]:format(var, type.unmarshall)
			str[5] = fmt[5]
		   else
			local fmt = {
				'\t\t{\n',
				'\t\t\tJSONArrayAdapter jArray = jObj.getList(\"%s\");\n',
				'\t\t\tfor (int i = 0; i < jArray.length(); ++i) {\n',
				'\t\t\t\t_%s.add(new %s(jArray.%s(ndx)));\n',
				'\t\t}\n'
			}
			str[1] = fmt[1]
			str[2] = fmt[2]:format(tag)
			str[3] = fmt[3]
			str[4] = fmt[4]:format(var, type.typename, type.unmarshall)
			str[5] = fmt[5]
		   end
		   return table.concat(str)
		end,
   geter = function(type, var)
	      local str = {}
	      local fmt = {
		 '\tpublic Vector<%s> get%s() {\n',
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
		 '\tpublic void set%s(Vector<%s> %s) {\n',
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