<ncl>
  <body>
    <port id='port0' component='c1' interface='port1'/>
    <context id='c1'>
      <port id='port1' component='c2'/>
      <context id='c2'>
        <property name='p1' value='0'/>
        <port id='port2' component='m1'/>
        <port id='port3' component='m2'/>
        <media id='m1'/>
        <media id='m2'/>
      </context>
    </context>
  </body>
</ncl>