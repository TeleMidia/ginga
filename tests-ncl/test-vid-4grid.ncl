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
  <port id="start1" component="vid1"/>
  <port id="start2" component="vid2"/>
  <media id="vid1" src="samples/clock.ogv" descriptor="desc1"/>
  <media id="vid2" src="samples/clock.ogv" descriptor="desc2"/>
  <port id="startctx" component="ctx"/>
  <context id="ctx">
    <port id="start3" component="vid3"/>
    <port id="start4" component="vid4"/>
    <media id="vid3" src="samples/clock.ogv" descriptor="desc3"/>
    <media id="vid4" src="samples/clock.ogv" descriptor="desc4"/>
  </context>
</body>
</ncl>
