<ncl>
  <head>
    <regionBase>
      <region id='reg' width='100%' height='100%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='lua'/>
    <media id='lua' src='samples/fps.lua' descriptor='desc'/>
  </body>
</ncl>
