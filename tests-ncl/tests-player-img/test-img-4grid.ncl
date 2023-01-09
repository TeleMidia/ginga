<ncl>
<head>
  <regionBase>
    <region id='reg1' left='0' top='0' width='50%' height='50%'/>
    <region id='reg2' left='50%' top='0' width='50%' height='50%'/>
    <region id='reg3' left='0' top='50%' width='50%' height='50%'/>
    <region id='reg4' left='50%' top='50%' width='50%' height='50%'/>
  </regionBase>
  <descriptorBase>
    <descriptor id="desc1" region="reg1"/>
    <descriptor id="desc2" region="reg2"/>
    <descriptor id="desc3" region="reg3"/>
    <descriptor id="desc4" region="reg4"/>
  </descriptorBase>
</head>
<body>
  <port id="start1" component="m1"/>
  <port id="start2" component="m2"/>
  <media id="m1" src="../samples/gnu.png" descriptor="desc1"/>
  <media id="m2" src="http://raw.github.com/telemidia/ginga/master//tests-ncl/samples/gnu.png" descriptor="desc2"/>
  <port id="startctx" component="ctx"/>
  <context id="ctx">
    <port id="start3" component="m3"/>
    <port id="start4" component="m4"/>
    <media id="m3" src="../samples/gnu.png" descriptor="desc3"/>
    <media id="m4" src="../samples/gnu.png" descriptor="desc4"/>
  </context>
</body>
</ncl>
