<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
      <region id='reg2' left='640' width='150' height='150'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" max="unbounded" qualifier="par"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video1' interface="area1"/>
    <media id='video1' src='../samples/bunny.ogg' descriptor='desc' >
      <area id='area1' begin='16s' />
      <property name="soundLevel" value="0"/>
    </media>
    <!-- <media id='video2' src='../samples/animGar.mp4' descriptor='desc2'>      
      <area id='area2' begin='15s' /> 
    </media>
    
    <link xconnector="onBeginStart">
      <bind role='onBegin' component='video1' />
      <bind role="start" component="video2" interface="area2" />
    </link> -->
  </body>
</ncl>
