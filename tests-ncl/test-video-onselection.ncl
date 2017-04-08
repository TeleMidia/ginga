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
    <port id='start' component='vid1'/>
    <media id='vid1' src='samples/animGar.mp4' descriptor='desc' type='video/x-new-video-player'>
      <property name="width"/>
    </media>
  
    
    
     <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="vid1">
         <bindParam name="var" value="RED" />
      </bind> 
      <bind role="set" component="vid1" interface="width">
        <bindParam name="var2" value="100%"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionStop">
      <bind role="onSelection" component="vid1">
         <bindParam name="var" value="GREEN" />
      </bind> 
      <bind role="stop" component="vid1"  />
    </link>

    

  </body>
</ncl>
