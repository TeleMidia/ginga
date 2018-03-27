<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
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
  <port id="start1" component="m1"/>
  <port id="start2" component="m2"/>
  <media id="m1" src='../samples/bunny.ogg' descriptor="desc1"/>
  <media id="m2" src='../samples/bunny.ogg' descriptor="desc2"/>
  <port id="startctx" component="ctx"/>
  <context id="ctx">
    <port id="start3" component="m3"/>
    <!-- <port id="start4" component="m4"/> -->
    <media id="m3" src='../samples/bunny.ogg' descriptor="desc3"/>
    <media id="m4" src='../samples/bunny.ogg' descriptor="desc4"/>
  </context>
</body>
</ncl>
