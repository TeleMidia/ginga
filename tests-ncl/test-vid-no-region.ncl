<ncl>
  <head>
    <descriptorBase>
      <descriptor id="desc">
        <descriptorParam name="top" value="25%"/>
        <descriptorParam name="left" value="25%"/>
        <descriptorParam name="width" value="50%"/>
        <descriptorParam name="height" value="50%"/>
      </descriptor>
    </descriptorBase>
  </head>
  <body>
    <port id="start" component="vid"/>
    <media id="vid" src="samples/clock.ogv" descriptor="desc"/>
  </body>
</ncl>
