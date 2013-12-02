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
*******************************************************************************
Part of this file is extracted from ffplay
ffplay is part of FFmpeg library.
FFmpeg is free software licensed under the LGPL or GPL
Many thanks to ffmpeg developers and to the community that support them!
*******************************************************************************/

#ifndef SDL2FFMPEG_H
#define SDL2FFMPEG_H

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "mb/interface/IContinuousMediaProvider.h"
#include "mb/interface/sdl/SDLDeviceScreen.h"

/* SDL2ffmpeg cplusplus compat begin */
extern "C" {

#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libavutil/avstring.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"

// AVFILTER BEGIN
# include "libavfilter/avcodec.h"
# include "libavfilter/avfilter.h"
# include "libavfilter/buffersink.h"
# include "libavfilter/buffersrc.h"
// AVFILTER end

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
}

#ifndef INT64_MIN
#define INT64_MIN		(-__INT64_C(9223372036854775807)-1)
#endif //INT64_MIN

#ifndef INT64_MAX
#define INT64_MAX		(__INT64_C(9223372036854775807))
#endif //INT64_MAX

/* SDL2ffmpeg cplusplus compat end*/


#include <SDL.h>
#include <SDL_thread.h>

#define SCALEBITS 10
#define ONE_HALF  (1 << (SCALEBITS - 1))
#define FIX(x)    ((int) ((x) * (1<<SCALEBITS) + 0.5))

#define RGB_TO_Y_CCIR(r, g, b) \
((FIX(0.29900*219.0/255.0) * (r) + FIX(0.58700*219.0/255.0) * (g) + \
  FIX(0.11400*219.0/255.0) * (b) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)

#define RGB_TO_U_CCIR(r1, g1, b1, shift)\
(((- FIX(0.16874*224.0/255.0) * r1 - FIX(0.33126*224.0/255.0) * g1 + \
		FIX(0.50000*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS \
    		 + shift)) + 128)

#define RGB_TO_V_CCIR(r1, g1, b1, shift)\
(((FIX(0.50000*224.0/255.0) * r1 - FIX(0.41869*224.0/255.0) * g1 - \
   FIX(0.08131*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + \
		   shift)) + 128)

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 5

/* SDL audio buffer size, in samples. Should be small to have precise
   A/V sync as SDL does not have hardware buffer fullness info. */
#define SDL_AUDIO_BUFFER_SIZE 1024

//Define audio macro when using ffmpeg 2.0.x.
#ifndef AVCODEC_MAX_AUDIO_FRAME_SIZE
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
#endif

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.01
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
/* TODO: We assume that a decoded and resampled frame fits into this buffer */
#define SAMPLE_ARRAY_SIZE (8 * 65536)

#define VIDEO_PICTURE_QUEUE_SIZE 3

#define ALPHA_BLEND(a, oldp, newp, s)\
		((((oldp << s) * (255 - (a))) + (newp * (a))) / (255 << s))

#define RGBA_IN(r, g, b, a, s) {\
	unsigned int v = ((const uint32_t *)(s))[0];\
	a = (v >> 24) & 0xff;\
	r = (v >> 16) & 0xff;\
	g = (v >> 8) & 0xff;\
	b = v & 0xff;\
}

#define YUVA_IN(y, u, v, a, s, pal) {\
	unsigned int val = ((const uint32_t *)(pal))[*(const uint8_t*)(s)];\
	a = (val >> 24) & 0xff;\
	y = (val >> 16) & 0xff;\
	u = (val >> 8) & 0xff;\
	v = val & 0xff;\
}

#define YUVA_OUT(d, y, u, v, a) {\
	((uint32_t *)(d))[0] = (a << 24) | (y << 16) | (u << 8) | v;\
}

#define BPP 1

#include <cmath>
#include <string>
#include <iostream>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	typedef struct MyAVPacketList {
		AVPacket pkt;
		struct MyAVPacketList *next;
		int serial;
	} MyAVPacketList;

	typedef struct PacketQueue {
		MyAVPacketList *first_pkt, *last_pkt;
		int nb_packets;
		int size;
		int abort_request;
		int serial;
		SDL_mutex *mutex;
		SDL_cond *cond;
	} PacketQueue;

	typedef struct VideoPicture {
		double pts;                //presentation timestamp for this picture
		int64_t pos;               //byte position in file
		SDL_Texture* tex;
		int width, height;         // source height & width
		int allocated;
		int reallocate;
		int serial;

		AVRational sar;

		//tmcode
		AVFrame* src_frame;
	} VideoPicture;

	typedef struct AudioParams {
		int freq;
		int channels;
		int64_t channel_layout;
		enum AVSampleFormat fmt;
	} AudioParams;

	typedef struct Clock {
		double pts;           /* clock base */
		double pts_drift;     /* clock base minus time at which we updated the clock */
		double last_updated;
		double speed;
		int serial;           /* clock is based on a packet with this serial */
		int paused;
		int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
	} Clock;

	typedef struct VideoState {
		SDL_Thread *read_tid;
		SDL_Thread *video_tid;
		AVInputFormat *iformat;
		int no_background;
		int abort_request;
		int force_refresh;
		int paused;
		int last_paused;
		int queue_attachments_req;
		int seek_req;
		int seek_flags;
		int64_t seek_pos;
		int64_t seek_rel;
		int read_pause_return;
		AVFormatContext *ic;
		int realtime;
		int audio_finished;
		int video_finished;

		Clock audclk;
		Clock vidclk;
		Clock extclk;

		int audio_stream;

		int av_sync_type;

		double audio_clock;
		int audio_clock_serial;
		double audio_diff_cum; /* used for AV difference average computation */
		double audio_diff_avg_coef;
		double audio_diff_threshold;
		int audio_diff_avg_count;
		AVStream *audio_st;
		PacketQueue audioq;
		int audio_hw_buf_size;
		uint8_t silence_buf[SDL_AUDIO_BUFFER_SIZE];
		uint8_t *audio_buf;
		uint8_t *audio_buf1;
		unsigned int audio_buf_size; /* in bytes */
		unsigned int audio_buf1_size;
		int audio_buf_index; /* in bytes */
		int audio_write_buf_size;
		int audio_buf_frames_pending;
		AVPacket audio_pkt_temp;
		AVPacket audio_pkt;
		int audio_pkt_temp_serial;
		int audio_last_serial;
		struct AudioParams audio_src;
		struct AudioParams audio_filter_src; // AVFILTER
		struct AudioParams audio_tgt;
		struct SwrContext *swr_ctx;
		int frame_drops_early;
		int frame_drops_late;
		AVFrame *frame;
		int64_t audio_frame_next_pts;
		int16_t sample_array[SAMPLE_ARRAY_SIZE];
		int sample_array_index;
		int last_i_start;
		RDFTContext *rdft;
		int rdft_bits;
		FFTSample *rdft_data;
		int xpos;
		double last_vis_time;

		double frame_timer;
		double frame_last_pts;
		double frame_last_duration;
		double frame_last_dropped_pts;
		double frame_last_returned_time;
		double frame_last_filter_delay;
		int64_t frame_last_dropped_pos;
		int frame_last_dropped_serial;
		int video_stream;
		AVStream *video_st;
		PacketQueue videoq;
		int64_t video_current_pos;      // current displayed file pos
		double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
		VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
		int pictq_size, pictq_rindex, pictq_windex;
		SDL_mutex *pictq_mutex;
		SDL_cond *pictq_cond;
		SDL_Rect last_display_rect;

		char filename[1024];
		int step;

		// AVFILTER begin
		AVFilterContext *in_video_filter;   // the first filter in the video chain
		AVFilterContext *out_video_filter;  // the last filter in the video chain
		AVFilterContext *in_audio_filter;   // the first filter in the audio chain
		AVFilterContext *out_audio_filter;  // the last filter in the audio chain
		AVFilterGraph *agraph;              // audio filter graph
		// AVFILTER end

		int last_video_stream, last_audio_stream;

		SDL_cond *continue_read_thread;
	} VideoState;

  class SDL2ffmpeg {
	public:
		static const short AV_SYNC_AUDIO_MASTER   = 0;
		static const short AV_SYNC_VIDEO_MASTER   = 1;
		static const short AV_SYNC_EXTERNAL_CLOCK = 2;

	private:
		//stream status
		static const short ST_PLAYING = 0;
		static const short ST_PAUSED  = 1;
		static const short ST_STOPPED = 2;

		//desired audio specification default values
		static const int ASD_SAMPLES  = 4096;
		static const int ASD_FREQ     = 44100;
		static const int ASD_CHANNELS = 2;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		static const int ASD_FORMAT   = AUDIO_S16LSB;
#else
		static const int ASD_FORMAT   = AUDIO_S16MSB;
#endif

		int wanted_stream[AVMEDIA_TYPE_NB];
		int seek_by_bytes;
		int av_sync_type;
		int64_t start_time;
		int64_t duration;
		int workaround_ffmpeg_bugs;
		int fast;
		int genpts;
		int lowres;
		int error_concealment;
		int decoder_reorder_pts;
		int framedrop;
		int infinite_buffer;
		int rdftspeed;

		char* vfilters;
		char* afilters;

		AVPacket flush_pkt;

		VideoState* vs;

		static bool init;
		static short refCount;

		static pthread_mutex_t aiMutex;
		static set<SDL2ffmpeg*> aInstances;

		AVFrame vFrame;
		AVPacket vPkt;

		int audioFreq;
		Uint8 audioChannels;

		short status;
		SDL_AudioSpec wantedSpec;
		static SDL_AudioSpec spec;
		SDL_AudioCVT acvt;
		float soundLevel;
		SDL_Texture* texture;

		bool hasSDLAudio;
		bool reof;
		bool hasPic;

		bool abortRequest;

		int64_t mono_cb_time;
		int monoStep;

		IContinuousMediaProvider* cmp;

	public:
		SDL2ffmpeg(IContinuousMediaProvider* cmp, const char *filename);
		~SDL2ffmpeg();

	private:
		void release();
		void close(bool quit);

		string ffmpegErr(int err);

		void openStreams();
		bool prepare();

	public:
		bool hasVideoStream();
		void play();
		void stop();
		void pause();
		void resume();

		void getOriginalResolution(int* width, int* height);

		double getDuration();
		double getPosition();
		void seek(int64_t pos);

		void setTexture(SDL_Texture* texture);
		bool hasTexture();
		SDL_Texture* getTexture();

		bool hasPicture();

        void setSoundLevel(float level);

		bool getAudioSpec(
				SDL_AudioSpec* spec, int sample_rate, uint8_t channels);

		static char* interleave(uint8_t* src, int srcLen, double ratio);

		static char* createCVT(
				uint8_t* src, int srcLen, double ratio, int sampleSize);

		static void clamp(short* buf, int len);

	private:
		static int cmp_audio_fmts(
				enum AVSampleFormat fmt1, 
				int64_t channel_count1,
				enum AVSampleFormat fmt2, 
				int64_t channel_count2);

		static int64_t get_valid_channel_layout(
				int64_t channel_layout, int channels);

		int nts_packet_queue_put(PacketQueue *q, AVPacket *pkt);
		int packet_queue_put(PacketQueue *q, AVPacket *pkt);
		void packet_queue_init(PacketQueue *q);
		void packet_queue_flush(PacketQueue *q);
		void packet_queue_destroy(PacketQueue *q);
		void packet_queue_abort(PacketQueue *q);
		void packet_queue_start(PacketQueue *q);
		int packet_queue_get(
			PacketQueue *q, AVPacket *pkt, int block, int* serial);

		void render_vp(VideoPicture* vp);

		void stream_close();

		double get_clock(Clock* c);
		void set_clock_at(Clock* c, double pts, int serial, double time);
		void set_clock(Clock* c, double pts, int serial);
		void set_clock_speed(Clock* c, double speed);
		void init_clock(Clock* c, int* queue_serial);
		void sync_clock_to_slave(Clock* c, Clock* slave);
		int get_master_sync_type();

	public:
		double get_master_clock();

	private:
		void check_external_clock_speed();

		void stream_seek(int64_t pos, int64_t rel, int seek_by_bytes);
		void stream_toggle_pause();
		void toggle_pause();
		void step_to_next_frame();
		double compute_target_delay(double delay);
		void pictq_next_picture();
		int pictq_prev_picture();
		void update_video_pts(double pts, int64_t pos, int serial);

	public:
		static void video_refresh(void* opaque, double* remaining_time);
		void alloc_picture();

	private:
		int queue_picture(AVFrame *src_frame, double pts, int64_t pos, int serial);
		int get_video_frame(AVFrame *frame, AVPacket* pkt, int* serial);

		//AVFILTER begin
		int configure_filtergraph(
				AVFilterGraph* graph, 
				const char* filtergraph,
				AVFilterContext* source_ctx, 
				AVFilterContext* sink_ctx);

		int configure_video_filters(
				AVFilterGraph* graph,
				const char* vfilters,
				AVFrame* frame);

		int configure_audio_filters(
				const char *afilters, 
				int force_output_format);
		//AVFILTER end

		static int video_thread(void *arg);

		int synchronize_audio(int nb_samples);

		int audio_decode_frame();
		static void sdl_audio_callback(void *opaque, Uint8 *stream, int len);

		int audio_open(
				int64_t wanted_channel_layout,
				int wanted_nb_channels,
				int wanted_sample_rate,
				struct AudioParams *audio_hw_params);

		int stream_component_open(int stream_index);
		void stream_component_close(int stream_index);

		static int decode_interrupt_cb(void *ctx);
		static int is_realtime(AVFormatContext *s);
		int read_init();
		static int read_thread(void *arg);

		void stream_cycle_channel(int codec_type);

		static int lockmgr(void **mtx, enum AVLockOp op);

		static AVDictionary* filter_codec_opts(
				AVDictionary *opts,
				enum AVCodecID codec_id,
				AVFormatContext *s, 
				AVStream *st, 
				AVCodec *codec);
  };
}
}
}
}
}
}

#endif /* SDL2FFMPEG_H */
