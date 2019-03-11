-- Gets a string representation of table.
local function tableToString (tab)
   local t = {}
   for k,v in pairs (tab) do
      table.insert (t, tostring (k)..'='..tostring (v))
   end
   return '{'..table.concat (t, ', ')..'}'
end

-- Gets a string representation of list.
local function listToString (list)
   local t = {}
   for _,v in ipairs (list) do
      local str
      if type (v) == 'table' then
         str = tableToString (v)
      elseif type (v) == 'string' then
         str = "'"..v.."'"
      else
         str = tostring (v)
      end
      table.insert (t, str)
   end
   return table.concat (t, ', ')
end

do
   local mt = ...

   -- Enable trace.
   mt._traceOn = mt._traceOn or {}

   -- Disable trace for these functions.
   mt._traceOff = mt._traceOff or {}

   -- Install trace wrappers.
   for name,func in pairs (mt) do

      if type (func) ~= 'function' then
         goto continue          -- nothing to do
      end

      if name:sub (1,2) == '__' then
         goto continue          -- nothing to do
      end

      if not mt._traceOn[name] and mt._traceOff[name] then
         goto continue          -- nothing to do
      end

      mt[name] = function (...)
         local input, output = {...}, {func (...)}
         if #output == 0 then
            print (('%s:\t%s (%s)')
                  :format (mt.__name, name, listToString (input)))
         else
            print (('%s:\t%s (%s) -> %s')
                  :format (mt.__name, name, listToString (input),
                           listToString (output)))
         end
         return table.unpack (output)
      end
      ::continue::
   end
end
