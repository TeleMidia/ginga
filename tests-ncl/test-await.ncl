<ncl>
  <head>
    <behavior>
    <![CDATA[
      while true do
         e = await {}
         local t = {}
         for k,v in pairs (e) do
            table.insert (t, k..'='..tostring (v))
         end
         print ('{'..table.concat (t, ', ')..'}')
      end
    ]]>
    </behavior>
  </head>
</ncl>
