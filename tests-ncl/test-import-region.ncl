<ncl>
  <head>
    <regionBase>
      <importBase documentURI="test-media-img-4grid.ncl" alias="r"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="d1" region="r#reg1"/>
      <descriptor id="d2" region="r#reg2"/>
      <descriptor id="d3" region="r#reg3"/>
      <descriptor id="d4" region="r#reg4"/>
    </descriptorBase>
  </head>
  <body>
    <port id="p1" component="m1"/>
    <port id="p2" component="m2"/>
    <port id="p3" component="m3"/>
    <port id="p4" component="m4"/>
    <media id="m1" src="samples/gnu.png" descriptor="d1"/>
    <media id="m2" src="samples/gnu.png" descriptor="d2"/>
    <media id="m3" src="samples/gnu.png" descriptor="d3"/>
    <media id="m4" src="samples/gnu.png" descriptor="d4"/>
  </body>
</ncl>
