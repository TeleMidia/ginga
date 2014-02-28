<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="epgFactory" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
xsi:schemaLocation="http://www.ncl.org.br/NCL3.0/EDTVProfile
http://www.ncl.org.br/NCL3.0/profiles/NCL30EDTV.xsd">

<body>
  <port id="doit" component="epgNode"/>

  <media type="application/x-ginga-EPGFactory" id="epgNode" src="epg.lua"/>
</body>
</ncl>
