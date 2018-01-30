<ncl>
  <body>
    <port id="start" component="m"/>
    <media id="m" src="samples/gnu.png">
      <property name="background" value="red"/>
      <property name="bounds" value="50%,50%,50%,50%"/>
      <!-- <property name="height" value="50%"/> -->
    </media>
    <media id="r" refer="m">
      <property name="background" value="blue"/>
    </media>
  </body>
</ncl>
