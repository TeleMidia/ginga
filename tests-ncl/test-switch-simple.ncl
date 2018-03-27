<ncl>
  <head>
    <ruleBase>
      <rule id="rColorRed" var="color" value="red" comparator="eq"/>
      <rule id="rColorGreen" var="color" value="green" comparator="eq"/>
    </ruleBase>
  </head>
  <body>
    <port id="pSwitch" component="switch"/>
    <media id="settings" type="application/x-ginga-settings">
      <property name="color" value="green"/>
    </media>
    <switch id="switch">
      <defaultComponent component="mBlue"/>
      <bindRule constituent="mRed" rule="rColorRed"/>
      <bindRule constituent="mGreen" rule="rColorGreen"/>
      <media id="mRed">
        <property name="background" value="red"/>
      </media>
      <media id="mGreen">
        <property name="background" value="green"/>
      </media>
      <media id="mBlue">
        <property name="background" value="blue"/>
      </media>
    </switch>
  </body>
</ncl>