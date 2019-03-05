do
   local mt = ...
   local trace = function (msg, ...)
      print ('>>> '..mt.__name..': '..msg:format (...))
   end

   mt.__index = function (self, k)
      --trace ('__index(%s)', k)
      return mt[k] or mt[self][k]
   end

   mt._attachData = function (self)
      trace ('_attachData (%s)', self:getId ())
      mt[self] = {
         object = self:getObject (),
         type   = self:getType (),
         id     = self:getId (),
      }
   end

   mt._detachData = function (self)
      trace ('_detachData (%s)', self:getId ())
      mt[self] = nil
   end
end
