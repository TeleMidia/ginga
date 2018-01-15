<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="menuEx" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
  <head>
    <regionBase>
      <region id='reg' width='100%' height='70%'/>
      <region id='reg2' top="50%" width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
    <connectorBase>

      <causalConnector id="onBeginStart">
         <simpleCondition role="onBegin" />
         <simpleAction role="start" />
      </causalConnector>

       <causalConnector id="onBeginStop">
         <simpleCondition role="onBegin" />
         <simpleAction role="stop" />
      </causalConnector>

       <causalConnector id="onEndStop">
         <simpleCondition role="onEnd" />
         <simpleAction role="stop" />
      </causalConnector>

       <causalConnector id="onEndStart">
         <simpleCondition role="onEnd" />
         <simpleAction role="start" />
      </causalConnector>

      <causalConnector id="onKeySelectionStart">
        <connectorParam name="var"/>
        <simpleCondition role="onSelection" key="$var" />
        <simpleAction role="start" />
      </causalConnector>

    </connectorBase>
  </head>
  <body>
     <port id='start' component='text1'/>
     <media id='text1' src='samples/animGar.mp4' descriptor='desc' type='video/x-new-video-player'>
         <area id="segText" begin="3s" end="7s" />
         <area id="segText2" begin="5s" end="10s" />
     </media>
    
     <media id='text2' src='samples/gnu.png' descriptor='desc2' >
          <area id="segText" begin="6s" end="8s" />
     </media>
     
    <link xconnector="onBeginStart">
        <bind role="onBegin" component="text1" interface="segText" />
        <bind role="start" component="text2"  />
     </link>
   
    <link xconnector="onBeginStop">
        <bind role="onBegin" component="text1" interface="segText2" />
        <bind role="stop" component="text2"  />
    </link>

     <link xconnector="onEndStart">
        <bind role="onEnd" component="text1" interface="segText" />
        <bind role="start" component="text2"  />
     </link>

    <link xconnector="onBeginStop">
        <bind role="onBegin" component="text2" interface="segText" />
        <bind role="stop" component="text1"  />
    </link>

    <link xconnector="onEndStart">
        <bind role="onEnd" component="text2" interface="segText" />
        <bind role="start" component="text1"  />
     </link>

  </body>
</ncl>
