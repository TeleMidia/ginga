<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
      <region id='reg2' left='640' width='150' height='150'/>
      <region id='reg3' left='640' top='150' width='150' height='150'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
      <descriptor id='desc3' region='reg3'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onEndStop">
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop" max="unbounded" qualifier="par"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video1' interface='video1area1'/>
    <media id='video1' src='../samples/bunny.ogg' descriptor='desc' >
      <area id='video1area1' begin="33s" end='40s' />
      <area id='video1area3' begin="22s" end="35s" />
      <!-- <area id='video1area2' begin="5s" /> 
      <area id='video1area4' end="9s" />
      <area id='video1area5' end="22s" /> -->
      <property name="soundLevel" value="0"/>
    </media>
    <media id='video2' src='../samples/animGar.mp4' descriptor='desc2'>      
      <area id='video2area1' begin='25s' /> 
    </media>
    <media id='img1' src='../samples/gnu.png' descriptor='desc3'/> 

    <link xconnector="onBeginStart">
      <bind role='onBegin' component='video1' interface='video1area1'/>
      <bind role="start" component="video2" interface="video2area1"/>
      <bind role="start" component="img1"/>
    </link> 

    <link xconnector="onEndStop">
      <bind role='onEnd' component='video1' interface='video1area3'/>
      <bind role="stop" component="video2" />
    </link>
  </body>
</ncl>
