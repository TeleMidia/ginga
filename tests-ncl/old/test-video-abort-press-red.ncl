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
      <causalConnector id="onKeySelectionAbort">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="abort" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start" />
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
    <media id='img' src='../samples/gnu.png' descriptor='desc3' />
    <link xconnector="onKeySelectionAbort">
      <bind role='onSelection' component='video'>
        <bindParam name='keyCode' value='RED'/>        
      </bind>
      <bind role="abort" component="video" />
    </link>
    <link xconnector="onEndStart">
      <bind role='onEnd' component='video' />
      <bind role="start" component='img' />
    </link>
  </body>
</ncl>
