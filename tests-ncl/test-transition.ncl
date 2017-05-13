<ncl>
  <head>
    <regionBase>
      <region id='reg1' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc1' region='reg1' transIn="trans1" transOut="trans2" />
    </descriptorBase>
    <transitionBase>
      <transition id="trans1" type="fade" dur="2s"/>
      <transition id="trans2" type="barWipe" dur="1s"/>
    </transitionBase>
  </head>
  <body>
    <port id='start1' component='text1'/>

    <media id='text1' src='samples/gnu.png' descriptor='desc1' />
  </body>
</ncl>
