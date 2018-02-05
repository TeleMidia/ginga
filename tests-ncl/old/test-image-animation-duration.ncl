<ncl>
  <head>
    <regionBase>
      <region id='reg1' width='120px' height='100px'/>
      <region id='reg2' left="200px" width='120px' height='100px'/>
      <region id='reg3' left="400px" width='120px' height='100px'/>
      <region id='reg4' left="600px" width='120px' height='100px'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc1' region='reg1'/>
      <descriptor id='desc2' region='reg2'/>
      <descriptor id='desc3' region='reg3'/>
      <descriptor id='desc4' region='reg4'/>
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
    <port id='start2' component='text2'/>
    <port id='start3' component='text3'/>
    <port id='start4' component='text4'/>

    <media id='text1' src='samples/gnu.png' descriptor='desc1'>
      <property name="top"/>
    </media>

    <media id='text2' src='samples/gnu.png' descriptor='desc2'>
      <property name="top"/>
    </media>

    <media id='text3' src='samples/gnu.png' descriptor='desc3'>
      <property name="top"/>
    </media>

     <media id='text4' src='samples/gnu.png' descriptor='desc4'>
      <property name="top"/>
    </media>

    <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text1">
         <bindParam name="var" value="RED" />
      </bind> 
      <bind role="set" component="text1" interface="top">
        <bindParam name="var2" value="350"/>
        <bindParam name="duration" value="1s"/>
      </bind>
    </link>

     <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text2">
         <bindParam name="var" value="GREEN" />
      </bind> 
      <bind role="set" component="text2" interface="top">
        <bindParam name="var2" value="350"/>
        <bindParam name="duration" value="2s"/>
      </bind>
    </link>

     <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text3">
         <bindParam name="var" value="YELLOW" />
      </bind> 
      <bind role="set" component="text3" interface="top">
        <bindParam name="var2" value="350"/>
        <bindParam name="duration" value="4s"/>
      </bind>
    </link>
   
     <link xconnector="onKeySelectionSet">
      <bind role="onSelection" component="text4">
         <bindParam name="var" value="BLUE" />
      </bind> 
      <bind role="set" component="text4" interface="top">
        <bindParam name="var2" value="350"/>
        <bindParam name="duration" value="8s"/>
      </bind>
    </link>
   
  </body>
</ncl>
