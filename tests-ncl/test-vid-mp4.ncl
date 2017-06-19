<ncl>
  <head>
    <regionBase>
      <region id="reg" width="100%" height="100%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc" region="reg"/>
    </descriptorBase>
  </head>
  <body>
    <port id="start" component="vid"/>
    <media id="vid" src="samples/animGar.mp4" descriptor="desc"/>
  </body>
</ncl>
