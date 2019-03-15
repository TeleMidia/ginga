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
end
