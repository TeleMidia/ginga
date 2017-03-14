<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
      <region id='reg2' width='640' height='480' left='160' top='120'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <port id='start2' component='video2'/>
    <media id='video' src='samples/bunny.ogg' descriptor='desc' type='video/x-new-video-player'/>
    <media id='video2' src='samples/bunny.ogg' descriptor='desc2' type='video/x-new-video-player'/>  
  </body>
</ncl>
