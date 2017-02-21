<ncl>
  <head>
    <regionBase>
      <region id='reg' width='50%' height='350'/>
       <region id='reg2' left="50%" width='50%' height='350'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
       <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='text'/>
    <port  id='start2' component="text2" />

    <media id='text' src='samples/text.txt' descriptor='desc'>
       <property name="fontColor" value="Black" />
       <property name="backgroundColor" value="White" />       
       <property name="fontFamily" value="serif" />
       <property name="textAlign" value="center" />
       <property name="verticalAlign" value="middle" />
       <property name="fontStyle" value="italic" />
       <property name="fontSize" value="22px" />
       <property name="fontWeight" value="bold" />
       <property name="fontVariant" value="small-caps" />
    </media>


    <media id='text2' src='samples/text2.txt' descriptor='desc2'>
       <property name="fontColor" value="Red" />
       <property name="backgroundColor" value="Green" />       
       <property name="fontFamily" value="serif" />
       <property name="textAlign" value="center" />
       <property name="verticalAlign" value="bottom" />
       <property name="fontStyle" value="italic" />
       <property name="fontSize" value="62px" />
       <property name="fontWeight" value="bold" />
       <property name="fontVariant" value="small-caps" />
    </media>
 

  </body>
</ncl>
