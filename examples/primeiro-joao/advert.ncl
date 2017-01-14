<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Exemplo de preparação para NCL aninhado-->
<ncl id="_00prepPassiveDevicesEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id="backgroundReg" width="100%" height="100%" zIndex="5">
        <region id="iconReg" width="100%" height="100%" zIndex="6"/>
        <region id="shoesReg" left="5%" top="30%" width="40%" height="40%" zIndex="6"/>
        <region id="formReg" left="50%" top="5%" width="45%" height="90%" zIndex="6"/>
      </region>
    </regionBase>
    <descriptorBase>
      <descriptor id="backgroundDesc" region="backgroundReg" explicitDur="12s"/>
      <descriptor id="iconDesc" region="iconReg" explicitDur="6s" focusIndex="1"/>
      <descriptor id="shoesDesc" region="shoesReg"/>
      <descriptor id="formDesc" region="formReg" focusIndex="2" explicitDur="12s"/>
    </descriptorBase>
    <connectorBase>
      <importBase documentURI="causalConnBase.ncl" alias="conEx"/>
    </connectorBase>
  </head>
  <body id="adv">
    <port id="pIcon" component="icon"/>
    <media id="icon" src="../media/iconPassive.png" descriptor="iconDesc"/>
    <media id="background" src="../media/backgroundPassive.png" descriptor="backgroundDesc"/>
    <media id="shoes" src="../media/shoes.mp4" descriptor="shoesDesc"/>
    <media id="ptForm" src="../media/ptForm.htm" descriptor="formDesc"/>
    <link id="lBegingAdvert" xconnector="conEx#onSelectionStopStart">
      <bind role="onSelection" component="icon"/>
      <bind role="stop" component="icon"/>
      <bind role="start" component="background"/>
      <bind role="start" component="ptForm"/>
      <bind role="start" component="shoes"/>
    </link>
    <link xconnector="conEx#onEndStop">
      <bind role="onEnd" component="ptForm"/>
      <bind role="stop" component="adv"/>
    </link>
  </body>
</ncl>
