<ncl>
<head>
  <regionBase>
    <region id="reg1" top="0%" left="0%" width="50%" height="50%"/>
    <region id="reg2" top="0%" left="50%" width="50%" height="50%"/>
    <region id="reg3" top="50%" left="0%" width="50%" height="50%"/>
    <region id="reg4" top="50%" left="50%" width="50%" height="50%"/>
  </regionBase>
  <descriptorBase>
    <descriptor id="desc1" region="reg1">
      <descriptorParam name="background" value="red"/>
      <descriptorParam name="fontColor" value="white"/>
      <descriptorParam name="fontFamily" value="Pacifico"/>
      <descriptorParam name="fontSize" value="20"/>
    </descriptor>
    <descriptor id="desc2" region="reg2"/>
    <descriptor id="desc3" region="reg3"/>
    <descriptor id="desc4" region="reg4"/>
  </descriptorBase>
  <fontBase>
    <font fontFamily="NunitoSans" src="../samples/MedievalSharp.ttf"/>
    <font fontFamily="MyCustomFontName" src="../samples/MedievalSharp-Bold.ttf"/>
    <font fontFamily="Pacifico" src="../samples/Pacifico.ttf"/>
    <font fontFamily="SudegnakNo2" src="../samples/SudegnakNo2.otf"/>
  </fontBase>
</head>
<body>
  <port id="start1" component="m1"/>
  <port id="start2" component="m2"/>
  <media id="m1" src="../samples/fps_font_NunitoSans.lua" descriptor="desc1"/>
  <media id="m2" src="../samples/fps_font_MyCustomFontName.lua" descriptor="desc2"/>
  <port id="startctx" component="ctx"/>
  <context id="ctx">
    <port id="start3" component="m3"/>
    <port id="start4" component="m4"/>
    <media id="m3" src="../samples/fps_font_Pacifico.lua" descriptor="desc3"/>
    <media id="m4" src="../samples/fps_font_SudegnakNo2.lua" descriptor="desc4"/>
  </context>
</body>
</ncl>
