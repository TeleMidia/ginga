#!/usr/bin/env lua

local function read (file)
   local fp = assert (io.open (file, 'r'))
   local s = fp:read'*a'
   fp:close ()
   return s
end

local function write (file, s)
   local fp = assert (io.open (file, 'w'))
   fp:write (s)
   fp:close ()
end

local function fix (s)
   local tbegin, tend = '', ''
   local dofix
   dofix = function (s)
      return s:gsub
      ('[^%a]namespace%s+(%a+)%s+(%b{})%s*',
       function (ns, cap)
          local ns = ns:upper ()
          tbegin, tend = tbegin..ns..'_', '_'..ns..tend
          if cap:match('^{%s*namespace') then
             return ns..'_'..dofix (cap:sub (2,-2))..'_'..ns
          else
             return ns..'_'..cap:sub (2,-2)..'_'..ns
          end
       end
      )
   end
   local s = dofix (s)
   if not s or #tbegin == 0 then
      return nil
   end
   s = assert (s:gsub (tbegin, '\n'..tbegin..'BEGIN\n'))
   s = assert (s:gsub (tend, '\n'..tbegin..'END\n'))
   return s
end

for _,file in ipairs (arg) do
   local old = read (file)
   local new = fix (old)
   if not new then
      goto continue
   end
   write (file..'~', old)
   write (file, new)
   ::continue::
end
