<ncl>
<head>
  <regionBase>
    <region id="reg1" top="0%" left="0%" width="50%" height="50%"/>
    <region id="reg2" top="0%" left="50%" width="50%" height="50%"/>
    <region id="reg3" bottom="0%" right="0%" width="50%" height="50%"/>
    <region id="reg4" bottom="0%" right="50%" width="50%" height="50%"/>
  </regionBase>
  <descriptorBase>
    <descriptor id="desc1" region="reg1"/>
    <descriptor id="desc2" region="reg2"/>
    <descriptor id="desc3" region="reg3"/>
    <descriptor id="desc4" region="reg4"/>
  </descriptorBase>
</head>
<body>
  <port id="start1" component="img1"/>
  <port id="start2" component="img2"/>
  <media id="img1" src="samples/gnu.png" descriptor="desc1"/>
  <media id="img2" src="samples/gnu.png" descriptor="desc2"/>
  <port id="startctx" component="ctx"/>
  <context id="ctx">
    <port id="start3" component="img3"/>
    <port id="start4" component="img4"/>
    <media id="img3" src="samples/gnu.png" descriptor="desc3"/>
    <media id="img4" src="samples/gnu.png" descriptor="desc4"/>
  </context>
</body>
</ncl>
