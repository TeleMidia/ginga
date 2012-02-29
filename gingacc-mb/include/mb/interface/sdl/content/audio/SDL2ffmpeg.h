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
Part of this file is extracted from SDL_ffmpeg library and ffplay
SDL_ffmpeg was created by Arjan Houben, but it is a discontinued library
SDL_ffmpeg was free software licensed under the LGPL
ffplay is part of FFmpeg library.
FFmpeg is free software licensed under the LGPL or GPL
Many thanks to these guys and to the community that support them!
*******************************************************************************/

#ifndef SDL2FFMPEG_H
#define SDL2FFMPEG_H

/* SDL_ffmpeg cplusplus compat begin */
extern "C" {
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#endif //INT64_C

#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif //UINT64_C

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"

#include <pthread.h>
}

/* SDL_ffmpeg cplusplus compat end*/

#include <cmath>

#include "SDL.h"
#include "SDL_thread.h"

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	typedef void (*Callback)(void *userdata, Uint8 *stream, int len);

	/* Struct to hold codec values */
	typedef struct Codec {
		/* video codec ID */
		int32_t videoCodecID;
		/* width of the output stream */
		int32_t width;
		/* height of the output stream */
		int32_t height;
		/* nominator part of the framerate */
		int32_t framerateNum;
		/* denominator part of the framerate */
		int32_t framerateDen;
		/* bitrate of video stream in bytes */
		int32_t videoBitrate;

		/*
		 * when variable bitrate is desired,
		 * this holds the minimal video bitrate
		 */
		int32_t videoMinRate;

		/*
		 * when variable bitrate is desired, this holds the maximal
		 * video bitrate
		 */
		int32_t videoMaxRate;

		/* audio codec ID */
		int32_t audioCodecID;
		/* number of audio channels in stream */
		int32_t channels;
		/* audio samplerate of output stream */
		int32_t sampleRate;
		/* bitrate of audio stream in bytes */
		int32_t audioBitrate;

		/* when variable bitrate is desired,
		 * this holds the minimal audio bitrate
		 */
		int32_t audioMinRate;

		/*
		 * when variable bitrate is desired,
		 * this holds the maximal audio bitrate
		 */
		int32_t audiooMaxRate;
	} Codec;

	/* Struct to hold packet buffers */
	typedef struct Packet {
		struct AVPacket *data;
		struct Packet *next;
	} Packet;

	/* Struct to hold audio data */
	typedef struct AudioFrame {
		/* Presentation timestamp, time at which this data should be used. */
		int64_t pts;
		/* Pointer to audio buffer, user adjustable. */
		uint8_t *buffer;
		/* Current size of this audio frame. */
		uint32_t size;
		/* Size of the complete audio frame. */
		uint32_t capacity;
		/* Value indicating wheter or not this is the last frame before EOF */
		int last;
	} AudioFrame;

	/* Struct to hold audio data */
	typedef struct VideoFrame {
		/* Presentation timestamp, time at which this data should be used. */
		int64_t pts;

		/*
		 * Value indicating if this frame holds data,
		 * or that it can be overwritten.
		 */
		int ready;

		/* Value indicating whether or not this is the last frame before EOF */
		int last;

		SDL_Texture* texture;
		int temph;
		int tempw;
	} VideoFrame;

	/* This is the basic stream for SDL2ffmpeg */
	typedef struct Stream {
		/* Pointer to ffmpeg data, internal use only! */
		struct AVStream *_ffmpeg;
		/* Intermediate frame which will be used when decoding */
		struct AVFrame *decodeFrame;
		/* Intermediate frame which will be used when encoding */
		struct AVFrame *encodeFrame;
		int encodeFrameBufferSize;
		uint8_t *encodeFrameBuffer;
		int encodeAudioInputSize;
		uint64_t frameCount;
		/* buffer for decoded audio data */
		int8_t *sampleBuffer;
		/* amount of data in samplebuffer */
		int sampleBufferSize;
		/* position of data in samplebuffer */
		int sampleBufferOffset;
		/* timestamp which fits the data in samplebuffer */
		int64_t sampleBufferTime;
		/* packet buffer */
		Packet *buffer;
		/* mutex for multi threaded access to buffer */
		SDL_mutex* mutex;
		/* Id of the stream */
		int id;

		/*
		 * This holds the lastTimeStamp calculated,
		 * usefull when frames don't provide
		 * a usefull dts/pts, also used for determining at
		 * what point we are in the file
		 */
		int64_t lastTimeStamp;

		/*
		 * pointer to the next stream,
		 * or NULL if current stream is the last one
		 */
		struct Stream *next;
		struct SwsContext* conversionContext;
	} Stream;

	/* Struct to hold information about file */
	typedef struct content {
		void* private_data;

		/* Pointer to ffmpeg data, internal use only! */
		AVFormatContext* _ffmpeg;

		/* Video streams */
		Stream* vs;
		/* Audio streams */
		Stream* as;

		/* stream mutex */
		SDL_mutex* streamMutex;

		/* Amount of video streams in file */
		uint32_t videoStreams;
		/* Amount of audio streams in file */
		uint32_t audioStreams;

		/* Pointer to active videoStream, NULL if no video stream is active */
		Stream* videoStream;
		/* Pointer to active audioStream, NULL if no audio stream is active */
		Stream* audioStream;

		/* Holds the lowest timestamp which will be decoded */
		int64_t minimalTimestamp;
	} Content;

	class SDL2ffmpeg {
		private:
			Content content;
			static bool initialized;

		public:
			SDL2ffmpeg(const char* mrl);
			~SDL2ffmpeg();

		private:
			void open(const char* mrl);
			static void init();

		public:
			void releaseAudioFrame(AudioFrame* frame);
			void releaseVideoFrame(VideoFrame* frame);

			AudioFrame* createAudioFrame(unsigned long int bytes);
			VideoFrame* createVideoFrame();

			int getNumOfAudioStreams();
			int getNumOfVideoStreams();

			int getVideoFrame(VideoFrame* frame);

			Stream* getAudioStream(uint32_t audioID);
			int selectAudioStream(int audioID);

			Stream* getVideoStream(uint32_t videoID);
			int selectVideoStream(int videoID);

			int seek(uint64_t timestamp);
			int seekRelative(int64_t timestamp);

			int flush();

			int getAudioFrame(AudioFrame *frame);

			int64_t getPosition();

			float getFrameRate(
					Stream *stream,
					int *numerator,
					int *denominator);

			SDL_AudioSpec getAudioSpec(
					uint16_t samples, Callback callback);

			uint64_t getDuration();
			uint64_t getAudioDuration();
			uint64_t getVideoDuration();

			int getVideoSize(int *w, int *h);

			bool isAudioValid();
			bool isVideoValid();

		private:
			int getPacket();
			Packet* getAudioPacket();
			Packet* getVideoPacket();
			int decodeAudioFrame(AVPacket* pkt, AudioFrame* frame);
			int decodeVideoFrame(AVPacket* pkt, VideoFrame* frame);

			static struct SwsContext* createContext(
					int inWidth, int inHeight,
					enum PixelFormat inFormat,
					int outWidth, int outHeight,
					enum PixelFormat outFormat);
	};
}
}
}
}
}
}

#endif /* SDL2FFMPEG_H */
