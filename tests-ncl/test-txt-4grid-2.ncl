<ncl>
  <head>
    <regionBase>
      <region id='reg' width='50%' height='50%'/>
      <region id='reg2' left="50%" width='50%' height='50%'/>
      <region id='reg3' top="50%" width='50%' height='50%'/>
      <region id='reg4' top="50%" left="50%" width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
       <descriptor id='desc' region='reg'/>
       <descriptor id='desc2' region='reg2'/>
       <descriptor id='desc3' region='reg3'/>
       <descriptor id='desc4' region='reg4'/>
    </descriptorBase>
  </head>
  <body>
    <port id='start' component='text'/>
    <port id='start2' component="text2"/>
    <port id='start3' component="text3"/>
    <port id='start4' component="text4"/>

    <media id='text' src='samples/text.txt' descriptor='desc'>
       <property name="fontColor" value="#0" />
       <property name="backgroundColor" value="rgb(255,255,0)" />
       <property name="fontFamily" value="Courier" />
       <property name="textAlign" value="center" />
       <property name="verticalAlign" value="middle" />
       <property name="fontStyle" value="normal" />
       <property name="fontSize" value="18px" />
       <property name="fontWeight" value="normal" />
       <property name="fontVariant" value="small-caps" />
    </media>

    <media id='text2' src='samples/text.txt' descriptor='desc2'>
       <property name="fontColor" value="rgb(255,0,0)" />
       <property name="backgroundColor" value="#0F0" />
       <property name="fontFamily" value="Arial" />
       <property name="textAlign" value="left" />
       <property name="verticalAlign" value="bottom" />
       <property name="fontStyle" value="italic" />
       <property name="fontSize" value="12pt" />
       <property name="fontWeight" value="bold" />
       <property name="fontVariant" value="small-caps" />
    </media>

     <media id='text3' src='samples/text.txt' descriptor='desc3'>
       <property name="fontColor" value="#E6F732" />
       <property name="backgroundColor" value="#000055" />
       <property name="fontFamily" value="Times" />
       <property name="textAlign" value="justify" />
       <property name="verticalAlign" value="top" />
       <property name="fontStyle" value="italic" />
       <property name="fontSize" value="22px" />
       <property name="fontWeight" value="bold" />
       <property name="fontVariant" value="small-caps" />
    </media>

    <media id='text4' src='samples/text.txt' descriptor='desc4'>
       <property name="fontColor" value="#000000FF" />
       <property name="backgroundColor" value="#e5e5e555" />
       <property name="fontFamily" value="Georgia" />
       <property name="textAlign" value="right" />
       <property name="verticalAlign" value="middle" />
       <property name="fontStyle" value="italic" />
       <property name="fontSize" value="16" />
       <property name="fontWeight" value="normal" />
       <property name="fontVariant" value="small-caps" />
    </media>

  </body>
</ncl>
