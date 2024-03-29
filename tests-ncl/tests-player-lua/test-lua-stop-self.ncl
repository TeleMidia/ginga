<ncl>
  <head>
    <regionBase>
      <region id='reg1' left='0' top='0' width='50%' height='50%'/>
      <region id='reg2' left='50%' top='0' width='50%' height='50%'/>
      <region id='reg3' left='0' top='50%' width='50%' height='50%'/>
      <region id='reg4' left='50%' top='50%' width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc1" region="reg1"/>
      <descriptor id="desc2" region="reg2"/>
      <descriptor id="desc3" region="reg3"/>
      <descriptor id="desc4" region="reg4"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="m"/>
    <port id="startctx" component="ctx"/>
    <media id="m" src="../samples/fps.lua" descriptor="desc1">
      <area id="a1" begin="2s"/>
      <area id="a2" begin="4s"/>
    </media>
    <context id="ctx">
      <port id="startm2" component="m2"/>
      <media id="m2" src="../samples/fps.lua" descriptor="desc4"/>
    </context>
    <link xconnector="onBeginStop">
      <bind role="onBegin" component="m" interface="a1"/>
      <bind role="stop" component="ctx" interface="startm2"/>
    </link>
    <link xconnector="onBeginStop">
      <bind role="onBegin" component="m" interface="a2"/>
      <bind role="stop" component="m"/>
    </link>
  </body>
</ncl>