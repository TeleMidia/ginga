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
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='text1'/>
    <media id='text1' src='samples/text.txt' descriptor='desc'>
      <property name="width"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="text1"  />
      <bind role="set" component="text1" interface="width">
        <bindParam name="var" value="100%"/>
      </bind>
    </link>
  </body>
</ncl>
