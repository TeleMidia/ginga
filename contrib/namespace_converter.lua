--This script joins all namespaces in one name only with the structure of: name_BEGIN *CONTENT* name_END

local input = io.open(arg[1], "r+")
local s = input:read("*a")
local u = ''
local t= ''
local j
local output
function func (s)
return s:gsub ('[^%a_]namespace (%a+) (%b{})%s',
		function(name,cap)
			u = '_' .. name ..u 
			t = t .. name .. '_'
			if cap:match('^{%s*namespace') then			
				return name:upper()..'_'..func (cap:sub (2,-2))..'_'..name:upper()
			else
				return name:upper()..'_'..cap:sub (2,-2)..'_'..name:upper()
			end
			end
			)
end

output = func(s)
u= u:upper()
t= t:upper()
output = output:gsub(u,t)
output = output:gsub(t,t..'END')
j=t..'END'
output = output:gsub(j,t..'BEGIN',1)

input = io.open(arg[1], "w+")
input:write(output)
input:close()
