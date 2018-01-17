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
      <transition id="trans1" type="diagonalWipe" dur="1s"/>
      <transition id="trans2" type="diagonalWipe" dur="1s"/>
      <transition id="trans3" type="diagonalWipe" dur="1s"/>
      <transition id="trans4" type="diagonalWipe" dur="1s"/>
    </transitionBase>
  </head>
  <body>
    <port id='port1' component='img1'/>
    <port id='port2' component='img2'/>
    <port id='port3' component='img3'/>
    <port id='port4' component='img4'/>

    <media id='img1' src='samples/felis.jpg' descriptor='desc1' />
    <media id='img2' src='samples/felis.jpg' descriptor='desc2' />
    <media id='img3' src='samples/felis.jpg' descriptor='desc3' />
    <media id='img4' src='samples/felis.jpg' descriptor='desc4' />
  </body>
</ncl>
