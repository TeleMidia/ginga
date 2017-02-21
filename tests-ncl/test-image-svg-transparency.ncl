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
    <port id='start1' component='img1'/>
    <port id='start2' component='img2'/>
    <media id='img1' src='samples/vector.svg' descriptor='desc'/>
    <media id='img2' src='samples/vector.svg'>
      <property name='top' value='50%'/>
      <property name='left' value='50%'/>
      <property name='transparency' value='50%'/>
    </media>
  </body>
</ncl>
