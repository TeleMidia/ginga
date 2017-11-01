<ncl>
  <head>
    <ruleBase>
      <rule id="rColorRed" var="color" value="red" comparator="eq"/>
      <rule id="rColorGreen" var="color" value="green" comparator="eq"/>
      <rule id="rColorBlue" var="color" value="blue" comparator="eq"/>
      <rule id="rColorYellow" var="color" value="yellow" comparator="eq"/>
    </ruleBase>
  </head>
  <body>
    <port id="start" component="switch"/>
    <media id="settings" type="application/x-ginga-settings">
      <property name="color" value="green"/>
    </media>
    <switch id="switch">
      <bindRule constituent="mRed" rule="rColorRed"/>
      <bindRule constituent="mGreen" rule="rColorGreen"/>
      <bindRule constituent="mBlue" rule="rColorBlue"/>
      <bindRule constituent="mYellow" rule="rColorYellow"/>
      <media id="mRed">
        <property name="background" value="red"/>
      </media>
      <media id="mGreen">
        <property name="background" value="green"/>
      </media>
      <media id="mBlue">
        <property name="background" value="blue"/>
      </media>
      <media id="mYellow">
        <property name="background" value="yellow"/>
      </media>
    </switch>
  </body>
</ncl>
