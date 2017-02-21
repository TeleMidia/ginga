<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <media id='video' src='samples/bunny.ogg' descriptor='desc' type='video/x-new-video-player'/>
  </body>
</ncl>
