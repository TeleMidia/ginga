<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
      <region id='reg2' width='640' height='480' top='50%' left='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg' />
      <descriptor id='desc1' region='reg2' />
    </descriptorBase>
  </head>
  <body>
    <port id='start1' component='img1'/>
    <port id='start2' component='img2'/>
    <media id='img1' src='samples/gnu.png' descriptor='desc'/>
    <media id='img2' src='samples/gnu.png' descriptor='desc1'/>
  </body>
</ncl>
