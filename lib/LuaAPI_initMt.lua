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
         local fget = funcs[k][1]
         if fget then
            trace ('fget (%s)', tostring (k))
            return fget (self)
         else
            return nil
         end
      end
      _mt.__newindex = function (_, k, v)
         local fset = funcs[k][2]
         if fset then
            trace ('fset (%s, %s)', tostring (k), tostring (v))
            fset (self, v)
         else
            error (("field '%s' is read-only"):format (k))
         end
      end
      mt[self] = setmetatable (data, _mt)
   end

   mt._detachData = function (self)
      mt[self] = nil
   end
end
