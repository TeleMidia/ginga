<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onSelectionSet">
        <simpleCondition role="onSelection"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start1" component="one"/>
    <media id="one">
      <property name="focusIndex" value="1"/>
      <property name="moveDown" value="2"/>
      <property name="moveUp" value="2"/>
      <property name="background" value="red"/>
      <property name="bounds" value="0%,0%,50%,50%"/>
    </media>
    <media id="two" type="application/x-ncl-360">
      <property name="device" value="Guarana"/>
    </media>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="one"/>
      <bind role="start" component="two" />
    </link>
  </body>
</ncl>