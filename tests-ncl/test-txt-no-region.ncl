<ncl>
  <head>
    <descriptorBase>
      <descriptor id="desc">
        <descriptorParam name="top" value="25%"/>
        <descriptorParam name="left" value="25%"/>
        <descriptorParam name="width" value="50%"/>
        <descriptorParam name="height" value="50%"/>
      </descriptor>
    </descriptorBase>
  </head>
  <body>
    <port id="start" component="txt"/>
    <media id="txt" src="samples/text.txt" descriptor="desc">
      <property name="fontColor" value="yellow"/>
    </media>
  </body>
</ncl>
