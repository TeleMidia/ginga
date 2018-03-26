<ncl>
  <head>
    <regionBase>
      <region id='reg1' left='0' top='0' width='50%' height='50%'/>
      <region id='reg2' left='50%' top='0' width='50%' height='50%'/>
      <region id='reg3' left='0' top='50%' width='50%' height='50%'/>
      <region id='reg4' left='50%' top='50%' width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc1' region='reg1' transIn="trans1" transOut="trans1" />
      <descriptor id='desc2' region='reg2' transIn="trans2" transOut="trans2" />
      <descriptor id='desc3' region='reg3' transIn="trans3" transOut="trans3" />
      <descriptor id='desc4' region='reg4' transIn="trans4" transOut="trans4" />
    </descriptorBase>
    <transitionBase>
      <transition id="trans1" type="fade" dur="1s"/>
      <transition id="trans2" type="fade" dur="1s"/>
      <transition id="trans3" type="fade" dur="1s"/>
      <transition id="trans4" type="fade" dur="1s"/>
    </transitionBase>
  </head>
  <body>
    <port id='port1' component='vid1'/>
    <port id='port2' component='vid2'/>
    <port id='port3' component='vid3'/>
    <port id='port4' component='vid4'/>

    <media id='vid1' src='samples/animGar.mp4' descriptor='desc1' />
    <media id='vid2' src='samples/animGar.mp4' descriptor='desc2' />
    <media id='vid3' src='samples/animGar.mp4' descriptor='desc3' />
    <media id='vid4' src='samples/animGar.mp4' descriptor='desc4' />
  </body>
</ncl>
