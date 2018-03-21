<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <connectorBase>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop" max="unbounded" qualifier="par"/>
      </causalConnector>
    </connectorBase>
    </head>
    <body>
      <port id="start" component="ctx"/>
      <context id="ctx">
        <property name="propName" value=""/>
        <port id="start0a" component="time0"/>
        <media id="time0">
          <property name="bounds" value="10%,10%,100%,100%"/>
          <property name="background" value="red"/>
        </media>
      </context>
      <link xconnector="onBeginStop">
        <bind role="onBegin" component="ctx"/>
        <bind role="stop" component="ctx"/>
      </link>
    </body>
  </ncl>
