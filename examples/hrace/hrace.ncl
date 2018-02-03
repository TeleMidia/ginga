<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- $Id: main.ncl,v 1.3 2009/02/11 18:53:13 root Exp $ -->
<!--
main.ncl
06feb2009, gflima@telemidia.puc-rio.br

Corrida de cavalos.
-->

<ncl id="hrace" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
<head>
  <regionBase device="1">
    <region id="rgScreen" left="80" top="60" width="640" height="480" zIndex="0">
      <region id="rgHeader" width="640" height="100"
              top="0" left="0" zIndex="1" />

      <!-- Race -->
      <region id="rgTrack" width="640" height="250"
              top="100" left="0" zIndex="1">

        <!-- Stripes (on track winner notification) -->
        <region id="rgStripeRed" width="496" height="23"
                top="56" left="72" zIndex="2" />

        <region id="rgStripeGreen" width="529" height="32"
                top="76" left="55" zIndex="2" />

        <region id="rgStripeYellow" width="570" height="40"
                top="104" left="35" zIndex="2" />

        <region id="rgStripeBlue" width="631" height="58"
                top="139" left="4" zIndex="2" />

        <!-- Horses initial image -->
        <region id="rgStanding" width="97" height="139"
                top="32" left="59" zIndex="3" />

        <!-- Lua -->
        <region id="rgLuaHorses" width="640" height="250"
                top="0" left="0" zIndex="5" />
      </region>

      <!-- Menus and notifications -->
      <region id="rgFooter" width="640" height="130"
              top="350" left="0" zIndex="1">

        <!-- Bet menu -->
        <region id="rgBetMenuTitle" width="215" height="130"
                top="0" left="0" zIndex="2" />

        <region id="rgBetMenuRed" width="101" height="130"
                top="0" left="215" zIndex="2" />

        <region id="rgBetMenuGreen" width="96" height="130"
                top="0" left="316" zIndex="2" />

        <region id="rgBetMenuYellow" width="99" height="130"
                top="0" left="412" zIndex="2" />

        <region id="rgBetMenuBlue" width="129" height="130"
                top="0" left="511" zIndex="2" />

        <!-- Result -->
        <region id="rgResult" width="490" height="130"
                top="0" left="0" zIndex="2" />

        <region id="rgPlayAgain" width="150" height="65"
                top="0" left="490" zIndex="2" />

        <region id="rgExit" width="150" height="65"
                top="65" left="490" zIndex="2" />
      </region>
    </region>
  </regionBase>

  <descriptorBase>
    <descriptor id="dHeader" region="rgHeader" />
    <descriptor id="dTrack" region="rgTrack" />

    <!-- Race -->
    <descriptor id="dStripeRed" region="rgStripeRed" />
    <descriptor id="dStripeGreen" region="rgStripeGreen" />
    <descriptor id="dStripeYellow" region="rgStripeYellow" />
    <descriptor id="dStripeBlue" region="rgStripeBlue" />
    <descriptor id="dStanding" region="rgStanding" />
    <descriptor id="dLuaHorses" region="rgLuaHorses" />

    <!-- Bet menu -->
    <descriptor id="dBetMenuTitle" region="rgBetMenuTitle" />
    <descriptor id="dBetMenuRed" region="rgBetMenuRed" focusIndex="0"
                moveUp="2" moveDown="1"
                moveLeft="2" moveRight="1"
                focusSrc="media/bet_red_on.png"
                focusSelSrc="media/bet_red_on.png"
                focusBorderWidth="0" />

    <descriptor id="dBetMenuGreen" region="rgBetMenuGreen"
                focusIndex="1"
                moveUp="0" moveDown="3"
                moveLeft="0" moveRight="3"
                focusSrc="media/bet_green_on.png"
                focusSelSrc="media/bet_green_on.png"
                focusBorderWidth="0" />

    <descriptor id="dBetMenuYellow" region="rgBetMenuYellow"
                focusIndex="3"
                moveUp="1" moveDown="2"
                moveLeft="1" moveRight="2"
                focusSrc="media/bet_yellow_on.png"
                focusSelSrc="media/bet_yellow_on.png"
                focusBorderWidth="0" />

    <descriptor id="dBetMenuBlue" region="rgBetMenuBlue"
                focusIndex="2"
                moveUp="3" moveDown="0"
                moveLeft="3" moveRight="0"
                focusSrc="media/bet_blue_on.png"
                focusSelSrc="media/bet_blue_on.png"
                focusBorderWidth="0" />

    <!-- Result -->
    <descriptor id="dResult" region="rgResult" />
    <descriptor id="dPlayAgain" region="rgPlayAgain"
                focusIndex="4"
                moveUp="5" moveDown="5"
                moveLeft="5" moveRight="5"
                focusSrc="media/play_again_on.png"
                focusSelSrc="media/play_again_on.png"
                focusBorderWidth="0" />
    <descriptor id="dExit" region="rgExit"
                focusIndex="5"
                moveUp="4" moveDown="4"
                moveLeft="4" moveRight="4"
                focusSrc="media/exit_on.png"
                focusSelSrc="media/exit_on.png"
                focusBorderWidth="0" />
  </descriptorBase>

  <connectorBase>
    <causalConnector id="onBeginSet">
      <connectorParam name="val"/>
      <simpleCondition role="onBegin" />
      <simpleAction role="set" value="$val" max="unbounded" />
    </causalConnector>

    <causalConnector id="onBeginSetStart">
      <connectorParam name="val"/>
      <simpleCondition role="onBegin" />
      <compoundAction operator="seq">
        <simpleAction role="set" value="$val" max="unbounded" />
        <simpleAction role="start" max="unbounded" />
      </compoundAction>
    </causalConnector>

    <causalConnector id="onBeginStopStart">
      <simpleCondition role="onBegin" />
      <compoundAction operator="seq">
        <simpleAction role="stop" max="unbounded" />
        <simpleAction role="start" max="unbounded" />
      </compoundAction>
    </causalConnector>

    <causalConnector id="onKeySelectionStop">
      <connectorParam name="key" />
      <compoundCondition operator="or">
        <simpleCondition role="onKeySelection" eventType="selection"
                         transition="starts" key="$key" />
        <simpleCondition role="onSelection" key="$key" />
      </compoundCondition>
      <simpleAction role="stop" max="unbounded" />
    </causalConnector>

    <causalConnector id="onKeySelectionStopStart">
      <connectorParam name="key" />
      <compoundCondition operator="or">
        <simpleCondition role="onKeySelection" eventType="selection"
                         transition="starts" key="$key" />
        <simpleCondition role="onSelection" key="$key" />
      </compoundCondition>
      <compoundAction operator="seq">
        <simpleAction role="stop" max="unbounded" />
        <simpleAction role="start" max="unbounded" />
      </compoundAction>
    </causalConnector>

    <causalConnector id="onKeySelectionSetStopStart">
      <connectorParam name="key" />
      <connectorParam name="val" />
      <compoundCondition operator="or">
        <simpleCondition role="onKeySelection" eventType="selection"
                         transition="starts" key="$key" />
        <simpleCondition role="onSelection" key="$key" />
      </compoundCondition>
      <compoundAction operator="seq">
        <simpleAction role="set" value="$val" max="unbounded" />
        <simpleAction role="stop" max="unbounded" />
        <simpleAction role="start" max="unbounded" />
      </compoundAction>
    </causalConnector>
  </connectorBase>

  <ruleBase>
    <!-- Red results -->
    <compositeRule id="rBetRedWin" operator="and">
      <rule id="rBetRed0" var="player_bet" comparator="eq" value="red" />
      <rule id="rRedWin" var="winner" comparator="eq" value="red" />
    </compositeRule>

    <compositeRule id="rBetRedLose" operator="and">
      <rule id="rBetRed1" var="player_bet" comparator="eq" value="red" />
      <rule id="rRedLose" var="winner" comparator="ne" value="red" />
    </compositeRule>

    <!-- Green results -->
    <compositeRule id="rBetGreenWin" operator="and">
      <rule id="rBetGreen0" var="player_bet" comparator="eq" value="green" />
      <rule id="rGreenWin" var="winner" comparator="eq" value="green" />
    </compositeRule>

    <compositeRule id="rBetGreenLose" operator="and">
      <rule id="rBetGreen1" var="player_bet" comparator="eq" value="green" />
      <rule id="rGreenLose" var="winner" comparator="ne" value="green" />
    </compositeRule>

    <!-- Yellow results -->
    <compositeRule id="rBetYellowWin" operator="and">
      <rule id="rBetYellow0" var="player_bet" comparator="eq"
            value="yellow" />
      <rule id="rYellowWin1" var="winner" comparator="eq" value="yellow" />
    </compositeRule>

    <compositeRule id="rBetYellowLose" operator="and">
      <rule id="rBetYellow" var="player_bet" comparator="eq"
            value="yellow" />
      <rule id="rYellowLose" var="winner" comparator="ne" value="yellow" />
    </compositeRule>

    <!-- Blue results -->
    <compositeRule id="rBetBlueWin" operator="and">
      <rule id="rBetBlue0" var="player_bet" comparator="eq" value="blue" />
      <rule id="rBlueWin" var="winner" comparator="eq" value="blue" />
    </compositeRule>

    <compositeRule id="rBetBlueLose" operator="and">
      <rule id="rBetBlue1" var="player_bet" comparator="eq" value="blue" />
      <rule id="rBlueLose" var="winner" comparator="ne" value="blue" />
    </compositeRule>
  </ruleBase>
</head>

<body>
  <port id="pHeader" component="header" />
  <port id="pTrack" component="track" />
  <port id="pBetMenu" component="bet_menu" />

  <media id="header" src="media/header.png" descriptor="dHeader" />
  <media id="track" src="media/bg.png" descriptor="dTrack" />
  <media id="standing" src="media/standing.png" descriptor="dStanding" />

  <media id="settings" type="application/x-ginga-settings">
    <property name="service.currentFocus" value="0" />
    <property name="player_bet" value="empty" />
    <property name="winner" value="empty" />
  </media>

  <context id="bet_menu">
    <port id="pTitle" component="bm_title" />
    <port id="pRed0" component="bm_red" />
    <port id="pGreen0" component="bm_green" />
    <port id="pYellow0" component="bm_yellow" />
    <port id="pBlue0" component="bm_blue" />

    <media id="bm_title" src="media/bet_title.png"
           descriptor="dBetMenuTitle" />
    <media id="bm_red" src="media/bet_red_off.png"
           descriptor="dBetMenuRed" />
    <media id="bm_green" src="media/bet_green_off.png"
           descriptor="dBetMenuGreen" />
    <media id="bm_yellow" src="media/bet_yellow_off.png"
           descriptor="dBetMenuYellow" />
    <media id="bm_blue" src="media/bet_blue_off.png"
           descriptor="dBetMenuBlue" />
  </context>

  <context id="stripes">
    <port id="pRed" component="stripe_red" />
    <port id="pGreen" component="stripe_green" />
    <port id="pYellow" component="stripe_yellow" />
    <port id="pBlue" component="stripe_blue" />

    <media id="stripe_red" src="media/stripe_red.png"
           descriptor="dStripeRed" />
    <media id="stripe_green" src="media/stripe_green.png"
           descriptor="dStripeGreen" />
    <media id="stripe_yellow" src="media/stripe_yellow.png"
           descriptor="dStripeYellow" />
    <media id="stripe_blue" src="media/stripe_blue.png"
           descriptor="dStripeBlue" />
  </context>

  <!-- Sounds -->
  <media id="start_sound" src="media/start.mp3">
    <area id="go" begin="5.8s" />
  </media>
  <media id="galloping_sound" src="media/galloping.mp3" />

  <!-- Result evaluation -->
  <switch id="result">
    <!-- Player wins -->
    <bindRule rule="rBetRedWin" constituent="win_red" />
    <context id="win_red">
      <port id="pWinRedImg" component="win_red_img" />
      <port id="pApplauseSound0" component="applause_sound0" />
      <media id="win_red_img" src="media/win_red.png"
             descriptor="dResult" />
      <media id="applause_sound0" src="media/applause.mp3" />
    </context>

    <bindRule rule="rBetGreenWin" constituent="win_green" />
    <context id="win_green">
      <port id="pWinGreenImg" component="win_green_img" />
      <port id="pApplauseSound1" component="applause_sound1" />
      <media id="win_green_img" src="media/win_green.png"
             descriptor="dResult" />
      <media id="applause_sound1" src="media/applause.mp3" />
    </context>

    <bindRule rule="rBetYellowWin" constituent="win_yellow" />
    <context id="win_yellow">
      <port id="pWinYellowImg" component="win_yellow_img" />
      <port id="pApplauseSound2" component="applause_sound2" />
      <media id="win_yellow_img" src="media/win_yellow.png"
             descriptor="dResult" />
      <media id="applause_sound2" src="media/applause.mp3" />
    </context>

    <bindRule rule="rBetBlueWin" constituent="win_blue" />
    <context id="win_blue">
      <port id="pWinBlueImg" component="win_blue_img" />
      <port id="pApplauseSound3" component="applause_sound3" />
      <media id="win_blue_img" src="media/win_blue.png"
             descriptor="dResult" />
      <media id="applause_sound3" src="media/applause.mp3" />
    </context>

    <!-- Player loses -->
    <bindRule rule="rBetRedLose" constituent="lose_red" />
    <context id="lose_red">
      <port id="pLoseRedImg" component="lose_red_img" />
      <port id="pLoseSound0" component="lose_sound0" />
      <media id="lose_red_img" src="media/lose_red.png"
             descriptor="dResult" />
      <media id="lose_sound0" src="media/lose.mp3" />
    </context>

    <bindRule rule="rBetGreenLose"  constituent="lose_green" />
    <context id="lose_green">
      <port id="pLoseGreenImg" component="lose_green_img" />
      <port id="pLoseSound1" component="lose_sound1" />
      <media id="lose_green_img" src="media/lose_green.png"
             descriptor="dResult" />
      <media id="lose_sound1" src="media/lose.mp3" />
    </context>

    <bindRule rule="rBetYellowLose" constituent="lose_yellow" />
    <context id="lose_yellow">
      <port id="pLoseYellowImg" component="lose_yellow_img" />
      <port id="pLoseSound2" component="lose_sound2" />
      <media id="lose_yellow_img" src="media/lose_yellow.png"
             descriptor="dResult" />
      <media id="lose_sound2" src="media/lose.mp3" />
    </context>

    <bindRule rule="rBetBlueLose" constituent="lose_blue" />
    <context id="lose_blue">
      <port id="pLoseBlueImg" component="lose_blue_img" />
      <port id="pLoseSound3" component="lose_sound3" />
      <media id="lose_blue_img" src="media/lose_blue.png"
             descriptor="dResult" />
      <media id="lose_sound3" src="media/lose.mp3" />
    </context>
  </switch>

  <!-- Play again or exit -->
  <context id="end_menu">
    <port id="pExit" component="em_exit" />
    <port id="pPlayAgain" component="em_play_again" />
    <media id="em_exit" src="media/exit_off.png" descriptor="dExit" />
    <media id="em_play_again" src="media/play_again_off.png"
           descriptor="dPlayAgain" />
  </context>

  <media id="lua_horses" src="hrace.lua" descriptor="dLuaHorses">
    <area id="red" label="red" />
    <area id="green" label="green" />
    <area id="yellow" label="yellow" />
    <area id="blue" label="blue" />
  </media>

  <!-- Links -->

  <!-- Initialize settings -->
  <link xconnector="onBeginSet">
    <bind component="bet_menu" interface="pRed0" role="onBegin" />
    <bind component="settings" interface="player_bet" role="set">
      <bindParam name="val" value="empty"/>
    </bind>
    <bind component="settings" interface="winner" role="set">
      <bindParam name="val" value="empty"/>
    </bind>
    <bind component="settings" interface="service.currentFocus" role="set">
      <bindParam name="val" value="0"/>
    </bind>
  </link>

  <!-- Bet red -->
  <link xconnector="onKeySelectionSetStopStart">
    <bind component="bet_menu" interface="pRed0" role="onSelection" />
    <bind component="bet_menu" interface="pRed0" role="onKeySelection">
      <bindParam name="key" value="RED" />
    </bind>
    <bind component="settings" interface="player_bet" role="set">
      <bindParam name="val" value="red"/>
    </bind>
    <bind component="bet_menu" role="stop" />
    <bind component="standing" role="start" />
    <bind component="start_sound" role="start" />
  </link>

  <!-- Bet green -->
  <link xconnector="onKeySelectionSetStopStart">
    <bind component="bet_menu" interface="pGreen0" role="onSelection" />
    <bind component="bet_menu" interface="pGreen0" role="onKeySelection">
      <bindParam name="key" value="GREEN" />
    </bind>
    <bind component="settings" interface="player_bet" role="set">
      <bindParam name="val" value="green"/>
    </bind>
    <bind component="bet_menu" role="stop" />
    <bind component="standing" role="start" />
    <bind component="start_sound" role="start" />
  </link>

  <!-- Bet yellow -->
  <link xconnector="onKeySelectionSetStopStart">
    <bind component="bet_menu" interface="pYellow0" role="onSelection" />
    <bind component="bet_menu" interface="pYellow0" role="onKeySelection">
      <bindParam name="key" value="YELLOW" />
    </bind>
    <bind component="settings" interface="player_bet" role="set">
      <bindParam name="val" value="yellow"/>
    </bind>
    <bind component="bet_menu" role="stop" />
    <bind component="standing" role="start" />
    <bind component="start_sound" role="start" />
  </link>

  <!-- Bet blue -->
  <link xconnector="onKeySelectionSetStopStart">
    <bind component="bet_menu" interface="pBlue0" role="onSelection" />
    <bind component="bet_menu" interface="pBlue0" role="onKeySelection">
      <bindParam name="key" value="BLUE" />
    </bind>
    <bind component="settings" interface="player_bet" role="set">
      <bindParam name="val" value="blue"/>
    </bind>
    <bind component="bet_menu" role="stop" />
    <bind component="standing" role="start" />
    <bind component="start_sound" role="start" />
  </link>

  <!-- Start race -->
  <link xconnector="onBeginStopStart">
    <bind component="start_sound" interface="go" role="onBegin"/>
    <bind component="standing" role="stop" />
    <bind component="galloping_sound" role="start" />
    <bind component="lua_horses" role="start" />
  </link>

  <!-- Red won -->
  <link xconnector="onBeginSetStart">
    <bind component="lua_horses" interface="red" role="onBegin"/>
    <bind component="settings" interface="winner" role="set">
      <bindParam name="val" value="red" />
    </bind>
    <bind component="stripes" interface="pRed" role="start" />
    <bind component="result" role="start" />
  </link>

  <!-- Green won -->
  <link xconnector="onBeginSetStart">
    <bind component="lua_horses" interface="green" role="onBegin"/>
    <bind component="settings" interface="winner" role="set">
      <bindParam name="val" value="green" />
    </bind>
    <bind component="stripes" interface="pGreen" role="start" />
    <bind component="result" role="start" />
  </link>

  <!-- Yellow won -->
  <link xconnector="onBeginSetStart">
    <bind component="lua_horses" interface="yellow" role="onBegin"/>
    <bind component="settings" interface="winner" role="set">
      <bindParam name="val" value="yellow" />
    </bind>
    <bind component="stripes" interface="pYellow" role="start" />
    <bind component="result" role="start" />
  </link>

  <!-- Blue won -->
  <link xconnector="onBeginSetStart">
    <bind component="lua_horses" interface="blue" role="onBegin"/>
    <bind component="settings" interface="winner" role="set">
      <bindParam name="val" value="blue" />
    </bind>
    <bind component="stripes" interface="pBlue" role="start" />
    <bind component="result" role="start" />
  </link>

  <!-- Show end menu -->
  <link xconnector="onBeginStopStart">
    <bind component="result" role="onBegin" />
    <bind component="galloping_sound" role="stop" />
    <bind component="end_menu" role="start" />
  </link>
  <link xconnector="onBeginSet">
    <bind component="end_menu" interface="pPlayAgain" role="onBegin" />
    <bind component="settings" interface="service.currentFocus" role="set">
      <bindParam name="val" value="4"/>
    </bind>
  </link>

  <!-- Play again -->
  <link xconnector="onKeySelectionStopStart">
    <bind component="end_menu" interface="pPlayAgain" role="onSelection" />
    <bind component="end_menu" interface="pPlayAgain" role="onKeySelection">
      <bindParam name="key" value="GREEN" />
    </bind>
    <bind component="result" role="stop" />
    <bind component="end_menu" role="stop" />
    <bind component="lua_horses" role="stop" />
    <bind component="stripes" role="stop" />
    <bind component="bet_menu" role="start" />
  </link>

  <!-- Exit -->
  <link xconnector="onKeySelectionStop">
    <bind component="end_menu" interface="pExit" role="onSelection" />
    <bind component="end_menu" interface="pExit" role="onKeySelection">
      <bindParam name="key" value="RED" />
    </bind>
    <bind component="result" role="stop" />
    <bind component="end_menu" role="stop" />
    <bind component="lua_horses" role="stop" />
    <bind component="stripes" role="stop" />
    <bind component="track" role="stop" />
    <bind component="header" role="stop" />
  </link>
</body>
</ncl>
