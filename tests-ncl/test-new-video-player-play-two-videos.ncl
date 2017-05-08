<ncl>
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


    </connectorBase>
  </head>
  <body>
     <port id='start1' component='audio'/>
    <!-- <port id='start2' component='img'/> -->
    <!-- <port id='start3' component='img2'/> -->
    <port id='start4' component='video'/>
    <port id='start5' component='video2'/>
    <!-- <port id='start6' component='video3'/> -->
    <!-- <port id='start7' component='audio'/> -->
    
    <media id='audio' src='samples/bunny.mp3'/>
    
    <media id='img' src='samples/gnu.png' descriptor='desc4' />
    <media id='img2' src='samples/gnu.png' descriptor='desc5' />
    
    <media id='video' src='samples/bunny.ogg' descriptor='desc' >
      <area id='area1' begin='5s'/>
      <area id='area4' begin='10s'/>
    </media>

    <media id='video2' src='samples/small.mp4' descriptor='desc2' >
      <area id='area2' begin='2s'/>
      <area id='area5' begin='4s'/>
    </media>

    <media id='video3' src='samples/animGar.mp4' descriptor='desc3' >
      <area id='area3' begin='2s'/>
    </media>
    
    <!-- <link xconnector="onBeginStart">
      <bind role="onBegin" component="audio" />
      <bind role="start" component="video3" />
    </link>

    <link xconnector="onEndStart">
      <bind role="onEnd" component="video2" />
      <bind role="start" component="video" />
    </link>
 
    <link xconnector="onKeySelectionStop">
      <bind role='onSelection' component='video2'>
        <bindParam name='keyCode' value='RED'/>        
      </bind>
      <bind role="stop" component="video2" />
    </link> -->

  </body>
</ncl>
