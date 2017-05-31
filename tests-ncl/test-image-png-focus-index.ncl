<ncl>
  <head>
    <regionBase>
      <region id="reg1" width="50%" height="50%"/>
      <region id="reg2" bottom="0%" right="0%" width="50%" height="50%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc1" region="reg1"
                  focusIndex="1"
                  moveUp="2"
                  moveDown="2"
                  moveLeft="2"
                  moveRight="2"/>
      <descriptor id="desc2" region="reg2" focusIndex="2"
                  moveUp="1"
                  moveDown="1"
                  moveLeft="1"
                  moveRight="1"/>
    </descriptorBase>
  </head>
  <body>
    <port id="start1" component="img1"/>
    <port id="start2" component="img2"/>
    <media id="img1" src="samples/gnu.png" descriptor="desc1"/>
    <media id="img2" src="samples/gnu.png" descriptor="desc2"/>
    <media id="settings" type="application/x-ginga-settings">
      <property name="service.currentFocus"/>
    </media>
  </body>
</ncl>
