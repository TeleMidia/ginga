<?xml version="1.0" encoding="UTF-8"?>
<!-- test-bad-lua-0.ncl - Check graceful termination.
     Copyright (C) 2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. -->

<ncl id="test-bad-lua-0">
<head>
  <connectorBase>
    <importBase documentURI="connbase.ncl" alias="conn"/>
  </connectorBase>
</head>
<body>
  <port id="start" component="lua"/>
  <media id="lua" src="test-bad-lua-0.lua"/>
  <media id="exit" src="exit.lua"/>
  <link xconnector="conn#onAbortStart">
    <bind role="onAbort" component="lua"/>
    <bind role="start" component="exit"/>
  </link>
</body>
</ncl>
