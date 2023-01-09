<ncl>
  <head>
    <regionBase>
      <region id='reg1' left='0' top='0' width='50%' height='50%'/>
      <region id='reg2' left='50%' top='0' width='50%' height='50%'/>
      <region id='reg3' left='0' top='50%' width='50%' height='50%'/>
      <region id='reg4' left='50%' top='50%' width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc1" region="reg1" transIn="trans1" transOut="trans1" explicitDur="2s" />
      <descriptor id="desc2" region="reg2" transIn="trans2" transOut="trans2" explicitDur="2s" />
      <descriptor id="desc3" region="reg3" transIn="trans3" transOut="trans3" explicitDur="2s" />
      <descriptor id="desc4" region="reg4" transIn="trans4" transOut="trans4" explicitDur="2s" />
    </descriptorBase>
    <transitionBase>
      <transition id="trans1" type="diagonalWipe" dur="1s"/>
      <transition id="trans2" type="diagonalWipe" dur="1s"/>
      <transition id="trans3" type="diagonalWipe" dur="1s"/>
      <transition id="trans4" type="diagonalWipe" dur="1s"/>
    </transitionBase>
    <connectorBase>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body id="body">
    <port id="port1" component="img1"/>
    <port id="port2" component="img2"/>
    <port id="port3" component="img3"/>
    <port id="port4" component="img4"/>
    <media id="img1" src="../samples/text.txt" descriptor="desc1" />
    <media id="img2" src="../samples/text.txt" descriptor="desc2" />
    <media id="img3" src="../samples/text.txt" descriptor="desc3" />
    <media id="img4" src="../samples/text.txt" descriptor="desc4" />
    <link xconnector="onEndStart">
      <bind role="onEnd" component="body"/>
      <bind role="start" component="body"/>
    </link>
  </body>
</ncl>