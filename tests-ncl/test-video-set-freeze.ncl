<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id='reg' width='50%' height='50%'/>
      <region id='reg2' left='640' width='150' height='150'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onKeySelectionStop">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="stop" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video1'/>
    <port id='start2' component='video2'/>
    
    <!--RED stop video1 -->
    <media id='video1' src='../samples/small.mp4' descriptor='desc' >
      <property name='freeze' value="true"/>
    </media>
    <media id='video2' src='../samples/animGar.mp4' descriptor='desc2'/>
    
    <link xconnector="onKeySelectionStop">
      <bind role='onSelection' component='video2'>
        <bindParam name='keyCode' value='RED'/>
      </bind>
      <bind role="stop" component="video1" />
    </link>
  </body>
</ncl>
