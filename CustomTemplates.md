# Custom Templates #

Custom templates can be created to be used by xsd-tools.

### Template Paths ###
Custom templates can be placed anywhere. By default the tools look the the following directories in the following order.
  1. ~/.xsdtools/templates/
  1. /usr/share/xsdtools/templates/

### Template Structure ###
The templates are large blocks of text with blocks of lua code injected between the following delimiters [@lua <lua code goes here> ]. The returned result of the lua blocks are outputted in place of the lua code block when the template is executed.

### Lua Blocks ###
The global environment of the lua blocks includes a table named 'schema'. That table contains the information from the xsd document. Its structure is as follows.
```
schema = {
   root = "<name of root schema>",
   type = { <tables for each element> },
   <element name> = { <table for each element> }
}
```

where each element table is defined as
```
{
    attributes = { <attribute name> = { type = "<xsd type>" ...} },
    content = { type = "<xsd type>" ... },
    dependents = { <child element name> = { <respective element table> }
}
```

here is an example
```
schema = {
    testA002 = {
        name = testA002
        dependents = {
	    myElem = {
		name = myElem
		dependents = {}
		content = {
			type = string
		}
		attributes = {}
	    }
 	    foo = {
		name = foo
		dependents = {}
		content = {
			type = string
		}
		attributes = {}
	    }
        }
        content = {}
        attributes = {}
    },
    myElem = {
       name = myElem
       dependents = {}
       content = {
           type = string
       }
       attributes = {}
    },
    foo = {
        name = foo
        dependents = {}
        content = {
            type = string
        }
        attributes = {}
    },
    root = "testA002"
}
```

### Sample Template ###
```
This is a sample template. The text will be outputted every time and not transformed, unlike the lua block below.
[@lua
local out = stringBuffer:new()
function printTable(t, depth)
	for k,v in pairs(t) do
		if "table" == type(v) then
			if nill == next(v) then out:append(string.rep("\t", depth)..k.." = {}\n")
			else
				out:append(string.rep("\t", depth)..k.." = {\n")
				printTable(v, depth + 1)
				out:append(string.rep("\t", depth).."}\n")
				end
 			else
 				out:append(string.rep("\t", depth)..k.." = "..v.."\n")
			end
		end
	end
function main(template)
	printTable(schema.types[schema.root], 0)
end
main("")
return(out:str())
]
```