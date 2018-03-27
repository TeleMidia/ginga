<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id='reg' width='320' height='240'/>
      <region id='reg2' width='320' height='240' top='240'/> 
      <region id='reg3' width='320' height='240' left='320' top='240'/>
      <region id='reg4' width='320' height='240' left='320'/>
      <region id='reg5' width='320' height='240' top='480'/>
      <region id='reg6' width='320' height='240' left='320' top='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
      <descriptor id='desc3' region='reg3'/>
      <descriptor id='desc4' region='reg4'/>
      <descriptor id='desc5' region='reg5'/>
      <descriptor id='desc6' region='reg6'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onBeginPause">
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onBeginResume">
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onBeginAbort">
        <simpleCondition role="onBegin"/>
        <simpleAction role="abort" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <compoundAction operator="seq">
          <simpleAction role="set" value="$var"/>
        </compoundAction>
      </causalConnector>

      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onEndStop">
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onEndPause">
        <simpleCondition role="onEnd"/>
        <simpleAction role="pause" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onEndResume">
        <simpleCondition role="onEnd"/>
        <simpleAction role="resume" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onEndAbort">
        <simpleCondition role="onEnd"/>
        <simpleAction role="abort" max="unbounded" qualifier="par"/>
      </causalConnector>

      <causalConnector id="onPauseStart">
        <simpleCondition role="onPause"/>
        <simpleAction role="start" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onPauseStop">
        <simpleCondition role="onPause"/>
        <simpleAction role="stop" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onPausePause">
        <simpleCondition role="onPause"/>
        <simpleAction role="pause" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onPauseResume">
        <simpleCondition role="onPause"/>
        <simpleAction role="resume" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onPauseAbort">
        <simpleCondition role="onPause"/>
        <simpleAction role="abort" max="unbounded" qualifier="par"/>
      </causalConnector>

      <!-- <causalConnector id="onResumeStart">
        <simpleCondition role="onResume"/>
        <simpleAction role="start" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onResumeStop">
        <simpleCondition role="onResume"/>
        <simpleAction role="stop" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onResumePause">
        <simpleCondition role="onResume"/>
        <simpleAction role="pause" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onResumeResume">
        <simpleCondition role="onResume"/>
        <simpleAction role="resume" max="unbounded" qualifier="par"/>
      </causalConnector>
      <causalConnector id="onResumeAbort">
        <simpleCondition role="onResume"/>
        <simpleAction role="abort" max="unbounded" qualifier="par"/>
      </causalConnector> -->
      
      <causalConnector id="onKeySelectionStart">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="start" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onKeySelectionStop">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="stop" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onKeySelectionPause">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="pause" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onKeySelectionResume">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="resume" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onKeySelectionAbort">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="abort" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      
      <causalConnector id="onKeySelectionSet">
        <connectorParam name="var"/>
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="set" value="$var"/>
        </compoundAction>
      </causalConnector>

    </connectorBase>
  </head>
  <body>    
    <port id='start1' component='audio'/>
    <port id='start2' component='img' />
    <port id='start3' component='video'/>

    <media id='audio' src='../samples/arcade.mp3' descriptor='desc'>
      <property name='trebleLevel' value='0'/>
      <property name='bassLevel' value='0'/>
    </media>

    <media id='img' src='../samples/gnu.png' descriptor='desc4'/>
    
    <media id='video' src='../samples/bunny.ogg' descriptor='desc' > <!-- Video com audio -->
      <property name='soundLevel' value="0"/>
      <area id='videoArea1' begin='5s'/>
      <area id='videoArea2' begin='10s'/>
    </media>

    <!-- Set bassLevel audio. 
         RED: -24; GREEN: 12;-->

    <!-- Set trebleLevel audio. 
         YELLOW: -24; BLUE: 12;-->

    <!-- Set trebleLevel and bassLevel audio. 
         INFO: 0;-->
    <link xconnector="onKeySelectionSet">
      <bind role='onSelection' component='img'>
        <bindParam name='keyCode' value='RED'/>        
      </bind>
      <bind role="set" component="audio" interface="bassLevel">
        <bindParam name="var" value="-24%"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionSet">
      <bind role='onSelection' component='img'>
        <bindParam name='keyCode' value='GREEN'/>        
      </bind>
      <bind role="set" component="audio" interface="bassLevel">
        <bindParam name="var" value="12"/>
      </bind>
    </link>
    
    <link xconnector="onKeySelectionSet">
      <bind role='onSelection' component='img'>
        <bindParam name='keyCode' value='YELLOW'/>        
      </bind>
      <bind role="set" component="audio" interface="trebleLevel">
        <bindParam name="var" value="-24"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionSet">
      <bind role='onSelection' component='img'>
        <bindParam name='keyCode' value='BLUE'/>        
      </bind>
      <bind role="set" component="audio" interface="trebleLevel">
        <bindParam name="var" value="12"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionSet">
      <bind role='onSelection' component='img'>
        <bindParam name='keyCode' value='INFO'/>        
      </bind>
      <bind role="set" component="audio" interface="bassLevel">
        <bindParam name="var" value="0"/>
      </bind>
    </link>
    
    <link xconnector="onKeySelectionSet">
      <bind role='onSelection' component='img'>
        <bindParam name='keyCode' value='INFO'/>        
      </bind>
      <bind role="set" component="audio" interface="trebleLevel">
        <bindParam name="var" value="0"/>
      </bind>
    </link>

  </body>
</ncl>
