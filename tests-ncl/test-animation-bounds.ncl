<ncl>
  <head>
    <regionBase>
      <region id='reg1' left="37.5%" top="37.5%" width='25%' height='25%'/>
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

    <media id='text1' src='samples/animGar.mp4' descriptor='desc1'>
      <property name="bounds"/>
    </media>

    <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="RED" />
      </bind> 
      <bind role="set" component="text1" interface="bounds">
        <bindParam name="var2" value="0,0,800,600"/>
        <bindParam name="duration" value="2s"/>
      </bind>
    </link>

     <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="GREEN" />
      </bind> 
      <bind role="set" component="text1" interface="bounds">
        <bindParam name="var2" value="0,0,0,0"/>
        <bindParam name="duration" value="1.5s"/>
      </bind>
    </link>

  
   
  </body>
</ncl>
