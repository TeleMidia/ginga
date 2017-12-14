<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginAbort">
        <simpleCondition role="onBegin"/>
        <simpleAction role="abort"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    
    <media id='video' src='../samples/bunny.ogg' descriptor='desc' >
      <area id='area1' begin='3s' />
    </media>
    <link xconnector="onBeginAbort">
      <bind role="onBegin" component="video" interface="area1"/>
      <bind role="abort" component="video" />
    </link>    
  </body>
</ncl>
