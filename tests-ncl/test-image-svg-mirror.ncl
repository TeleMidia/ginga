<ncl>
  <body>
    <port id='start1' component='img'/>
    <port id='start2' component='mir'/>
    <media id='img' src='samples/vector.svg'>
      <property name='width' value='640'/>
      <property name='height' value='480'/>
    </media>
    <media id='mir' src='ncl-mirror://img'>
      <property name='top' value='50%'/>
      <property name='left' value='50%'/>
    </media>
  </body>
</ncl>
