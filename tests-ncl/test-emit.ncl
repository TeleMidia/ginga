<ncl>
  <head>
    <behavior>
    <![CDATA[
      local doc = ...
      parOr {
         function ()
            print (doc.time, 't1 awaiting for x')
            await'x'
            print ('t1 ended')
         end,
         function ()
            print (doc.time, 't2 emitted x')
            doc:_broadcast'x'
            print ('t2 ended')
         end
      }
      print ('parOr ended')
    ]]>
    </behavior>
  </head>
</ncl>
