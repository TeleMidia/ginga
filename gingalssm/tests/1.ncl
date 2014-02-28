<ncl id="infoExchange1">
  <head>
    <regionBase>
      <region id="r1" left="0%" top="0%" width="50%" height="50%" zIndex="1"/>
      <region id="r2" left="50%" top="50%" width="50%" height="50%" zIndex="2"/>
    </regionBase>

    <descriptorBase>
      <descriptor id="d1" region="r1"/>
      <descriptor id="d2" region="r2" focusIndex="1"/>
    </descriptorBase>

    <connectorBase>
      <causalConnector id="onSelectionSet">
      <connectorParam name="var"/>
      <simpleCondition role="onSelection"/>
      <simpleAction role="set" value="$var" max="unbounded"/>
     </causalConnector>

     <causalConnector id="onEndAttributionSet">
      <simpleCondition role="onEndAttribution"/>
      <simpleAction role="set" value="$var" max="unbounded"/>
     </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="doc1p1" component="m1"/>
    <port id="doc1p2" component="m2"/>
    <port id="doc1p3" component="m1" interface="extern"/>

    <media id="m1" src="" descriptor="d1">
    	<property name="background" value="yellow"/>
    	<property name="extern"/>
    </media>

    <media id="m2" src="hi.txt" descriptor="d2">
    	<property name="background" value="purple"/>
    	<property name="x-content"/>
    </media>

    <link id="l1" xconnector="onSelectionSet">
      <bind role="onSelection" component="m1"/>
      <bind role="set" component="m1" interface="extern">
      	<bindParam name="var" value="HereIsMyInternalInfo"/>
      </bind>
    </link>

    <link id="l2" xconnector="onEndAttributionSet">
      <bind role="onEndAttribution" component="m1" interface="extern"/>
      <bind role="set" component="m2" interface="x-content">
      	<bindParam name="var" value="$getValue"/>
      </bind>
      <bind role="getValue" component="m1" interface="extern"/>
    </link>

  </body>
</ncl>
