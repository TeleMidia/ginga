do
   local mt = ...

   local trace = function (msg, ...)
      print ('>>> '..mt.__name..': '..msg:format (...))
   end
   mt._trace = trace

   mt.__index = function (self, k)
      return mt[k] or mt[self][k]
   end

   mt.__newindex = function (self, k, v)
      mt[self][k] = v
   end

   mt._attachData = function (self, data, funcs)
      assert (type (data)  == 'table')
      assert (type (funcs) == 'table')
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
         --trace ('fget (%s)', tostring (k))
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
         --trace ('fset (%s, %s)', tostring (k), tostring (v))
         fset (self, v)
      end
      mt[self] = setmetatable (data, _mt)
   end

   mt._detachData = function (self)
      mt[self] = nil
   end
end
