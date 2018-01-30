<ncl>
  <body>
    <port id="start" component="r"/>
    <media id="m" src="samples/gnu.png">
      <property name="background" value="red"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <media id="r" refer="m">
      <property name="background" value="blue"/>
    </media>
  </body>
</ncl>
