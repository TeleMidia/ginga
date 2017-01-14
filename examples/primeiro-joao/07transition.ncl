<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Exemplo de transicoes -->
<ncl id="switchEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <ruleBase>
      <rule id="en" var="system.language" value="eng" comparator="eq"/>
    </ruleBase>
    <transitionBase>
      <transition id="trans1" type="fade" dur="2s"/>
      <transition id="trans2" type="barWipe" dur="1s"/>
    </transitionBase>
    <regionBase>
      <region id="backgroundReg" width="100%" height="100%" zIndex="1"/>
      <region id="screenReg" width="100%" height="100%" zIndex="2">
        <region id="frameReg" left="5%" top="6.7%" width="18.5%" height="18.5%" zIndex="3"/>
        <region id="iconReg" left="87.5%" top="11.7%" width="8.45%" height="6.7%" zIndex="3"/>
        <region id="shoesReg" left="15%" top="60%" width="25%" height="25%" zIndex="3"/>
        <region id="formReg" left="57.25%" top="9.83%" width="37.75%" height="70.2%" zIndex="3"/>
      </region>
    </regionBase>
    <descriptorBase>
      <descriptor id="backgroundDesc" region="backgroundReg"/>
      <descriptor id="screenDesc" region="screenReg"/>
      <descriptor id="photoDesc" region="frameReg" explicitDur="5s"/>
      <descriptor id="audioDesc"/>
      <descriptor id="dribleDesc" region="frameReg" transIn="trans1" transOut="trans2"/>
      <descriptor id="iconDesc" region="iconReg" explicitDur="6s"/>
      <descriptor id="shoesDesc" region="shoesReg"/>
      <descriptor id="formDesc" region="formReg" focusIndex="1" explicitDur="15s"/>
    </descriptorBase>
    <connectorBase>
      <importBase documentURI="causalConnBase.ncl" alias="conEx"/>
    </connectorBase>
  </head>

  <body>
    <port id="entry" component="animation"/>
    <media id="background" src="media/background.png" descriptor="backgroundDesc"/>
    <media id="animation" src="media/animGar.mp4" descriptor="screenDesc">
      <area id="segDrible" begin="12s"/>
      <area id="segPhoto" begin="41s"/>
      <area id="segIcon" begin="45s" end="51s"/>
    </media>
    <media id="choro" src="media/choro.mp4" descriptor="audioDesc"/>
    <media id="drible" src="media/drible.mp4" descriptor="dribleDesc"/>
    <media id="photo" src="media/photo.png" descriptor="photoDesc"/>
    <context id="advert">
      <media id="reusedAnimation" refer="animation" instance="instSame">
        <property name="bounds"/>
      </media>
      <media id="icon" src="media/icon.png" descriptor="iconDesc"/>
      <media id="shoes" src="media/shoes.mp4" descriptor="shoesDesc"/>
      <switch id="form">
        <switchPort id="spForm">
          <mapping component="enForm"/>
          <mapping component="ptForm"/>
        </switchPort>
        <bindRule constituent="enForm" rule="en"/>
        <defaultComponent component="ptForm"/>
        <media id="ptForm" src="media/ptForm.htm" descriptor="formDesc"/>
        <media id="enForm" src="media/enForm.htm" descriptor="formDesc"/>
      </switch>
      <link  id="lIcon" xconnector="conEx#onBeginStart">
        <bind role="onBegin" component="reusedAnimation" interface="segIcon"/>
        <bind role="start" component="icon"/>
      </link>
      <link  id="lBegingShoes" xconnector="conEx#onKeySelectionStopSet_varStart">
        <bind role="onSelection" component="icon">
          <bindParam name="keyCode" value="RED"/>
        </bind>
        <bind role="start" component="shoes"/>
        <bind role="start" component="form" interface="spForm"/>
        <bind role="set" component="reusedAnimation" interface="bounds">
          <bindParam name="var" value="5%,6.67%,45%,45%"/>
        </bind>
        <bind role="stop" component="icon"/>
      </link>
      <link  id="lEndForm" xconnector="conEx#onEndSet_var">
        <bind role="onEnd" component="form" interface="spForm"/>
        <bind role="set" component="reusedAnimation" interface="bounds">
          <bindParam name="var" value="0,0,222.22%,222.22%"/>
        </bind>
      </link>
    </context>
    <link  id="lMusic" xconnector="conEx#onBeginStart_delay">
      <bind role="onBegin" component="animation"/>
      <bind role="start" component="background">
        <bindParam name="delay" value="5s"/>
      </bind>
      <bind role="start" component="choro">
        <bindParam name="delay" value="5s"/>
      </bind>
    </link>
    <link  id="lDrible" xconnector="conEx#onBeginStart">
      <bind role="onBegin" component="animation" interface="segDrible"/>
      <bind role="start" component="drible"/>
    </link>
    <link  id="lPhoto" xconnector="conEx#onBeginStart">
      <bind role="onBegin" component="animation" interface="segPhoto"/>
      <bind role="start" component="photo"/>
    </link>
    <link  id="lEnd" xconnector="conEx#onEndStop">
      <bind role="onEnd" component="animation"/>
      <bind role="stop" component="background"/>
      <bind role="stop" component="choro"/>
    </link>
  </body>
</ncl>
