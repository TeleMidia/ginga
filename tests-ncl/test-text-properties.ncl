<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='text'/>
    <media id='text' src='samples/text.txt' descriptor='desc'>
       <property name="fontColor" value="black" />
       <property name="fontFamily" value="serif" />
       <property name="textAlign" value="left" />
       <property name="fontStyle" value="italic" />
       <property name="fontSize" value="32px" />
       <property name="fontWeight" value="bold" />
       <property name="backgroundColor" value="blue" />
       <property name="fontVariant" value="small-caps" />
    </media>
  </body>
</ncl>
