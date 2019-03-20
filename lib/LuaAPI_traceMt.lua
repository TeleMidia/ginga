-- Gets a string representation of table.
local function tableToString (tab)
   local t = {}
   for k,v in pairs (tab) do
      local str
      if type (v) == 'table' then
         str = tableToString (v)
      elseif type (v) == 'string' then
         str = "'"..v.."'"
      else
         str = tostring (v)
      end
      table.insert (t, tostring (k)..'='..str)
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

   -- Wrap GLib logging functions into more convenient functions.
   -- TODO: Move this to initMt.
   local t = {_debug=mt._debug, _warning=mt._warning, _error=mt._error}
   for name,func in pairs  (t) do
      assert (type (func) == 'function')
      mt[name] = function (self, fmt, ...)
         local tag
         local args
         if type (self) == 'userdata' then
            args = {...}
            local _mt = assert (getmetatable (self))
            tag = assert (_mt.__name)..': '
         elseif type (self) == 'string' then
            args = {fmt, ...}
            tag = ''
            fmt = self
         else
            error ('bad format: '..tostring (fmt))
         end
         return func ((tag..fmt):format (table.unpack (args)))
      end
      mt._traceOff[name] = true
   end

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
            mt._debug ('%s:\t%s (%s)', mt.__name, name,
                       listToString (input))
         else
            mt._debug ('%s:\t%s (%s) -> %s', mt.__name, name,
                       listToString (input), listToString (output))
         end
         return table.unpack (output)
      end
      ::continue::
   end
end
