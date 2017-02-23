<ncl>
  <head>
    <regionBase>
      <region id='reg' width='50%' height='50%'/>
      <region id='reg2' width='100%' height='50%'/>
    </regionBase>
    <descriptorBase>
       <descriptor id='desc' region='reg'/>
       <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
    
  <connectorBase>
      <causalConnector id="onBeginStart">
          <connectorParam name="var"  />
          <simpleCondition role="onBegin"/>
          <simpleAction role="set" value="$var" max="unbounded" qualifier="seq" />
      </causalConnector>
  </connectorBase>

  </head>
  <body>
    <port id='start' component='text1'/>
  
    <media id='text1' src='samples/text.txt' descriptor='desc'>
       <property name="fontColor" value="#0" />
       <property name="backgroundColor" value="#F" />       
       <property name="fontFamily" value="Courier" />
       <property name="textAlign" value="center" />
       <property name="fontStyle" value="normal" />
       <property name="fontSize" value="18px" />

       <area id="segText" begin="2s"/>
    </media>

     <media id='text2' src='samples/text.txt' descriptor='desc2'>
       <property name="fontColor" value="#0" />
       <property name="backgroundColor" value="#F" />       
       <property name="fontFamily" value="Courier" />
       <property name="textAlign" value="center" />
       <property name="fontStyle" value="normal" />
       <property name="fontSize" value="18px" />
    </media>

    <link id="linkText" xconnector="onBeginStart">
        <bind role="onBegin" component="text1"  /> 
        <bind role="set" component="text1" interface="width">
            <bindParam name="var" value="100%"/> 
        </bind>
    </link>
   

  </body>
</ncl>
