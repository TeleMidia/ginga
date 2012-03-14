#ifndef GEODEVIDEO4LINUX_H_
#define GEODEVIDEO4LINUX_H_

#ifdef GEODE

#ifdef __cplusplus
extern "C" {
#endif
#define _LARGEFILE64_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <directfb.h>
#ifdef __cplusplus
}
#endif

#define _DEFINE_FMP_TYPES_
#include "fmp.h"

#define PLAYING 1
#define PAUSED 0

//===============================
/* These are needed to use the V4L2 driver */
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>  /* Video for Linux Two */
#include <linux/ioctl_geode_v.h>


#define TRUE 		1
#define FALSE 		0

/* video standard */

#define VIDEOSTD_NTSC  	0
#define VIDEOSTD_PAL 	1

/* video input selection */

#define VID_TUNER 	0
#define VID_COMPOSITE	1
#define VID_SVIDEO 	2
#define VID_DIGITAL	3

/* alpha window id */
#define alpha0 		0
#define alpha1 		1
#define alpha2 		2

/* overlay attributes */
#define OVERLAY_KEY 	0x00f000f0
#define OVERLAY_MASK 	0x00f0f0f0

/* mpeg operations */
#define MPEG_CLOSE      0
#define MPEG_STOP	1
#define MPEG_START	2
#define MPEG_UPDATE	3
#define MPEG_PLAY	4
#define MPEG_PAUSE	5
#define MPEG_ERROR	6


/* items ID */
#define IDC_GROUP1	11
#define	IDC_GROUP2	12
#define	IDC_GROUP3	13
#define IDC_GROUP4	14
#define IDC_GROUP5	15

#define IDC_TUNER	0x202
#define IDC_VIDEO	0x200
#define IDC_SVIDEO	0x201
#define IDC_DIGITAL	0x203
#define IDC_STREAM	0x204
#define IDC_PREF	0x205
#define IDC_CHNUM10x	0x206
#define IDC_CHNUM1x	0x207
#define IDC_PANLEFT	0x307
#define IDC_PANCENTER	0x308
#define IDC_PANRIGHT	0x309
#define IDC_STATUSWND	0x401
#define IDC_MUTE	0x402
#define IDC_CLOSECAPTION 0x403
#define IDC_VOLUMEUP	0x404
#define IDC_VOLUMEDOWN	0x405
#define IDC_CLOSEPREF	0x407
#define IDC_LOGO	0x408
#define IDC_SCALE	0x409
#define IDC_EXIT	0x40A
#define IDC_FULLSCREEN	0x506
#define IDC_DEBUGDUMP	0x507
#define IDC_CINEMA	0x509
#define IDC_DVIP	0x50A
#define IDC_STATUS	0x600
#define IDC_PREFPANEL	0x601
#define IDC_DISPLAY	0x602
#define IDC_PATTERN	0x701
/* timer */
#define mainTimerID	999

/* Close Caption display */
#define CCDISPLAY_W 	32
#define CCDISPLAY_H 	15
#define CCLEFT		80
#define CCTOP		180
#define CC_POP 		1
#define CC_PAINT	0
#define CC_RU2 		2
#define CC_RU3 		3
#define CC_RU4 		4
#define CCBKCOLOR	BLACK

/* local command and debug */
#define READ_VIP	2
#define WRITE_VIP	3
#define READ_REG	4
#define WRITE_REG	5
#define READ_DEC	6
#define WRITE_DEC	7
#define READ_VID	8
#define WRITE_VID	9


/* gui operations */
#define ITEM_APPLY -2
#define ITEM_TOGGLE -1
#define ITEM_ON 1
#define ITEM_OFF 0

int dvip(int enable);
int video_set_control(unsigned long id,unsigned char value);
int video_brightness(unsigned char bright);
int video_contrast(unsigned char contrast);
int video_saturation(unsigned char sat);
int video_hue(unsigned char hue);
int video_sharpness(unsigned char sharp);
int open_devices();
void close_devices();
int get_vout_format(int * width, int* height);
int set_color_key(unsigned long key, unsigned long mask);
int set_graphic_on_video(int enable, int color_key_src);
int set_alpha_window(unsigned char num, unsigned short x, unsigned short y,
			unsigned short width, unsigned short height);

int set_alpha_operation(unsigned char num, unsigned char alpha,
			unsigned long color, int color_enable,
			int priority, char fade, int enable);

int set_video_outside_alpha(int enable);
int set_overlay_info( unsigned long key, unsigned long mask, int graphics,
			int enable);

int set_alpha_blend_info(int window, int x, int y, int w, int h,
			  int priority, int alpha, int delta, int enable,
			  unsigned long color);

int set_video_fmt(int width, int height);
int set_vout_window(int x, int y, int width, int height);
int set_input(int inp);
int capture_enable( int enable);
int preview(int enable);
int set_chan(int ch);
int set_vout_control(int val);
int DriverCapability();
void open_port();
void close_port();
int kbhit();
int readch();
DWORD setOutputMode(DWORD newvgatv);
int SetAlphaBlend(int wnum, int x, int y, int w, int h, int subcolor, int alphaval, int xoff, int yoff);

/* driver wrapper */
#ifdef __cplusplus
}
#endif

#endif /*GEODE*/
#endif /*GEODEVIDEO4LINUX_H_*/
