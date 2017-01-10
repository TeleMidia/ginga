-- Execute this script and the name of the file wanted to update. 
-- WARNING: Backup the original before using this.

local input = io.open(arg[1], "r+")
local s = input:read("*a")
local output
   = s:gsub ('[^%a_]namespace br (%b{})',
                 function (cap)
                    return (cap:sub (2,-2)):gsub ('[^%a_]namespace pucrio (%b{})',
                 function (cap)
                    return (cap:sub (2,-2)):gsub ('[^%a_]namespace telemidia (%b{})',
                 function (cap)
                    return cap:sub (2,-2) -- remove captured braces
                 end
                ) -- remove captured braces
                 end
                ) -- remove captured braces
                 end
                )
print (output)
input = io.open(arg[1], "w+")
input:write(output)
io.close(input)
