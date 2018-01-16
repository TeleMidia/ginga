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
      <descriptor id='desc2' region='reg2' transIn="trans1" transOut="trans1" />
      <descriptor id='desc3' region='reg3' transIn="trans2" transOut="trans2" />
      <descriptor id='desc4' region='reg4' transIn="trans3" transOut="trans3" />
    </descriptorBase>
    <transitionBase>
      <transition id="trans1" type="fade" dur="1s"/>
      <transition id="trans2" type="barWipe" dur="1s"/>
      <transition id="trans3" type="barWipe" subtype="topToBottom" dur="1s"/>
    </transitionBase>
  </head>
  <body>
   
    <port id='start4' component='img4'/>

    <media id='img1' src='samples/gnu.png' descriptor='desc1' />
    <media id='img2' src='samples/gnu.png' descriptor='desc2' />
    <media id='img3' src='samples/gnu.png' descriptor='desc3' />
    <media id='img4' src='samples/gnu.png' descriptor='desc4' />
  </body>
</ncl>
