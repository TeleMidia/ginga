<ncl>
  <head>
    <script>
    <![CDATA[
      local doc = ...
      print"I'm going to await for 1s..."
      await (1)
      print ("Done.  Now document time is: "..doc.time.."us.")
      os.exit (0)
    ]]>
    </script>
  </head>
</ncl>
