<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="nclClicks" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">

<head>

<regionBase>
  <region width="30%" height="30%" left="10%" top="40%" id="rg1"/>
</regionBase>

<descriptorBase>
  <descriptor id="ds1" region="rg1"/>
</descriptorBase>

</head>

<body>
  <port id="entryPoint" component="lua"/>
  <media id="lua" src="samples/randomprop.lua" descriptor="ds1">
    <property name="max_value" value="500"/>
  </media>
</body>

</ncl>

