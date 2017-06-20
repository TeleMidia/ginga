<ncl>
  <head>
    <regionBase>
      <region id='reg' width='50%' height='50%'/>
      <region id='reg2' top="50%" width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
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
    <port id='start' component='text1'/>
    <media id='text1' src='samples/gnu.png' descriptor='desc'>
      <property name="width"/>
    </media>
    <media id='text2' src='samples/gnu.png' descriptor='desc2'>
    </media>
    
     <link xconnector="onKeySelectionStart">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="RED" />
      </bind> 
      <bind role="start" component="text2"  />
    </link>

     <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="GREEN" />
      </bind> 
      <bind role="set" component="text1" interface="width">
        <bindParam name="var2" value="100%"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionStop">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="YELLOW" />
      </bind> 
      <bind role="stop" component="text1"  />
    </link>

     <link xconnector="onKeySelectionStart">
      <bind role="onSelection" component="text2">
         <bindParam name="var" value="BLUE" />
      </bind> 
      <bind role="start" component="text1"  />
    </link>

  </body>
</ncl>
