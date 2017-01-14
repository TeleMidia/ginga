<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Exemplo de sincronismo sem a interacao do usuario e com reuso apenas de relacoes-->
<ncl id="syncEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <connectorBase>
      <causalConnector id="onBeginStart_delay">
        <connectorParam name="delay"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" delay="$delay" max="unbounded" qualifier="par"/>
      </causalConnector>
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
    <port id="entry" component="animation"/>
    <media id="animation" src="media/animGar.mp4">
      <area id="segDrible" begin="12s"/>
      <area id="segPhoto" begin="41s"/>
      <property name="width" value="100%"/>
      <property name="height" value="100%"/>
      <property name="zIndex" value="2"/>
    </media>
    <media id="choro" src="media/choro.mp4"/>
    <media id="drible" src="media/drible.mp4">
      <property name="left" value="5%"/>
      <property name="top" value="6.7%"/>
      <property name="width" value="18.5%"/>
      <property name="height" value="18.5%"/>
      <property name="zIndex" value="3"/>
    </media>
    <media id="photo" src="media/photo.png">
      <property name="left" value="5%"/>
      <property name="top" value="6.7%"/>
      <property name="width" value="18.5%"/>
      <property name="height" value="18.5%"/>
      <property name="zIndex" value="3"/>
      <property name="explicitDur" value="5s"/>
    </media>
    <link  id="lMusic" xconnector="onBeginStart_delay">
      <bind role="onBegin" component="animation"/>
      <bind role="start" component="choro">
        <bindParam name="delay" value="5s"/>
      </bind>
    </link>
    <link  id="lDrible" xconnector="onBeginStart">
      <bind role="onBegin" component="animation" interface="segDrible"/>
      <bind role="start" component="drible"/>
    </link>
    <link  id="lPhoto" xconnector="onBeginStart">
      <bind role="onBegin" component="animation" interface="segPhoto"/>
      <bind role="start" component="photo"/>
    </link>
    <link  id="lEnd" xconnector="onEndStop">
      <bind role="onEnd" component="animation"/>
      <bind role="stop" component="choro"/>
    </link>
  </body>
</ncl>
