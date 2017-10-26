<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id='reg' width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onKeySelectionStart">
        <connectorParam name="var"/>
        <simpleCondition role="onSelection" key="$var" />
        <simpleAction role="start" />
      </causalConnector>
       <causalConnector id="onKeySelectionStop">
        <connectorParam name="var"/>
        <simpleCondition role="onSelection" key="$var" />
        <simpleAction role="stop" />
      </causalConnector>
       <causalConnector id="onKeySelectionSet">
        <connectorParam name="var"/>
        <connectorParam name="var2"/>
        <simpleCondition role="onSelection" key="$var" />
        <simpleAction role="set" value="$var2"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='vid1'/>
    <media id='vid1' src='../samples/small.mp4' descriptor='desc' >
      <property name='freeze' value="false"/>
    </media>
  </body>
</ncl>
