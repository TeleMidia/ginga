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
      <causalConnector id="onKeySelectionPause">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="pause" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onKeySelectionResume">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="resume" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onBeginResume">
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume" />
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <port id='start2' component='video2'/>
    <media id='video' src='../samples/bunny.ogg' descriptor='desc' >
      <property name="soundLevel" value="0"/>
    </media>
    <media id='video2' src='../samples/animGar.mp4' descriptor='desc2'>      
      <area id='area1' begin='5s' />
    </media>
    <link xconnector="onKeySelectionPause">
      <bind role='onSelection' component='video'>
        <bindParam name='keyCode' value='RED'/>        
      </bind>
      <bind role="pause" component="video" />
    </link>

    <!-- <link xconnector="onKeySelectionResume">
      <bind role='onSelection' component='video'>
        <bindParam name='keyCode' value='GREEN'/>
      </bind>
      <bind role="resume" component="video" />
    </link> -->

   <!--  <link xconnector="onBeginResume">
      <bind role="onBegin" component="video2" interface="area1" />
      <bind role="resume" component="video" />
    </link> -->

    <link xconnector="onKeySelectionResume">
      <bind role='onSelection' component='video'>
        <bindParam name='keyCode' value='BLUE'/>
      </bind>
      <bind role="resume" component="video" />
    </link>
  </body>
</ncl>
