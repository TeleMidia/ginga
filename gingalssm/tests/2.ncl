<ncl id="infoExchange2">
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

     <causalConnector id="onEndAttributionTestStart">
      <connectorParam name="value"/>
      <compoundCondition operator="and">
         <assessmentStatement comparator="eq">
            <attributeAssessment role="test" max="unbounded" eventType="attribution" attributeType="nodeProperty"/>
            <valueAssessment value="$value"/>
         </assessmentStatement>
         <simpleCondition role="onEndAttribution"/>
      </compoundCondition>
      <simpleAction role="start" max="unbounded"/>
     </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="doc2p1" component="m1"/>
    <port id="doc2p2" component="m1" interface="import"/>

    <media id="m1" src="hi.txt" descriptor="d1">
    	<property name="background" value="green"/>
    	<property name="import"/>
    </media>

    <media id="m2" descriptor="d2">
    	<property name="background" value="blue"/>
    	<property name="x-content"/>
    </media>

    <link id="l1" xconnector="onEndAttributionTestStart">
      <bind role="onEndAttribution" component="m1" interface="import"/>
      <bind role="test" component="m1" interface="import">
         <bindParam name="value" value="HereIsMyInternalInfo"/>
      </bind>
      <bind role="start" component="m2"/>
    </link>
    
    <link id="l2" xconnector="onSelectionSet">
      <bind role="onSelection" component="m2"/>
      <bind role="set" component="m1" interface="import">
      	<bindParam name="var" value="infoReceived"/>
      </bind>
    </link>
  </body>
</ncl>
