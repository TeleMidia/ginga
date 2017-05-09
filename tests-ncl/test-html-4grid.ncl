<ncl>
  <head>
    <regionBase>
      <region id="rgPage1" top="0%" left="0%" width="50%" height="50%" zIndex="1"/>
      <region id="rgPage2" top="0%" left="50%" width="50%" height="50%" zIndex="1"/>
      <region id="rgPage3" top="50%" left="0%" width="50%" height="50%" zIndex="1"/>
      <region id="rgPage4" top="50%" left="50%" width="50%" height="50%" zIndex="1"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="dsPage1" region="rgPage1"/>
      <descriptor id="dsPage2" region="rgPage2"/>
      <descriptor id="dsPage3" region="rgPage3"/>
      <descriptor id="dsPage4" region="rgPage4"/>
    </descriptorBase>
  </head>
  <body>
    <port id="pPage1" component="mPage1"/>
    <port id="pPage2" component="mPage2"/>
    <port id="pPage3" component="mPage3"/>
    <port id="pPage4" component="mPage4"/>

    <media id="mPage1" type="text/test-html" src="https://www.youtube.com/embed/cR7RdKWvejc/?autoplay=1" descriptor="dsPage1"/>
    <media id="mPage2" type="text/test-html" src="http://www.uol.com/" descriptor="dsPage2"/>
    <media id="mPage3" type="text/test-html" src="http://alteredqualia.com/three/examples/webgl_pasta.html" descriptor="dsPage3"/>
    <media id="mPage4" type="text/test-html" src="samples/page.html" descriptor="dsPage4"/>
  </body>
</ncl>
