<ncl>
  <head>
    <regionBase>
      <region width="30%" height="30%" left="10%" top="40%" id="rg1"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="ds1" region="rg1"/>
    </descriptorBase>
  </head>
  <body>
    <port id="entryPoint" component="lua"/>
    <media id="lua" src="samples/randomprop.lua" descriptor="ds1">
      <property name="max_value" value="500"/>
    </media>
  </body>
</ncl>