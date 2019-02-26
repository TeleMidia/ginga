<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="preparationExample" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="regLoading" width="100%" height="100%" zIndex="1"/>
			<region id="regVideo" top="50%" left="50%" width="50%" height="50%" zIndex="2"/>
		</regionBase>

		<descriptorBase>
			<descriptor id="descVideo" region="regVideo" explicitDur="5s"/>
			<descriptor id="descLoading" region="regLoading"/>
		</descriptorBase>

		<connectorBase>
			<causalConnector id="onBeginStartPreparation">
				<simpleCondition role="onBegin"/>
				<simpleAction role="startPreparation"/>
			</causalConnector>
			<causalConnector id="onEndPreparationStopStart">
				<simpleCondition role="onEndPreparation"/>
				<simpleAction role="stop"/>
				<simpleAction role="start"/>
			</causalConnector>
		</connectorBase>
	</head>
	<body>
		<port id="port1" component="loading"/>

		<media id="loading" src="samples/clock.ogv" descriptor="descLoading"/>
		<media id="video" src="samples/bunny.ogg" descriptor="descVideo"/>

		<link xconnector="onBeginStartPreparation">
			<bind role="onBegin" component="loading"/>
			<bind role="startPreparation" component="video"/>
		</link>

		<link xconnector="onEndPreparationStopStart">
			<bind role="onEndPreparation" component="video"/>
			<bind role="stop" component="loading"/>
			<bind role="start" component="video"/>
		</link>
	</body>
</ncl>
