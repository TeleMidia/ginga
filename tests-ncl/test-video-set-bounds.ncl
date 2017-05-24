<ncl>
  <head>
    <regionBase>
      <region id='reg' width='100%' height='100%'/>
      <region id='reg4' width='320' height='240' left='320'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc4' region='reg4'/>
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

      <causalConnector id="onResumeStart">
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
      </causalConnector>
      
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
    <port id='start2' component='video'/>
    <port id='start4' component='img'/>
    
    <media id='img' src='samples/gnu.png' descriptor='desc4' />
    
    <media id='video' src='samples/bunny.ogg' descriptor='desc' >
      <property name='bounds'/>
      <area id='area1' begin='5s'/>
      <area id='area4' begin='10s'/>
    </media>

    <link xconnector="onBeginSet">
      <bind role='onBegin' component='video' interface='area1'/>
      <bind role="set" component="video" interface="bounds">
        <bindParam name="var" value="5%,6.67%,45%,45%"/>
      </bind>
    </link>

    <link xconnector="onKeySelectionSet">
      <bind role='onSelection' component='img'>
        <bindParam name='keyCode' value='RED'/>        
      </bind>
      <bind role="set" component="video" interface="bounds">
        <bindParam name="var" value="50%,6.67%,45%,45%"/>
      </bind>
    </link>

  </body>
</ncl>
