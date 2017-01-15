#!/bin/bash
#This script replaces in configur.ac file the use of libav by ffmpeg.
#This is util for systems using Ubuntu 14.04 or above.
#The ffmpeg can be found in http://www.telemidia.puc-rio.br/~rafaeldiniz/ginga4linux/.
sed -i -e s/avcodec/avcodecffmpeg/g -e s/avdevice/avdeviceffmpeg/g -e s/avfilter/avfilterffmpeg/g -e s/avformat/avformatffmpeg/g -e s/avutil/avutilffmpeg/g -e s/postproc/postprocffmpeg/g -e s/swresample/swresampleffmpeg/g -e s/swscale/swscaleffmpeg/g configure.ac 
