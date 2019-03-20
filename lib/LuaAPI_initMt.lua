-- Code shared by all metatables.
do
   local mt = ...

   mt.__index = function (self, k)
      return mt[k] or mt[self][k]
   end

   mt.__newindex = function (self, k, v)
      mt[self][k] = v
   end

   -- Attaches private data and access functions.
   mt._attachData = function (self, data, funcs)
      local data = data or {}
      local funcs = funcs or {}
      local _mt = {}
      _mt.__index = function (_, k)
         local t = funcs[k]
         if t == nil then
            return nil
         end
         local f = t[1]
         if f == nil then
            return nil
         end
         return f (self)
      end
      _mt.__newindex = function (_, k, v)
         local t = funcs[k]
         if t == nil then
            return
         end
         local f = t[2]
         if f == nil then
            return
         end
         f (self, v)
      end
      mt[self] = setmetatable (data, _mt)
      mt._init (self)
   end

   -- Detaches private data.
   mt._detachData = function (self)
      mt._fini (self)
      rawset (mt, '__tostring', nil) -- avoid crashing tracer
      mt[self] = nil
   end

   -- Initializes private data.
   mt._init = function ()
   end

   -- Finalizes private data.
   mt._fini = function ()
   end

   -- Wrap GLib logging functions into more convenient functions.
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
   end
end
