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

#ifndef FFmpegVideoProvider_H_
#define FFmpegVideoProvider_H_

#include "../../../../../config.h"

#include "../IContinuousMediaProvider.h"
#include "../audio/FusionSoundAudioProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include <math.h>

#include <pthread.h>

#include <direct/types.h>
#include <direct/list.h>
#include <direct/messages.h>
#include <direct/memcpy.h>
#include <direct/thread.h>
#include <direct/util.h>

#include <dvc/dvc.h>
#include <directfb.h>

#include "fusionsound/fusionsound_limits.h"

#include "libavutil/common.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif

typedef struct {
	DirectLink       link;
	AVPacket         packet;
} PacketLink;

typedef struct {
	PacketLink      *list;
	int              size;
	s64              max_len;
	int              max_size;
	pthread_mutex_t  lock;
} PacketQueue;

typedef struct {
	DirectLink            link;
	IDirectFBEventBuffer *buffer;
} EventLink;

typedef struct {
	int                            ref;

	DFBVideoProviderStatus         status;
	DFBVideoProviderPlaybackFlags  flags;
	double                         speed;
	float                          volume;

	u16                            brightness;
	u16                            contrast;
	u16                            saturation;

	IDirectFBDataBuffer           *buffer;
	bool                           seekable;
	void                          *iobuf;

	ByteIOContext                  pb;
	AVFormatContext*               context;

	s64                            start_time;

	struct {
		DirectThread             *thread;
		pthread_mutex_t           lock;

		bool                      buffering;

		bool                      seeked;
		s64                       seek_time;
		int                       seek_flag;
	} input;

	struct {
		DirectThread             *thread;
		pthread_mutex_t           lock;
		pthread_cond_t            cond;

		AVStream                 *st;
		AVCodecContext           *ctx;
		AVCodec                  *codec;

		PacketQueue               queue;

		s64                       pts;

		double                    rate;

		bool                      seeked;

		IDirectFBSurface         *dest;
		DFBRectangle              rect;

		AVFrame                  *src_frame;
		DVCColormap              *colormap;

	} video;

	struct {
		DirectThread             *thread;
		pthread_mutex_t           lock;
		pthread_cond_t            cond;

		AVStream                 *st;
		AVCodecContext           *ctx;
		AVCodec                  *codec;

		PacketQueue               queue;

		s64                       pts;

		bool                      seeked;

		IFusionSound             *sound;
		IFusionSoundStream       *stream;
		IFusionSoundPlayback     *playback;

		int                       sample_size;
		int                       sample_rate;
		int                       buffer_size;
	} audio;

	ISurface*                      surface;
	DVFrameCallback                callback;
	void                          *ctx;
} IDirectFBVideoProvider_FFmpeg_data;


#define IO_BUFFER_SIZE       8 /* in kylobytes */

#define MAX_QUEUE_LEN        3 /* in seconds */

#define GAP_TOLERANCE    15000 /* in microseconds */

#define GAP_THRESHOLD   250000 /* in microseconds */

/*****************************************************************************/

#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	class FFmpegVideoProvider : public IContinuousMediaProvider {
		private:
			IDirectFBVideoProvider_FFmpeg_data* rContainer;
			int resumePos;
			int startPos;
			static bool _ffmpegInitialized;

		public:
			FFmpegVideoProvider(const char* mrl);
			virtual ~FFmpegVideoProvider();

		private:
			bool initializeFFmpeg(const char* mrl);

		public:
			void setAVPid(int aPid, int vPid);
			void* getContent();
			void feedBuffers();

		private:
			void getVideoSurfaceDescription(DFBSurfaceDescription* dsc);

		public:
			ISurface* getPerfectSurface();
			bool checkVideoResizeEvent(ISurface* frame);

			void getOriginalResolution(int* height, int* width);
			double getTotalMediaTime();
			int64_t getVPts();
			double getMediaTime();
			void setMediaTime(double pos);

		private:
			bool updateVisualData(ISurface* surface);

		public:
			static void dynamicRenderCallBack(void* surface);
			void playOver(
					ISurface* surface,
					bool hasVisual=true, IProviderListener* listener=NULL);

			void resume(ISurface* surface, bool hasVisual=true);
			void pause();
			void stop();
			void setSoundLevel(float level);
			bool releaseAll();
	};
}
}
}
}
}
}
}

#endif /*FFmpegVideoProvider_H_*/
