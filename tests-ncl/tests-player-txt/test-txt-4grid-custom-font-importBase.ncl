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
    <importBase alias="customFonts"
      documentURI="test-media-txt-4grid-custom-font-fontBase.ncl"/>
  </fontBase>
</head>
<body>
  <port id="start1" component="m1"/>
  <port id="start2" component="m2"/>
  <media id="m1" src="../samples/text.txt" descriptor="desc1"/>
  <media id="m2" src="../samples/text.txt" descriptor="desc2">
    <property name="background" value="white"/>
    <property name="fontColor" value="red"/>
    <property name="fontFamily" value="SudegnakNo2"/>
    <property name="fontSize" value="20"/>
  </media>
  <port id="startctx" component="ctx"/>
  <context id="ctx">
    <port id="start3" component="m3"/>
    <port id="start4" component="m4"/>
    <media id="m3" src="../samples/text.txt" descriptor="desc3">
      <property name="background" value="blue"/>
      <property name="fontColor" value="white"/>
      <property name="fontFamily" value="Medieval Sharp"/>
      <property name="fontSize" value="20"/>
    </media>
    <media id="m4" src="../samples/text.txt" descriptor="desc4">
      <property name="background" value="blue"/>
      <property name="fontColor" value="white"/>
      <property name="fontFamily" value="MyCustomFontName"/>
      <property name="fontWeight" value="bold"/>
      <property name="fontSize" value="20"/>
    </media>
  </context>
</body>
</ncl>
