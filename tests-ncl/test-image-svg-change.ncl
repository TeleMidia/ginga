<ncl>
  <head>
    <regionBase>
      <region id='reg' width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
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
    <port id='start' component='image'/>
    <media id='image' src='samples/vector.svg' descriptor='desc'>
      <property name="width"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image"  />
      <bind role="set" component="image" interface="width">
        <bindParam name="var" value="100%"/>
      </bind>
    </link>
  </body>
</ncl>
