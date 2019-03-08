do
   local mt = ...

   mt.__index = function (self, k)
      return mt[k] or mt[self][k]
   end

   mt.__newindex = function (self, k, v)
      mt[self][k] = v
   end

   mt._traceOff  = {}
   mt._traceSelf = nil
   mt._trace = function (self, func, ...)
      if mt._traceOff[func] then
         return                 -- nothing to do
      end
      local args = {}
      for _,v in ipairs ({...}) do
         table.insert (args, tostring (v))
      end
      args = table.concat (args, ', ')
      local prefix
      if mt._traceSelf then
         prefix = self:_traceSelf ()..':'
      else
         prefix = ''
      end
      local tag = tostring (self)
      local tp, ptr = tag:match ('^Ginga.(%w+): (.-)$')
      if tp and ptr then
         local fill = #'Document' - #tp
         tag = ('%s::%s%s'):format (ptr, tp, (' '):rep (fill))
      end
      print (('>>> [%s] %s%s (%s)'):format (tag, prefix, func, args))
   end

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
         fset (self, v)
      end
      mt[self] = setmetatable (data, _mt)
   end

   mt._detachData = function (self)
      mt[self] = nil
   end
end
