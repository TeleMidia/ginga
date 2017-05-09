<ncl>
  <head>
    <regionBase>
      <region id="rgPage" width="100%" height="100%" zIndex="1"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="dsPage" region="rgPage"/>
    </descriptorBase>
  </head>
  <body>
    <port id="pPage" component="mPage"/>
    <media id="mPage" type="text/test-html" src="http://www.uol.com/" descriptor="dsPage"/>
  </body>
</ncl>
