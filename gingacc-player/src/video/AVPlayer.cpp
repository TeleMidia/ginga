/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "player/AVPlayer.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <sys/types.h>
#include <fcntl.h>

/********/
/*ST7100*/
/********/
#ifdef STx7100

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	AVPlayer::AVPlayer(string mrl, bool hasVisual) : ESPlayer(mrl) {

	}

	AVPlayer::AVPlayer(video_t *video) : ESPlayer("") {
		provider = NULL;
		this->mrl = NULL;
		stopTime = -1;
		mainAV = true;

		frame = NULL;
		status = STOP;
		running = false;
		this->hasVisual = true;
		this->video = video;
		this->audio = NULL;
		video_open(video);
	}

	AVPlayer::AVPlayer(audio_t *audio) : ESPlayer("") {
		provider = NULL;
		this->mrl = NULL;
		stopTime = -1;
		mainAV = true;

		frame = NULL;
		status = STOP;
		running = false;
		this->hasVisual = false;
		this->video = NULL;
		this->audio = audio;
		audio_open(audio);
	}

	AVPlayer::~AVPlayer() {
		lock();
		wclog << "AVPlayer::~AVPlayer()" << endl;

		if (hasVisual) {
			video_close(video);

		} else {
			audio_close(audio);
		}

		wclog << "AVPlayer::~AVPlayer() done" << endl;
	}

	Surface* AVPlayer::loadFile() {

	}

	void AVPlayer::setSoundLevel(float level) {
		if (!hasVisual) {
			audio_volume(audio, (int)(level*100));
		}
	}

	void AVPlayer::createFrame() {

	}

	Surface* AVPlayer::getFrame() {
		return NULL;
	}

	void AVPlayer::getOriginalResolution(int* height, int* width) {
		*width = 0;
		*height = 0;
	}

	double AVPlayer::getMediaTotalTime() {
		return 0;
	}

	double AVPlayer::getMediaTime() {
		if (!running) {
			return elapsedTime;
		}

		return (elapsedTime + getCurrentTimeMillis() -
					    initTime - elapsedPause);
	}

	void AVPlayer::setMediaTime(double pos) {

	}

	void AVPlayer::setStopTime(double pos) {

	}

	double AVPlayer::getStopTime() {
	     return -1;
	}

	void AVPlayer::play() {
		elapsedTime = 0;
		elapsedPause = 0;
		initTime = getCurrentTimeMillis();
		running = true;

		if (hasVisual) {
			video_play(video);

		} else {
			audio_play(audio);
		}
	}

	void AVPlayer::pause() {
		pauseTime = getCurrentTimeMillis();
		elapsedTime = elapsedTime + (pauseTime - initTime);
		running = false;
		if (hasVisual) {
			video_pause(video);

		} else {
			//audio_pause(audio);
		}
	}

	void AVPlayer::stop() {
		running = false;
		elapsedTime = 0;
		elapsedPause = 0;
		if (hasVisual) {
			video_stop(video);

		} else {
			audio_stop(audio);
		}
	}

	void AVPlayer::resume() {
		initTime = getCurrentTimeMillis();
		elapsedPause = elapsedPause + (initTime - pauseTime);
		running = true;
		if (hasVisual) {
			video_resume(video);

		} else {
			//audio_resume(audio);
		}
	}

	void AVPlayer::release() {

	}

	void AVPlayer::setMrl(string mrl) {

	}

	string AVPlayer::getMrl() {

	}

	bool AVPlayer::isPlaying() {

	}

	void AVPlayer::addListener(PlayerListener* listener) {
		Player::addListener(listener);
	}

	void AVPlayer::setVoutWindow(int x, int y, int w, int h) {
		if (hasVisual) {
			video_resize_window(video, x, y, w, h);
		}
	}

	void AVPlayer::setAlphaBlend(int x, int y, int w, int h) {

	}

	void AVPlayer::checkResize() {

	}

	void AVPlayer::run() {

	}
}
}
}
}
}
}

/*******/
/*GEODE*/
/*******/
#elif GEODE

int audio, width, height, x, y;
DFBRectangle rect;
int fd;
double initialTimeStamp;
int streamtype;
int bytesread;
int rc;
int buf;
int playfile_state;
int b_stopit;
int b_getoutofit;
struct v4l2_capability	v4l2cap;
int fd_video;
int fd_vout;
static int DVIP_error;
geode_general_param_t par;
int CanTV;
int HasDecoder;
int CanDigital;

int peek;
struct termios orig, newOne;

/* enable dvip mode */
int dvip(int enable)
{
      if (ioctl(fd_vout,GEODE_IOCTL_S_DVIP,&enable)==-1) {
         if(DVIP_error==0 && enable==1) {
           printf("Cannot enable DVIP mode, please check v4l2 load options.\n");
           DVIP_error=1;
         }
         return (-1);
      }
      return 0;
}

/* set control value -- not used */
int video_set_control(unsigned long id,unsigned char value)
{
	struct v4l2_control control;
	control.id = id;
	control.value = value;
	if(ioctl(fd_video, VIDIOC_S_CTRL, &control) == -1) {
	  printf ("error in setting video control\n");
        return (-1);
      }
      return 0;
}

/* set brightness */
int video_brightness(unsigned char bright)
{
     if (video_set_control(V4L2_CID_BRIGHTNESS, bright) == -1){
        printf("error in setting brightness\n");
        return (-1);
      }
      return 0;
}

/* set contrast */
int video_contrast(unsigned char contrast)
{
	if (video_set_control(V4L2_CID_CONTRAST, contrast)==-1){
          printf("error in setting contrast\n");
          return (-1);
	}
	return 0;
}

/* set saturation */
int video_saturation(unsigned char sat)
{
	if (video_set_control(V4L2_CID_SATURATION, sat) == -1){
	    printf("error in setting saturation\n");
            return (-1);
	}
	return 0;
}

/* set hue */
int video_hue(unsigned char hue)
{
	if (video_set_control(V4L2_CID_HUE, hue)==-1){
	   printf("error in setting hue\n");
         return (-1);
	}
	return 0;
}

/* set sharpness */
int video_sharpness(unsigned char sharp)
{
   if(video_set_control(V4L2_CID_WHITENESS, sharp) == -1){
	printf("error in setting sharpness\n");
	return (-1);
   }
   return 0;
}

/* open video, vout, and realmagic drivers */
int open_devices()
{
      int retval = 0;
      int fd_vbi = -1;
      int fd_realmagic = -1;

      fd_video = open("/dev/video", O_RDWR);
      if(fd_video == -1){
        printf("Error in open /dev/video\n");
        printf("Please check geode_v driver and /dev/video node\n");
        retval = -1;
      }
      fd_vout = open("/dev/vout", O_RDWR);
      if(fd_vout == -1) {
        printf("Error in open /dev/vout\n");
        printf("Please check geode_v driver and /dev/vout node\n");
        retval = -1;
      }
      fd_vbi = open("/dev/vbi", O_RDWR);
      if(fd_vbi == -1) {
        printf("Error in open /dev/vbi\n");
        printf("Please check geode_v driver and /dev/vbi node\n");
        retval = -1;
      }
      close(fd_vbi);
      fd_realmagic = open("/dev/realmagic0", O_RDWR);
      if(fd_realmagic == -1) {
         fd_realmagic = open("/dev/realmagic", O_RDWR);
         if (fd_realmagic == -1){
            printf("Cannot open /dev/realmagic0\n");
             CanDigital = 0;
         }
      }
      close(fd_realmagic);
      return retval;
}

/* close video and vout drivers */
void close_devices()
{
	close(fd_video);
	close(fd_vout);
}

/* get vout format */
int get_vout_format(int * width, int* height)
{
	struct v4l2_format fmt;
	if (ioctl(fd_vout,VIDIOC_G_FMT,&fmt)== -1){
         printf("error getting vout format\n");
         return (-1);
      }
	*width = fmt.fmt.pix.width;
	*height = fmt.fmt.pix.height;
      return 0;
}

/* set color key for the overlay */
int set_color_key(unsigned long key, unsigned long mask)
{
	par.p_1 = key;
	par.p_2 = mask;
	if(ioctl(fd_vout, GEODE_IOCTL_S_COLOR_KEY,&par)== -1){
		printf ("error in setting color key\n");
            return (-1);
      }
      return 0;
}

/* set graphic on video */
int set_graphic_on_video(int enable, int color_key_src)
{
	par.p_1 = enable;
	par.p_2 = color_key_src;
	if(ioctl(fd_vout, GEODE_IOCTL_S_GRAPHICS_ON_VIDEO, &par)==-1) {
		printf ("error in setting graphics on video\n");
            return (-1);
      }
	return 0;
}

/* set alpha windows coordinates */
int set_alpha_window(unsigned char num, unsigned short x, unsigned short y,
			unsigned short width, unsigned short height)
{
	par.p_1 = num;
	par.p_2 = x;
	par.p_3 = y;
	par.p_4 = width;
	par.p_5 = height;
	if (ioctl(fd_vout, GEODE_IOCTL_S_ALPHA_WINDOW, &par)== -1) {
	   printf ("error in settign alpha window\n");
           return (-1);
        }
	return 0;
}

/* deploy alpha window */
int set_alpha_operation(unsigned char num, unsigned char alpha,
			unsigned long color, int color_enable,
			int priority, char fade, int enable)
{
	par.p_1 = num;
	par.p_2 = alpha;
	par.p_3 = color;
	par.p_4 = color_enable;
	par.p_5 = priority;
	par.p_6 = fade;
	par.p_7 = enable;
	if(ioctl(fd_vout, GEODE_IOCTL_S_ALPHA_OPER, &par)== -1){
		printf ("error in setting alpha operation\n");
		return (-1);
 	}
	return 0;
}

/* enable video outside of alpha windows */
int set_video_outside_alpha(int enable)
{
	par.p_1 = (unsigned long)enable;
	if(ioctl(fd_vout, GEODE_IOCTL_VIDEO_OUTSIDE_ALPHA, &par)== -1) {
	   printf ("error in settign video ouside alpha window\n");
         return (-1);
	}
	return 0;
}

/* set overlay */
int set_overlay_info( unsigned long key, unsigned long mask, int graphics,
			int enable)
{
	overlay_info_t ovr_info;
	ovr_info.ColorKey = key;
	ovr_info.ColorMask = mask;
	ovr_info.Graphics = graphics;
	ovr_info.Enable = enable;
	if(ioctl(fd_vout, GEODE_IOCTL_SET_OVERLAY_INFO, &ovr_info)== -1) {
         printf("error in setting overlay\n");
         return (-1);
	}
	return 0;
}

/* set alpha blend */
int set_alpha_blend_info(int window, int x, int y, int w, int h,
			  int priority, int alpha, int delta, int enable,
			  unsigned long color)
{
	alpha_blend_info_t alp_info;
	alp_info.WindowNumber = window;
	alp_info.StartX = x;
	alp_info.StartY = y;
	alp_info.Width = w;
	alp_info.Height = h;
	alp_info.Alpha = alpha;
	alp_info.Delta = delta;
	alp_info.Enable = enable;
	alp_info.Color = color;
	if (ioctl(fd_vout, GEODE_IOCTL_SET_ALPHA_BLEND_INFO, &alp_info)==-1){
	   printf("error in setting alpha blend\n");
         return (-1);
	}
	return 0;
}

/* set video format */
int set_video_fmt(int width, int height)
{
	struct v4l2_format fmt;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	if(ioctl(fd_video, VIDIOC_S_FMT, &fmt)==-1){
	   printf("error in setting video format\n");
         return (-1);
	}
	return 0;
}

/* set vout window coordinates */
int set_vout_window(int x, int y, int width, int height)
{
	struct v4l2_window win;
	win.x = x;
	win.y = y;
	win.width = width;
	win.height = height;
	win.clipcount = 0;
      if(ioctl(fd_vout, VIDIOC_S_WIN, &win)==-1){
	   printf("error in setting vout window\n");
         return (-1);
	}
	return 0;
}

/* select video input */
int set_input(int inp)
{
	int input = inp;
	if( ioctl(fd_video, VIDIOC_S_INPUT,&input)==-1){
	   printf("error in setting video input\n");
         return (-1);
	}
	return 0;
}

/* enable video capturing */
int capture_enable( int enable)
{
	if(enable) {
	   if (ioctl(fd_video, VIDIOC_STREAMON)== -1) {
	      printf("error in enable video capturing\n");
              return (-1);
	   }
	} else {
	    if (ioctl(fd_video, VIDIOC_STREAMOFF) == -1) {
		printf("error in disable video capturing\n");
                return (-1);
	    }
	}
        return 0;
}

/* enable preview */
int preview(int enable)
{
      int i = enable;
      if (ioctl(fd_vout, VIDIOC_PREVIEW,&i) == -1) {
         if(enable)
	     printf("error in enable preveiw\n");
         else
	     printf("error in disable preveiw\n");
           return (-1);
	}
	return 0;
}

/* select tuner channel */
int set_chan(int ch)
{
	unsigned long c = ch;
	if (ioctl(fd_video, VIDIOC_S_FREQ,&c)==-1){
	      printf("error in selecting tuner channel\n");
            return (-1);
	 }
	 return 0;
}

int set_vout_control(int val)
{
	int i = 0;
	struct v4l2_control ctrl;
	if (ioctl(fd_vout,VIDIOC_S_EFFECT,&i)== -1) {
	    printf("error in setting S_EFFECT\n");
          return (-1);
	}
	ctrl.id = GEODE_CID_OVERLAY_MODE;
	ctrl.value = val;
	if (ioctl(fd_vout, VIDIOC_S_CTRL, &ctrl)==-1) {
	    printf("error in setting S_CTRL\n");
            return (-1);
	}
	return 0;
}

int DriverCapability()
{
       int retval;

       retval = ioctl(fd_video, VIDIOC_QUERYCAP, &v4l2cap);

       if (retval == -1) {
	  fprintf(stderr, "Query driver capability returned error\n");
	  return -1;
       }
       if (!(v4l2cap.flags&V4L2_FLAG_TUNER))
          CanTV = 0;
       if (v4l2cap.inputs==1)
          HasDecoder = 0;
       return 0;
}
/* end driver wrapper */

/*
void open_port()
{
    if (tcgetattr(0,&orig)==-1)
    {
        printf("Could not tcgetattr");
        exit(1);
    }

    newOne = orig;

    newOne.c_lflag     &= ~ICANON;
    newOne.c_lflag     &= ~ECHO;
    newOne.c_lflag     &= ~ISIG;
    newOne.c_cc[VMIN]  =  1;
    newOne.c_cc[VTIME] =  0;

    if (tcsetattr(0,TCSANOW,&newOne)==-1)
    {
        printf("Could not tcsetattr");
        exit(1);
    }
}

void close_port()
{
    tcsetattr(0, TCSANOW, &orig);
}
int kbhit()
{
    char ch;
    int  nread;

    if (peek != -1) return 1;

    newOne.c_cc[VMIN] = 0;
    tcsetattr(0, TCSANOW, &newOne);
    nread = read(0, &ch, 1);
    newOne.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &newOne);

    if(nread == 1)
    {
        peek = ch;
        return 1;
    }

    return 0;
}

int readch()
{
    char ch;

    if(peek != -1)
    {
        ch   = peek;
        peek = -1;
        return ch;
    }

    read(0, &ch, 1);
    return ch;
}
*/
DWORD setOutputMode(DWORD newvgatv)
{
  DWORD vgatv;
  vgatv = FMPGet(FMPI_VIDEOOUT);
  vgatv = vgatv  & (~FMPV_VIDEOOUT_MODE_MASK);
  vgatv = vgatv | newvgatv;
  return FMPSet(FMPI_VIDEOOUT, vgatv);
}

int SetAlphaBlend(
	    int wnum, int x, int y, int w, int h,
	    int subcolor, int alphaval, int xoff, int yoff) {

	int retval=0;
	retval = set_alpha_blend_info(wnum, x, y, w, h,
     	    0, alphaval & 0xFF, (alphaval >> 8) & 0xFF,
     	    (alphaval < 0) ? 0 : 1, subcolor);

	return retval;
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	AVPlayer::AVPlayer(string mrl) : ESPlayer(mrl) {
		provider = NULL;
		this->mrl = mrl;
		stopTime = -1;
		peek = -1;
		fd_video = -1;
		fd_vout = -1;
		fd = -1;
		initialTimeStamp = -1;
		DVIP_error = 0;
		CanTV = 1;
		HasDecoder = 1;audio
		CanDigital = 1;
		bytesread=1;
		rc = FMPE_OK;
		b_stopit = 0;
		b_getoutofit = 0;
		streamtype=FMPF_TRANSPORT;

		try {
			//tentando abrir dispositivos
			if (open_devices() == -1) {
				wclog << "Erro: v4l2 devices nao foram iniciados." << endl;

			} else {
				//verifica a existencia de decoder, capturadora de TV ...
				int DriverCapability();
				if(HasDecoder && !CanDigital) {
					wclog << "Erro no CanDigital." << endl;
					close_devices();

				} else if (!HasDecoder) {
					if(!CanDigital) {
						if(set_input(0) == -1) {
							wclog << "Erro: decoder nao encontrada." << endl;
							close_devices();
						}
					}

				} else if (get_vout_format(&width,&height) ==-1) {
					wclog << "Erro: nao tem capacidade para pegar resolucao"
					wclog << " da tela." << endl;
					close_devices();

				} else if (set_video_fmt(width, height) == -1) {
					wclog << "Erro: nao tem capacidade para configurar"
					wclog << " a resolucao do video" << endl;
					close_devices();

				} else if (set_color_key(OVERLAY_KEY, OVERLAY_MASK) == -1) {
					wclog << "Erro: nao tem capacidade para fazer overlay.";
					wclog << endl;
					close_devices();

				} else if (set_vout_control(1) == -1) {
					wclog << "Erro: falha na inicializacao do overlay" << endl;
					close_devices();
				}

				set_input(0);
				capture_enable(1);

				x = 0;
				y = 0;
				/*SetAlphaBlend(alpha0, x, y, width, height, 0, -1, 0, 0);
				SetAlphaBlend(alpha1, 150, 150, 80, 80, 0, -1, 0, 0);*/

				//para fazer com que o driver de audio seja carregado
				audio = open( "/dev/audio", O_RDONLY );
				if (audio == -1) {
					wclog << "Warning! Initializing audio device." << endl;
				}
				close(audio);

				set_alpha_blend_info(
					    0, 0, 0, width, height, 1,100,0,1,0xFFFF0000);

				FMPSetDestination(0, 0, width, height);
				set_vout_window(0, 0, width, height);
				set_overlay_info(OVERLAY_KEY, OVERLAY_MASK, 1, 1);

				MPEGDriverEntry(NO_DRIVE);
				FMPOpen(streamtype, 32*1024, 10, NULL, 0);
				videoData = new VideoFrame();

			}
		} catch (...) {
			wclog << "Warning! AVPlayer::AVPlayer" << endl;
		}
	} //constructor

	void AVPlayer::loadFile() {
		if (fd == -1) {
			fd=open(mrl, O_RDONLY|O_NONBLOCK|O_LARGEFILE);
		}
	}

	void AVPlayer::setSoundLevel(float level) {
		if (provider != NULL) {
			provider->SetVolume(provider, level);
		}
	}

	void AVPlayer::createFrame() {

	}

	void AVPlayer::getOriginalResolution(int* height, int* width) {

	}

	double AVPlayer::getMediaTotalTime() {

	}

	double AVPlayer::getMediaTime() {
		double mediaTime;
		try {
			if (initialTimeStamp < 0) {
				mediaTime = 0;

			} else {
				mediaTime = (double)((
					    FMPGet(FMPI_PTS) - initialTimeStamp) / 100000);

				//mediaTime = (double)(FMPGet(FMPI_STC) / 100000);
			}

		} catch(...) {
			mediaTime = 0;
		}
		return mediaTime;
	}

	void AVPlayer::setMediaTime(double pos) {

	}

	void AVPlayer::setStopTime(double pos) {
		stopTime = pos;
	}

	double AVPlayer::getStopTime() {
		return stopTime;
	}

	void AVPlayer::play() {
		Thread::Thread::start();
	}

	void AVPlayer::pause() {

	}

	void AVPlayer::stop() {

	}

	void AVPlayer::resume() {

	}

	void AVPlayer::release() {

	}

	void AVPlayer::setOutputDisplay(Window* w) {
		if (w != NULL && videoData != NULL) {
			videoData->setDestination(w);
		}
	}

	void AVPlayer::setMrl(const char* mrl) {
		this->mrl = mrl;
	}

	bool AVPlayer::isPlaying() {
		if (getMediaTime() < 0 ||
			    (getMediaTime() >= getStopTime() && getStopTime() > 0))

			return false;
		else
			return true;
	}

	void AVPlayer::frameCB(void *cdata) {

	}

	void AVPlayer::addListener(PlayerListener* listener) {
		Player::addListener(listener);
	}

	void AVPlayer::setVoutWindow(int posX, int posY, int wdt, int hgt) {
		try {
			Window* w;
			IDirectFBSurface* s;

			w = videoData->getDestination();
			s = w->getWidgetSurface();
			s->Clear(s, 0xF0, 0x00, 0xF0, 0xFF);
			s->Flip(s, NULL, (DFBSurfaceFlipFlags)(0));

			//habilitando o resize
			FMPSet(FMPI_OVERLAY_FLAGS, 0x0002);
			FMPSetDestination(posX, posY, wdt, hgt);
			/*set_video_fmt(wdt, hgt);
			set_vout_window(posX, posY, wdt, hgt);*/
			/*FMPSetVideoPortDimensions(wdt, hgt);
			FMPSetSource(posX, posY, wdt, hgt);
			FMPSetVisibleSource(posX, posY, wdt, hgt);*/
			/*FMPEnableOverlay(1);*/

		} catch (...) {
			wclog << "Warning! AVPlayer::setVoutWindow" << endl;
		}
	}

	void AVPlayer::setAlphaBlend(int x, int y, int w, int h) {
		try {
			SetAlphaBlend(alpha0, x, y, width, height, 0, -1, 0, 0);
			//SetAlphaBlend(alpha1, x, y, w, h, 0, 0x00FFFF, 0, 0);

		} catch(...) {
			wclog << "Warning! AVPlayer::setAlphaBlend" << endl;
		}
	}

	void AVPlayer::run() {
		bool firstLoop;
		firstLoop = true;

		try {
			//wclog << "AVPlayer:: Thread running" << endl;
			//wclog << "AVPlayer:: API Play" << endl;
			FMPPlay();

			//wclog << "AVPlayer:: turn on preview" << endl;
			preview(1);
			//wclog << "AVPlayer:: setting alpha blend";
			SetAlphaBlend(alpha0, x, y, width, height, 0, -1, 0, 0);

			/*FMPSetVideoPortDimensions(width,height);
			FMPSetSource(0,0,width,height);
			FMPSetVisibleSource(0,0,width,height);
			FMPSetDestination(0,0,width,height);
			FMPEnableOverlay(1);*/
			//FMPEnableFullScreen(1);

			playfile_state = PLAYING;

			while (bytesread > 0) {
				//wclog << "AVPlayer:: reading from file" << endl;
				FMP_BUFFER FMPBuf;

				// get a pointer to a free buffer
				rc = FMPGetBuffer (&FMPBuf, TRUE);
				if (rc != FMPE_OK) {
					wclog << "FMPError : " << FMPGetLastError(rc) << endl;
					break;
				}

				// signals can cause harmless EINTR
				while (((bytesread=read(
					    fd, FMPBuf.pBuffer, FMPBuf.dwBufferSize))==-1) &&
					    (errno==EINTR));

				if (bytesread == -1) {
					bytesread = 0;
					FMPBuf.dwDataSize=bytesread;
					break;
				}

				FMPBuf.dwDataSize=bytesread;

				if (FMPGet(FMPI_PTS) < initialTimeStamp) {
					initialTimeStamp = FMPGet(FMPI_PTS);
				}
				//wclog << "PTS = " << FMPGet (FMPI_PTS) << " STC = ";
				//wclog << FMPGet (FMPI_STC) << endl;

				// the buffer is ready to be queued
				if (FMPPush (&FMPBuf) != FMPE_OK) {
					wclog << "Warning! pushing buffer" << endl;
				}
				if (firstLoop && FMPGet(FMPI_PTS) != 0) {
					firstLoop = false;
					initialTimeStamp = FMPGet(FMPI_PTS);
					//wclog << "initialTimeStamp = " << initialTimeStamp << endl;
				}
			}
			::usleep(2000000);

			FMPStop();
			FMPClose();

			close(fd);
			close_devices();

			//setOutputMode(FMPV_VIDEOOUT_VGA);
			MPEGDriverUnload();

			notifyListeners(PlayerListener::PL_NOTIFY_STOP);

		} catch (...) {
			wclog << "Warning! AVPlayer::run" << endl;
		}
	}
}
}
}
}
}
}

/**************/
/*     PC     */
/**************/
#else

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	static const unsigned int buffSize = 188 * 1024;
	static const char dataBuffer[buffSize] = "";

	AVPlayer::AVPlayer(string mrl, bool hasVisual) : Thread(), Player(mrl) {
		string::size_type pos;

		this->provider    = NULL;
		this->mainAV      = false;
		this->buffered    = false;

		this->status      = STOP;
		this->running     = false;
		this->hasVisual   = hasVisual;
		this->soundLevel  = 1.0;
		this->win         = NULL;

		pthread_mutex_init(&pMutex, NULL);

#if HAVE_CCRTPIC
		this->icListener  = NULL;
#endif

		if (mrl.length() > 6 && mrl.substr(0, 6) == "rtp://") {
#if HAVE_CCRTPIC
			icListener = new RTPListener(mrl);
			this->mrl  = icListener->getUrl();

			Thread::start();

			pthread_t _tId;
			pthread_create(&_tId, NULL, createProvider, this);
			pthread_detach(_tId);
#endif

		} else if (mrl.length() > 11 && mrl.substr(0, 11) == "sbtvd-ts://") {
			this->mainAV = true;
			pos = mrl.find("#");
			if (pos != std::string::npos) {
				this->mrl = mrl.substr(pos + 1, mrl.length() - (pos + 1));

			} else {
				this->mrl = mrl.substr(11, mrl.length() - 11);
			}
			cout << "AVPlayer::AVPlayer MAINAV CREATED MRL = '";
			cout << this->mrl << "'" << endl;

		} else {
			createProvider(this);
			this->scopeEndTime = getTotalMediaTime();
		}
	}

	AVPlayer::~AVPlayer() {
		if (status != STOP) {
			stop();
		}

		pthread_mutex_lock(&pMutex);
		pthread_mutex_unlock(&pMutex);
		pthread_mutex_destroy(&pMutex);
		unlockConditionSatisfied();
		lock();
		if (surface != NULL && mainAV) {
			surface->setParent(NULL);
		}

		if (mainAV) {
			if (win != NULL) {
				delete win;
				win = NULL;
			}
		}

		if (this->provider != NULL) {
			release();
		}
		unlock();
	}

	ISurface* AVPlayer::getSurface() {
		if (provider == NULL) {
			createProvider(this);
			if (provider == NULL) {
				cout << "AVPlayer::getSurface() can't create provider" << endl;
			}
		}

		return Player::getSurface();
	}

	void* AVPlayer::createProvider(void* ptr) {
		AVPlayer* p = (AVPlayer*)ptr;

		cout << "AVPlayer::createProvider '" << p->mrl << "'" << endl;
		pthread_mutex_lock(&(p->pMutex));

		if (p->provider == NULL && fileExists(p->mrl)) {
#if HAVE_COMPSUPPORT
			if (p->hasVisual) {
				p->provider = ((CMPCreator*)(cm->getObject(
						"VideoProvider")))(p->mrl.c_str());

			} else {
				p->provider = ((CMPCreator*)(cm->getObject(
						"AudioProvider")))(p->mrl.c_str());
			}
#else
			if (p->hasVisual) {
				p->provider = new DFBVideoProvider(p->mrl.c_str());

			} else {
				p->provider = new FusionSoundAudioProvider(p->mrl.c_str());
			}
#endif

			cout << "AVPlayer::createProvider call createFrame" << endl;
			p->surface = p->createFrame();
		}

		pthread_mutex_unlock(&(p->pMutex));
		cout << "AVPlayer::createProvider '" << p->mrl << "' all done" << endl;
	}

	void AVPlayer::finished() {
		cout << "AVPlayer::finished" << endl;
	}

	double AVPlayer::getEndTime() {
		return scopeEndTime;
	}

	void AVPlayer::initializeAudio(int numArgs, char* args[]) {

	}

	void AVPlayer::releaseAudio() {

	}

	bool AVPlayer::getHasVisual() {
		return this->hasVisual;
	}

	void AVPlayer::setSoundLevel(float level) {
		//wclog << "AVPlayer::setSoundLevel()" << endl;
		//lock()();
		this->soundLevel = level;
		if (provider != NULL) {
			provider->setSoundLevel(soundLevel);
		}
		//unlock();
	}

	ISurface* AVPlayer::createFrame() {
		//wclog << "AVPlayer::createFrame()" << endl;
		lock();
		if (surface != NULL) {
			cout << "AVPlayer::createFrame Warning! surface != NULL";
			cout << endl;
			if (mainAV) {
				surface->setParent(NULL);
			}
			delete surface;
		}

		surface = provider->getPerfectSurface();
		if (win != NULL && mainAV) {
			surface->setParent(win);
		}
		unlock();
		return surface;
	}

	void AVPlayer::getOriginalResolution(int* height, int* width) {
		if (provider != NULL) {
			provider->getOriginalResolution(height, width);
		}
	}

	double AVPlayer::getTotalMediaTime() {
		if (provider != NULL) {
			return provider->getTotalMediaTime();
		}

		this->wakeUp();
		return 0;
	}

	int64_t AVPlayer::getVPts() {
		if (provider == NULL) {
			return 0;
		}

		return provider->getVPts();
	}

	void AVPlayer::timeShift(string direction) {
		if (provider != NULL) {
			if (direction == "forward") {
				provider->setMediaTime(provider->getMediaTime() + 10);

			} else if (direction == "backward") {
				provider->setMediaTime(provider->getMediaTime() - 10);
			}
		}
	}

	double AVPlayer::getCurrentMediaTime() {
		if (provider == NULL) {
			this->wakeUp();
			return -1;
		}

		return provider->getMediaTime();
	}

	double AVPlayer::getMediaTime() {
		return getCurrentMediaTime();
	}

	void AVPlayer::setMediaTime(double pos) {
		if (status == PLAY) {
			status = PAUSE;
			this->wakeUp();
			provider->setMediaTime(pos);
			status = PLAY;
			running = true;
			Thread::start();

		} else if (provider != NULL) {
			provider->setMediaTime(pos);
		}
	}

	void AVPlayer::setStopTime(double pos) {
		if (status == PLAY) {
			status = PAUSE;
			this->wakeUp();
			scopeEndTime = pos;
			status = PLAY;
			running = true;
			Thread::start();

		} else {
			scopeEndTime = pos;
		}
	}

	double AVPlayer::getStopTime() {
	     return scopeEndTime;
	}

	void AVPlayer::setScope(
			string scope, short type, double begin, double end) {

		Player::setScope(scope, type, begin, end);
		if (type == TYPE_PRESENTATION) {
			if (scopeInitTime > 0) {
				setMediaTime(scopeInitTime);
			}

			if (scopeEndTime > 0) {
				setStopTime(scopeEndTime);
			}
		}
	}

	void AVPlayer::play() {
		if (provider == NULL) {
			cout << "AVPlayer::play() can't play, provider is NULL" << endl;
			this->wakeUp();
			return;
		}

		Player::play();
		provider->playOver(surface, hasVisual, this);

		if (!running) {
			running = true;
			Thread::start();
		}
	}

	void AVPlayer::pause() {
		status = PAUSE;
		if (provider == NULL) {
			this->wakeUp();
			return;
		}

		if (outputWindow != NULL) {
			outputWindow->validate();
		}
		provider->pause();
		this->wakeUp();
		/*if (hasVisual) {
			Window::dynamicRenderCallBack((void*)(this->surface));
		}*/
	}

	void AVPlayer::stop() {
		Player::stop();
#if HAVE_CCRTPIC
		if (icListener != NULL) {
			icListener->releaseIC();
		}
#endif
		if (provider == NULL) {
			this->wakeUp();
			return;
		}
		provider->stop();
		this->wakeUp();
	}

	void AVPlayer::resume() {
		setSoundLevel(soundLevel);

		Player::play();
		provider->resume(surface, hasVisual);

		if (!running) {
			running = true;
			Thread::start();
		}
	}

	void AVPlayer::setPropertyValue(string name, string value) {
		float fValue = 1.0;
		vector<string>* vals;

		/*cout << "AVPlayer::setPropertyValue '" << name << "' ";
		cout << "= '" << value << "'" << endl;*/

		//TODO: animation, set volume, brightness, ...
		if (name == "soundLevel") {
			if (value != "") {
				fValue = stof(value);
			}
			setSoundLevel(fValue);

		} else if (mainAV) {
			if (name == "createWindow") {
				vals = split(value, ",");
				if (vals->size() == 4) {
#if HAVE_COMPSUPPORT
					win = ((WindowCreator*)(cm->getObject("Window")))(
							-1,
							stof((*vals)[0]),
							stof((*vals)[1]),
							stof((*vals)[2]),
							stof((*vals)[3]));

#else
					win = new DFBWindow(
							stof((*vals)[0]),
							stof((*vals)[1]),
							stof((*vals)[2]),
							stof((*vals)[3]));
#endif

					win->setCaps(win->getCap("NOSTRUCTURE") |
							win->getCap("DOUBLEBUFFER"));

					win->draw();
				}

				delete vals;

				if (!running) {
					Thread::start();
				}

			} else if (name == "bounds" && win != NULL) {
				vals = split(value, ",");
				if (vals->size() == 4) {
					win->setBounds(
							stof((*vals)[0]),
							stof((*vals)[1]),
							stof((*vals)[2]),
							stof((*vals)[3]));
				}
				delete vals;

			} else if (name == "show" && win != NULL) {
				win->show();

			} else if (name == "hide" && win != NULL) {
				win->hide();
			}
		}

		Player::setPropertyValue(name, value);
	}

	void AVPlayer::addListener(IPlayerListener* listener) {
		Player::addListener(listener);
	}

	void AVPlayer::release() {
		if (provider == NULL) {
			return;
		}

		provider->stop();
		delete provider;
		provider = NULL;

#if HAVE_COMPSUPPORT
		if (hasVisual) {
			cm->releaseComponentFromObject("VideoProvider");
		} else {
			cm->releaseComponentFromObject("AudioProvider");
		}
#endif

#if HAVE_CCRTPIC
		if (icListener != NULL) {
			delete icListener;
			icListener = NULL;
		}
#endif
	}

	void AVPlayer::setMrl(const char* mrl) {
		this->mrl = mrl;
	}

	string AVPlayer::getMrl() {
		return mrl;
	}

	bool AVPlayer::isPlaying() {
		if (provider == NULL) {
			return false;
		}

		if ((getCurrentMediaTime() <= 0 && status != PAUSE) ||
			    (getCurrentMediaTime() >= getStopTime() && getStopTime() > 0)) {

			return false;

		} else {
			return true;
		}
	}

	bool AVPlayer::setOutWindow(int windowId) {
		if (mainAV && win == NULL) {
#if HAVE_COMPSUPPORT
			win = ((WindowCreator*)(cm->getObject("Window")))(
					windowId, -1, -1, -1, -1);

#else
			win = new DFBWindow(windowId);
#endif

			if (!running) {
				Thread::start();
			}
			return true;

		} else {
			Player::setOutWindow(windowId);
		}
		return false;
	}

	void AVPlayer::setAVPid(int aPid, int vPid) {
		while (!buffered) {
			::usleep(150000);
		}
		provider->setAVPid(aPid, vPid);
	}

	void AVPlayer::setAlphaBlend(int x, int y, int w, int h) {
		// only for geode
	}

	void AVPlayer::checkResize() {
		// only for geode
	}

	bool AVPlayer::isRunning() {
		return running;
	}

	bool AVPlayer::checkVideoResizeEvent() {
		bool hasEvent = false;

		if (mainAV) {
			provider->feedBuffers();
		} else {
			::usleep(150000);
		}
		hasEvent = provider->checkVideoResizeEvent(surface);
		setSoundLevel(this->soundLevel);
		if (hasEvent) {
			provider->playOver(surface, hasVisual);
		}

		return hasEvent;
	}

	void AVPlayer::run() {
		double dur;
		double currentTime;
		double timeRemain;

		lock();
#if HAVE_CCRTPIC
		if (icListener != NULL) {
			running = true;
			unlock();
			cout << "AVPlayer::run call performIC" << endl;
			icListener->performIC();
			cout << "AVPlayer::run call performIC done" << endl;
			if (status != STOP && status != PAUSE) {
				status  = STOP;
				running = false;
				notifyListeners(PL_NOTIFY_STOP, "");
			}

			return;
		}
#endif

		if (mainAV) {
			running = true;

			if (hasVisual) {
#if HAVE_COMPSUPPORT
				this->provider = ((CMPCreator*)(cm->getObject(
						"TSVideoProvider")))(mrl.c_str());

#else
				this->provider = new XineVideoProvider(mrl.c_str());
#endif

				unlock();
				this->surface = createFrame();
				lock();

			} else {
#if HAVE_COMPSUPPORT
				this->provider = ((CMPCreator*)(cm->getObject(
						"TSAudioProvider")))(mrl.c_str());

#else
				this->provider = new XineVideoProvider(mrl.c_str());
#endif
			}

			if (this->win != NULL && surface->getParent() == NULL) {
				this->surface->setParent((void*)win);
			}

			provider->playOver(surface, hasVisual);
			checkVideoResizeEvent();
			buffered = true;
			waitForUnlockCondition();

		} else {
			if (!isInfinity(scopeEndTime) &&
					scopeEndTime > 0 && scopeEndTime < getTotalMediaTime()) {

				dur = getStopTime();

			} else {
				dur = getTotalMediaTime();
			}

			::usleep(850000);
			currentTime = getCurrentMediaTime();
			if (currentTime > 0) {
				while (dur > (currentTime + 0.1)) {
					/*cout << "AVPlayer::run dur = '" << dur;
					cout << "' curMediaTime = '";
					cout << currentTime << "' total = '";
					cout << getTotalMediaTime();
					cout << "' for '" << mrl << "'" << endl;*/

					if (status != PLAY) {
						break;
					}

					timeRemain = (dur - currentTime) * 1000;
					if (notifyContentUpdate) {
						if (timeRemain > 250) {
							notifyListeners(
									PL_NOTIFY_UPDATECONTENT,
									"", TYPE_PASSIVEDEVICE);
						}

						if (!this->usleep(65)) { // 15 fps
							break;
						}

					} else if (!this->usleep(timeRemain)) {
						break;
					}

					currentTime = getCurrentMediaTime();
					if (currentTime <= 0) {
						break;
					}
				}
			}
		}

		/*cout << "AVPlayer::run(" << mrl << ") notifying ... " << endl;*/
		if (status != PAUSE) {
			presented = true;
		}

		if (provider != NULL) {
			provider->stop();
		}

		if (status != STOP && status != PAUSE) {
			status = STOP;
			running = false;
			unlock();
			notifyListeners(PL_NOTIFY_STOP, "");

		} else {
			status = STOP;
			running = false;
			unlock();
		}

		/*cout << endl;
		cout << "AVPlayer::run(" << mrl << ") ALL DONE" << endl;
		cout << endl;*/
	}
}
}
}
}
}
}

#endif /* GEODE */

using namespace ::br::pucrio::telemidia::ginga::core::player;

static pthread_mutex_t avpm;
static bool avpmInit = false;

extern "C" IPlayer* createAVPlayer(const char* mrl, bool hasVisual) {
	AVPlayer* player;
	if (!avpmInit) {
		avpmInit = true;
		pthread_mutex_init(&avpm, NULL);
	}

	pthread_mutex_lock(&avpm);
	player = new AVPlayer(mrl, hasVisual);
	pthread_mutex_unlock(&avpm);

	return player;
}

extern "C" void destroyAVPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
