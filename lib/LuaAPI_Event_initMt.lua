do
   local mt = ...
   local trace = function (msg, ...)
      print ('>>> '..mt.__name..': '..msg:format (...))
   end

   mt.__index = function (self, k)
      --trace ('__index(%s)', k)
      return mt[k] or mt[self][k]
   end

   mt.__newindex = function (self, k, v)
      trace ('__newindex(%s, %s, %s)',
             self:getId (), tostring (k), tostring (v))
      mt[self][k] = v
   end

   mt._attachData = function (self)
      trace ('_attachData (%s)', self:getId ())
      local f = {
         object    = {mt.getObject,    nil},
         type      = {mt.getType,      nil},
         id        = {mt.getId,        nil},
         state     = {mt.getState,     mt.setState},
         beginTime = {mt.getBeginTime, mt.setBeginTime},
         endTime   = {mt.getEndTime,   mt.setEndTime},
         label     = {mt.getLabel,     mt.setLabel},
      }
      local t = {
         __index = function (_t, k)
            local fget = f[k][1]
            if fget then
               trace ('fget (%s, %s)', self:getId (), tostring (k))
               return fget (self)
            else
               return nil
            end
         end,
         __newindex = function (_t, k, v)
            local fset = f[k][2]
            if fset then
               trace ('fset (%s, %s, %s)', self:getId (),
                      tostring (k), tostring (v))
               fset (self, v)
            end
         end
      }
      mt[self] = setmetatable ({}, t)
   end

   mt._detachData = function (self)
      trace ('_detachData (%s)', self:getId ())
      mt[self] = nil
   end
end
