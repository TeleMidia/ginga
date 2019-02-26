<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="preparationExample" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regVideo" width="100%" height="100%" zIndex="2"/>
			<region id="regLoading" width="100%" height="100%" zIndex="1"/>
			<region id="regBtnPlay" top="45%" left="35%" width="30%" height="13%" zIndex="2"/>
		</regionBase>

		<descriptorBase>
			<descriptor id="descVideo" region="regVideo" explicitDur="5s"/>
			<descriptor id="descLoading" region="regLoading"/>
			<descriptor id="descBtnPlay" region="regBtnPlay"/>
		</descriptorBase>

		<connectorBase>
			<causalConnector id="onBeginPreparationStart">
				<simpleCondition role="onBeginPreparation"/>
				<simpleAction role="start"/>
			</causalConnector>
			<causalConnector id="onEndPreparationStartStop">
				<simpleCondition role="onEndPreparation"/>
				<simpleAction role="start"/>
				<simpleAction role="stop"/>
			</causalConnector>
			<causalConnector id="onKeySelStartPreparationStop">
				<connectorParam name="key"/>
				<simpleCondition role="onSelection" key="$key"/>
				<simpleAction role="startPreparation"/>
				<simpleAction role="stop"/>      
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		<port id="port1" component="btnPlay"/>

		<media id="video" src="samples/bunny.ogg" descriptor="descVideo">
			<area id="area1" end="5s"/>
		</media>
		<media id="btnPlay" src="samples/watch-button.png" descriptor="descBtnPlay"/>
		<media id="loading" src="samples/clock.ogv" descriptor="descLoading"/>

		<link xconnector="onKeySelStartPreparationStop">
			<bind role="onSelection" component="btnPlay">
				<bindParam name="key" value="ENTER"/>
			</bind>
			<bind role="startPreparation" component="video" interface="area1"/>
			<bind role="stop" component="btnPlay"/>
		</link>

		<link xconnector="onBeginPreparationStart">
			<bind role="onBeginPreparation" component="video" interface="area1"/>
			<bind role="start" component="loading"/>
		</link>    

		<link xconnector="onEndPreparationStartStop">
			<bind role="onEndPreparation" component="video" interface="area1"/>
			<bind role="start" component="video"/>
			<bind role="stop" component="loading"/>
		</link>
	</body>
</ncl>
