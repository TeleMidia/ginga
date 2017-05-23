<ncl>
  <head>
    <regionBase>
      <region id='reg' width='320' height='240'/>
      <region id='reg2' width='320' height='240' left='320' top='240'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <port id='start2' component='video2'/>
    <media id='video' src='samples/bunny.ogg' descriptor='desc' />
    <media id='video2' src='samples/animGar.mp4' descriptor='desc2' />
  </body>
</ncl>
