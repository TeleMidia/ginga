<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Exemplo de aplicacao NCL embutida-->
<ncl id="_03prepPassiveDevicesEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id="screenReg" width="100%" height="100%" zIndex="1">
        <region id="frameReg" left="5%" top="6.7%" width="18.5%" height="18.5%" zIndex="2"/>
      </region>
      <region id="passiveAdvReg"  left="5%" top="5%" width="40%" height="40%" zIndex="2"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="passiveAdvDesc" region="passiveAdvReg" focusIndex="5" focusBorderWidth="0">
        <descriptorParam name="transparency" value="50%"/>
      </descriptor>
      <descriptor id="screenDesc" region="screenReg"/>
      <descriptor id="photoDesc" region="frameReg" explicitDur="3s"/>
      <descriptor id="audioDesc"/>
      <descriptor id="dribleDesc" region="frameReg"/>
    </descriptorBase>
    <connectorBase>
      <importBase documentURI="causalConnBase.ncl" alias="conEx"/>
    </connectorBase>
  </head>
  <body>
    <port id="entry" component="animation"/>
    <media id="animation" src="media/animGar.mp4" descriptor="screenDesc">
      <area id="segDrible" begin="12s"/>
      <area id="segPhoto" begin="41s"/>
      <area id="segIcon" begin="45s" end="51s"/>
    </media>
    <media id="choro" src="media/choro.mp4" descriptor="audioDesc"/>
    <media id="drible" src="media/drible.mp4" descriptor="dribleDesc"/>
    <media id="photo" src="media/photo.png" descriptor="photoDesc"/>
    <media id="advert" src="advert.ncl" descriptor="passiveAdvDesc"/>
    <link id="lMusic" xconnector="conEx#onBeginStart_delay">
      <bind role="onBegin" component="animation"/>
      <bind role="start" component="choro">
        <bindParam name="delay" value="5s"/>
      </bind>
    </link>
    <link id="lDrible" xconnector="conEx#onBeginStart">
      <bind role="onBegin" component="animation"         interface="segDrible"/>
      <bind role="start" component="drible"/>
    </link>
    <link id="lPhoto" xconnector="conEx#onBeginStart">
      <bind role="onBegin" component="animation" interface="segPhoto"/>
      <bind role="start" component="photo"/>
    </link>
    <link id="lEnd" xconnector="conEx#onEndStop">
      <bind role="onEnd" component="animation"/>
      <bind role="stop" component="advert"/>
      <bind role="stop" component="choro"/>
    </link>
    <link id="lIcon" xconnector="conEx#onBeginStart">
      <bind role="onBegin" component="animation" interface="segIcon"/>
      <bind role="start" component="advert"/>
    </link>
  </body>
</ncl>
