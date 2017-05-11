<ncl>
  <head>
    <regionBase>
      <region id='reg1' width='100%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc1' region='reg1'/>
    </descriptorBase>
    <connectorBase>
       <causalConnector id="onKeySelectionSet">
        <connectorParam name="var"/>
        <connectorParam name="var2"/>
        <connectorParam name="delay"/>
        <connectorParam name="duration"/>
        <simpleCondition role="onSelection" key="$var" />
        <simpleAction role="set" value="$var2" delay="$delay" duration="$duration" />
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start1' component='text1'/>

    <media id='text1' src='samples/gnu.png' descriptor='desc1'>
      <property name="top"/>
      <property name="left"/>
      <property name="width"/>
      <property name="height"/>
    </media>

    <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="RED" />
      </bind> 
      <bind role="set" component="text1" interface="height">
        <bindParam name="var2" value="800"/>
        <bindParam name="duration" value="2s"/>
      </bind>
    </link>

     <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="GREEN" />
      </bind> 
      <bind role="set" component="text1" interface="height">
        <bindParam name="var2" value="0"/>
        <bindParam name="duration" value="2s"/>
      </bind>
    </link>

  
   
  </body>
</ncl>
