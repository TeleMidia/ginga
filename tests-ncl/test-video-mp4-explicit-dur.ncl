<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg' explicitDur='2s'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='img'/>
    <media id='img' src='samples/animGar.mp4' descriptor='desc'/>
  </body>
</ncl>
