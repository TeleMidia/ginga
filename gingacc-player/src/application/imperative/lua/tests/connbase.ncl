<?xml version="1.0" encoding="UTF-8"?>
<!-- connbase.ncl - Connector base.
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

<ncl id="connbase">
<head>
  <connectorBase>
    <causalConnector id="onAbortStart">
      <simpleCondition role="onAbort"/>
      <simpleAction role="start"/>
    </causalConnector>

    <causalConnector id="onBeginAttributionStart">
      <simpleCondition role="onBeginAttribution" eventType="attribution" transition="starts"/>
      <simpleAction role="start"/>
    </causalConnector>

    <causalConnector id="onBeginSet">
      <connectorParam name="var"/>
      <simpleCondition role="onBegin"/>
      <simpleAction role="set" value="$var"/>
    </causalConnector>

    <causalConnector id="onEndAttributionStartStop">
      <simpleCondition role="onEndAttribution" eventType="attribution" transition="stops"/>
      <compoundAction operator="seq">
        <simpleAction role="start"/>
        <simpleAction role="stop"/>
      </compoundAction>
    </causalConnector>

    <causalConnector id="onEndStart">
      <simpleCondition role="onEnd"/>
      <simpleAction role="start"/>
    </causalConnector>
  </connectorBase>
</head>
</ncl>
