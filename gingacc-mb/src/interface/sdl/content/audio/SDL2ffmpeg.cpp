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

#include "mb/interface/sdl/content/audio/SDL2ffmpeg.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	bool SDL2ffmpeg::initialized = false;

	SDL2ffmpeg::SDL2ffmpeg(const char* mrl) {
		memset(&content, 0, sizeof(Content));
		content.streamMutex = SDL_CreateMutex();

		open(mrl);
	}

	SDL2ffmpeg::~SDL2ffmpeg() {
		flush();

		Stream *s = content.vs;
		while (s != NULL) {
			Stream* old = s;

			s = s->next;

			SDL_DestroyMutex(old->mutex);

			av_free(old->decodeFrame);

			if (old->_ffmpeg) {
				avcodec_close(old->_ffmpeg->codec);
			}

			delete old;
		}

		s = content.as;
		while (s != NULL)  {
			Stream *old = s;

			s = s->next;

			SDL_DestroyMutex(old->mutex);

			av_free(old->sampleBuffer);

			if (old->_ffmpeg) {
				avcodec_close( old->_ffmpeg->codec);
			}

			delete old;
		}

		if (content._ffmpeg != NULL) {
			avformat_close_input(&content._ffmpeg);
		}

		SDL_DestroyMutex(content.streamMutex);
	}


	void SDL2ffmpeg::open(const char* mrl) {
		uint32_t i;

		init();

		if (avformat_open_input(
				(AVFormatContext**)(&content._ffmpeg), mrl, 0, 0) != 0) {

			cout << "SDL2ffmpeg::open could not open '" << mrl << "'" << endl;
			return;
		}

		if (avformat_find_stream_info(content._ffmpeg, NULL) < 0) {
			cout << "SDL2ffmpeg::open could not retrieve info for '" << mrl;
			cout << "'" << endl;
			return;
		}

		for (i = 0; i < content._ffmpeg->nb_streams; i++) {
			/* disable all streams by default */
			content._ffmpeg->streams[i]->discard = AVDISCARD_ALL;
			if (content._ffmpeg->streams[i]->codec->codec_type ==
					AVMEDIA_TYPE_VIDEO) {

				/*
				 * if this is a packet of the correct type,
				 * then we create a new stream
				 */
				Stream* stream = new Stream;

				if (stream) {
					/* we set our stream to zero */
					memset(stream, 0, sizeof(Stream));

					/* save unique streamid */
					stream->id = i;

					/* _ffmpeg holds data about streamcodec */
					stream->_ffmpeg = content._ffmpeg->streams[i];

					/* get the correct decoder for this stream */
					AVCodec *codec = avcodec_find_decoder(
							stream->_ffmpeg->codec->codec_id );

					if (!codec) {
						delete stream;
						cout << "SDL2ffmpeg::open ";
						cout << "could not find video codec" << endl;

					} else if (avcodec_open2(
							content._ffmpeg->streams[i]->codec,
							codec,
							NULL) < 0 ) {

						delete stream;
						cout << "SDL2ffmpeg::open ";
						cout << "could not open video codec" << endl;

					} else {
						stream->mutex = SDL_CreateMutex();

						stream->decodeFrame = avcodec_alloc_frame();

						Stream** s = &content.vs;
						while (*s) {
							*s = (*s)->next;
						}

						*s = stream;

						content.videoStreams++;
					}
				}

			} else if (content._ffmpeg->streams[i]->codec->codec_type ==
					AVMEDIA_TYPE_AUDIO) {

				/*
				 * if this is a packet of the correct type,
				 * then we create a new stream
				 */
				Stream* stream = new Stream;

				if (stream) {
					/* we set our stream to zero */
					memset(stream, 0, sizeof( Stream));

					/* save unique streamid */
					stream->id = i;

					/* _ffmpeg holds data about streamcodec */
					stream->_ffmpeg = content._ffmpeg->streams[i];

					/* get the correct decoder for this stream */
					AVCodec* codec = avcodec_find_decoder(
							content._ffmpeg->streams[i]->codec->codec_id);

					if (!codec) {
						delete stream;
						cout << "SDL2ffmpeg::open ";
						cout << "could not find audio codec" << endl;

					} else if (avcodec_open2(
							content._ffmpeg->streams[i]->codec,
							codec,
							NULL) < 0) {

						delete stream;
						cout << "SDL2ffmpeg::open ";
						cout << "could not open audio codec" << endl;

					} else {
						stream->mutex = SDL_CreateMutex();

						stream->sampleBuffer       = (int8_t*)av_malloc(
								AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(int16_t));

						stream->sampleBufferSize   = 0;
						stream->sampleBufferOffset = 0;
						stream->sampleBufferTime   = AV_NOPTS_VALUE;

						Stream** s = &content.as;
						while (*s) {
							*s = (*s)->next;
						}

						*s = stream;

						content.audioStreams++;
					}
				}
			}
		}
	}

	void SDL2ffmpeg::init() {
		if (!initialized) {
			initialized = true;

			avcodec_register_all();
			av_register_all();
			avformat_network_init();
		}
	}

	void SDL2ffmpeg::releaseAudioFrame(AudioFrame* frame) {
		if (!frame) {
			return;
		}

		av_free(frame->buffer);

		delete frame;
	}

	void SDL2ffmpeg::releaseVideoFrame(VideoFrame* frame) {
		if (!frame) {
			return;
		}

		if (frame->texture) {
			SDL_DestroyTexture(frame->texture);
			frame->texture = 0;
		}

		delete frame;
	}

	AudioFrame* SDL2ffmpeg::createAudioFrame(unsigned long int bytes) {
		SDL_LockMutex(content.streamMutex);

		if (!content.audioStream || !bytes) {
			SDL_UnlockMutex(content.streamMutex);
			return NULL;
		}

		/* allocate new frame */
		AudioFrame *frame = new AudioFrame;
		memset(frame, 0, sizeof(AudioFrame));

		SDL_UnlockMutex(content.streamMutex);

		frame->capacity = bytes;
		frame->buffer   = (uint8_t*)av_malloc(bytes);
		frame->pts      = AV_NOPTS_VALUE;

		return frame;
	}

	VideoFrame* SDL2ffmpeg::createVideoFrame() {
		VideoFrame *frame = new VideoFrame;
		memset(frame, 0, sizeof(VideoFrame));

		return frame;
	}

	int SDL2ffmpeg::getNumOfAudioStreams() {
		return content.audioStreams;
	}

	int SDL2ffmpeg::getNumOfVideoStreams() {
		return content.videoStreams;
	}

	int SDL2ffmpeg::getVideoFrame(VideoFrame* frame) {
		SDL_LockMutex(content.streamMutex);

		if (!frame || !content.videoStream) {
			SDL_UnlockMutex(content.streamMutex);
			return 0;
		}

		SDL_LockMutex(content.videoStream->mutex);

		frame->ready = 0;
		frame->last  = 0;

		Packet* pack = getVideoPacket();

		while (!pack && !frame->last) {
			pack        = getVideoPacket();
			frame->last = getPacket();
		}

		while (pack && !frame->ready) {
			decodeVideoFrame(pack->data, frame);

			av_free_packet(pack->data);
			delete pack;

			pack = getVideoPacket();

			while (!pack && !frame->last) {
				pack        = getVideoPacket();
				frame->last = getPacket();
			}
		}

		if (pack) {
			pack->next = content.videoStream->buffer;
			content.videoStream->buffer = pack;

		} else if (!frame->ready && frame->last) {
			decodeVideoFrame(NULL, frame);
		}

		SDL_UnlockMutex(content.videoStream->mutex);
		SDL_UnlockMutex(content.streamMutex);

		return frame->ready;
	}

	Stream* SDL2ffmpeg::getAudioStream(uint32_t audioID) {
		uint32_t i;

		/* check if we have any audiostreams */
		if (!content.audioStreams) {
			return 0;
		}

		Stream*s = content.as;

		for (i = 0; i < audioID && s; i++) {
			s = s->next;
		}

		return s;
	}

	int SDL2ffmpeg::selectAudioStream(int audioID) {
		int i;

		SDL_LockMutex(content.streamMutex);

		if (!content.audioStreams || audioID >= (int)content.audioStreams) {
			SDL_UnlockMutex(content.streamMutex);

			cout << "SDL2ffmpeg::selectAudioStream ";
			cout << "requested audio stream ID is not available in file";
			cout << endl;

			return -1;
		}

		Stream* stream = content.as;

		while (stream && stream->_ffmpeg) {
			stream->_ffmpeg->discard = AVDISCARD_ALL;
			stream = stream->next;
		}

		if (audioID < 0) {
			content.audioStream = 0;

		} else {
			content.audioStream = content.as;
			for (i = 0; i < audioID && content.audioStream; i++) {
				content.audioStream = content.audioStream->next;
			}

			content.audioStream->_ffmpeg->discard = AVDISCARD_DEFAULT;
		}

		SDL_UnlockMutex(content.streamMutex);

		return 0;
	}

	Stream* SDL2ffmpeg::getVideoStream(uint32_t videoID) {
		uint32_t i;

		/* check if we have any audiostreams */
		if (!content.videoStreams) {
			return 0;
		}

		/* check if the requested id is possible */
		if (videoID >= content.videoStreams) {
			return 0;
		}

		Stream*s = content.vs;

		/* return audiostream linked to audioID */
		for (i = 0; i < videoID && s; i++) {
			s = s->next;
		}

		return s;
	}

	int SDL2ffmpeg::selectVideoStream(int videoID) {
		int i;

		/* when changing audio/video stream, streamMutex should be locked */
		SDL_LockMutex(content.streamMutex);

		if (videoID >= (int)content.videoStreams) {
			SDL_UnlockMutex(content.streamMutex);

			cout << "SDL2ffmpeg::selectVideoStream ";
			cout << "requested video stream ID is not available in file";
			cout << endl;

			return -1;
		}

		Stream* stream = content.vs;

		while (stream && stream->_ffmpeg) {
			stream->_ffmpeg->discard = AVDISCARD_ALL;
			stream = stream->next;
		}

		if (videoID < 0) {
			content.videoStream = 0;

		} else {
			content.videoStream = content.vs;

			for (i = 0; i < videoID && content.videoStream; i++) {
				content.videoStream = content.videoStream->next;
			}

			content.videoStream->_ffmpeg->discard = AVDISCARD_DEFAULT;
		}

		SDL_UnlockMutex(content.streamMutex);

		return 0;
	}

	int SDL2ffmpeg::seek(uint64_t timestamp ) {
		if (getDuration() < timestamp) {
			cout << "SDL2ffmpeg::seek ";
			cout << "can not seek past end of file" << endl;
			return -1;
		}

		uint64_t seekPos = timestamp * (AV_TIME_BASE / 1000);

		av_seek_frame(content._ffmpeg, -1, seekPos, AVSEEK_FLAG_BACKWARD);

		content.minimalTimestamp = timestamp;

		flush();

		return 0;
	}

	int SDL2ffmpeg::seekRelative(int64_t timestamp ) {
		return seek(getPosition() + timestamp);
	}

	int SDL2ffmpeg::flush() {
		SDL_LockMutex(content.streamMutex);

		if (content.audioStream) {
			SDL_LockMutex(content.audioStream->mutex);

			Packet* pack = content.audioStream->buffer;

			while (pack) {
				Packet* old = pack;

				pack = pack->next;

				av_free(old->data);
				delete old;
			}

			content.audioStream->buffer = 0;

			if (content.audioStream->_ffmpeg) {
				avcodec_flush_buffers(content.audioStream->_ffmpeg->codec);
			}

			SDL_UnlockMutex(content.audioStream->mutex);
		}

		if (content.videoStream) {
			SDL_LockMutex(content.videoStream->mutex);

			Packet* pack = content.videoStream->buffer;

			while (pack) {
				Packet* old = pack;

				pack = pack->next;

				av_free(old->data);
				delete old;
			}

			content.videoStream->buffer = 0;

			if (content.videoStream->_ffmpeg) {
				avcodec_flush_buffers(content.videoStream->_ffmpeg->codec);
			}

			SDL_UnlockMutex(content.videoStream->mutex);
		}

		SDL_UnlockMutex(content.streamMutex);

		return 0;
	}

	int SDL2ffmpeg::getAudioFrame(AudioFrame *frame) {
		if (!frame ) {
			return -1;
		}

		SDL_LockMutex(content.streamMutex);

		if (!content.audioStream) {
			SDL_UnlockMutex(content.streamMutex);

			cout << "SDL2ffmpeg::getAudioFrame ";
			cout << "no valid audio stream selected" << endl;
			return 0;
		}

		SDL_LockMutex(content.audioStream->mutex);

		frame->last = 0;
		frame->size = 0;

		Packet* pack = getAudioPacket();

		while (!pack && !frame->last) {
			pack = getAudioPacket();
			frame->last = getPacket();
		}

		while (pack && decodeAudioFrame(pack->data, frame)) {
			av_free_packet( pack->data );
			delete pack;
			pack = NULL;

			if (frame->size < frame->capacity) {
				pack = getAudioPacket();

				while (!pack && !frame->last) {
					pack = getAudioPacket();
					frame->last = getPacket();
				}
			}
		}

		if (pack) {
			pack->next = content.audioStream->buffer;
			content.audioStream->buffer = pack;
		}

		SDL_UnlockMutex(content.audioStream->mutex);
		SDL_UnlockMutex(content.streamMutex);

		return (frame->size == frame->capacity);
	}

	int64_t SDL2ffmpeg::getPosition() {
		SDL_LockMutex(content.streamMutex);

		int64_t pos = 0;

		if (content.audioStream) {
			pos = content.audioStream->lastTimeStamp;
		}

		if (content.videoStream && content.videoStream->lastTimeStamp > pos) {
			pos = content.videoStream->lastTimeStamp;
		}

		SDL_UnlockMutex(content.streamMutex);

		return pos;
	}

	float SDL2ffmpeg::getFrameRate(
			Stream* stream, int* nominator, int* denominator) {

		if (stream && stream->_ffmpeg && stream->_ffmpeg->codec) {
			if (nominator) {
				*nominator = stream->_ffmpeg->r_frame_rate.num;
			}

			if (denominator) {
				*denominator = stream->_ffmpeg->r_frame_rate.den;
			}

			return (float)stream->_ffmpeg->r_frame_rate.num /
					stream->_ffmpeg->r_frame_rate.den;

		} else {
			cout << "SDL2ffmpeg::getFrameRate ";
			cout << "could not retreive frame rate from stream" << endl;

			if (nominator) {
				*nominator = 0;
			}

			if (denominator) {
				*denominator = 0;
			}
		}

		return 0.0;
	}

	SDL_AudioSpec SDL2ffmpeg::getAudioSpec(
			uint16_t samples, Callback callback) {

		SDL_AudioSpec spec;

		memset(&spec, 0, sizeof(SDL_AudioSpec));

		SDL_LockMutex(content.streamMutex);

		if (content.audioStream) {
			spec.format   = AUDIO_S16SYS;
			spec.samples  = samples;
			spec.userdata = &content;
			spec.callback = callback;
			spec.freq     = content.audioStream->_ffmpeg->codec->sample_rate;
			spec.channels = (uint8_t)
					content.audioStream->_ffmpeg->codec->channels;

		} else {
			cout << "SDL2ffmpeg::getAudioSpec ";
			cout << "no valid audio stream selected" << endl;
		}

		SDL_UnlockMutex(content.streamMutex);

		return spec;
	}

	uint64_t SDL2ffmpeg::getDuration() {
		return content._ffmpeg->duration / ( AV_TIME_BASE / 1000 );
	}

	uint64_t SDL2ffmpeg::getAudioDuration() {
		SDL_LockMutex(content.streamMutex);

		uint64_t duration = 0;

		if (!content.audioStream) {
		   duration = av_rescale(
				   1000 * content.audioStream->_ffmpeg->duration,
				   content.audioStream->_ffmpeg->time_base.num,
				   content.audioStream->_ffmpeg->time_base.den);

		} else {
			cout << "SDL2ffmpeg::getAudioDuration ";
			cout << "no valid audio stream selected" << endl;
		}

		SDL_UnlockMutex(content.streamMutex);

		return duration;
	}

	uint64_t SDL2ffmpeg::getVideoDuration() {
		SDL_LockMutex(content.streamMutex);

		uint64_t duration = 0;

		if (content.videoStream) {
			duration = av_rescale(
					1000 * content.videoStream->_ffmpeg->duration,
					content.videoStream->_ffmpeg->time_base.num,
					content.videoStream->_ffmpeg->time_base.den);

		} else {
			cout << "SDL2ffmpeg::getVideoDuration ";
			cout << "no valid video stream selected" << endl;
		}

		SDL_UnlockMutex(content.streamMutex);

		return duration;
	}

	int SDL2ffmpeg::getVideoSize(int *w, int *h ) {
		if (!w || !h ) {
			return -1;
		}

		SDL_LockMutex(content.streamMutex);

		if (content.videoStream) {
			*w = content.videoStream->_ffmpeg->codec->width;
			*h = content.videoStream->_ffmpeg->codec->height;

			SDL_UnlockMutex(content.streamMutex);

			return 0;

		} else {
			cout << "no valid video stream selected" << endl;
		}

		*w = 0;
		*h = 0;

		SDL_UnlockMutex(content.streamMutex);

		return -1;
	}

	bool SDL2ffmpeg::isAudioValid() {
		return (content.audioStream != NULL);
	}

	bool SDL2ffmpeg::isVideoValid() {
		return (content.videoStream != NULL);
	}

	int SDL2ffmpeg::getPacket() {
		AVPacket *pack = (AVPacket*)av_malloc(sizeof(AVPacket));

		av_init_packet(pack);

		int decode = av_read_frame(content._ffmpeg, pack);

		if (decode < 0) {
			av_free(pack);

			return 1;
		}

		if (av_dup_packet(pack)) {
			av_free_packet( pack );

		} else {
			if (content.audioStream &&
					pack->stream_index == content.audioStream->id) {

				Packet* temp = new Packet;

				temp->data = pack;
				temp->next = 0;

				Packet** p = &content.audioStream->buffer;

				while (*p) {
					p = &(*p)->next;
				}

				*p = temp;

			} else if (content.videoStream &&
					pack->stream_index == content.videoStream->id) {

				Packet* temp = new Packet;

				temp->data = pack;
				temp->next = 0;

//				SDL_LockMutex( content.videoStream->mutex );

				Packet** p = &content.videoStream->buffer;

				while (*p) {
					p = &(*p)->next;
				}

				*p = temp;

//				SDL_UnlockMutex( content.videoStream->mutex );

			} else {
				av_free_packet(pack);
			}
		}

		return 0;
	}

	Packet* SDL2ffmpeg::getAudioPacket() {
		if (!content.audioStream) {
			return 0;
		}

		Packet* pack = 0;

		if (content.audioStream->buffer) {
			pack = content.audioStream->buffer;
			content.audioStream->buffer = pack->next;
		}

		return pack;
	}

	Packet* SDL2ffmpeg::getVideoPacket() {
		if (!content.videoStream) {
			return 0;
		}

		Packet* pack = 0;

		if (content.videoStream->buffer) {
			pack = content.videoStream->buffer;
			content.videoStream->buffer = pack->next;
		}

		return pack;
	}

	int SDL2ffmpeg::decodeAudioFrame(AVPacket *pack, AudioFrame *frame) {
		uint8_t *data = pack->data;
		int size      = pack->size;
		int audioSize = AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(int16_t);

		if (content.audioStream->sampleBufferSize) {
			if (!frame->size) {
				frame->pts = content.audioStream->sampleBufferTime;
			}

			int fs = frame->capacity - frame->size;

			if (fs < content.audioStream->sampleBufferSize) {
				memcpy(
						frame->buffer + frame->size,
						content.audioStream->sampleBuffer +
								content.audioStream->sampleBufferOffset,
						fs);

				content.audioStream->sampleBufferSize   -= fs;
				content.audioStream->sampleBufferOffset += fs;

				frame->size = frame->capacity;

			} else {
				memcpy(
						frame->buffer + frame->size,
						content.audioStream->sampleBuffer +
								content.audioStream->sampleBufferOffset,
						content.audioStream->sampleBufferSize);

				frame->size += content.audioStream->sampleBufferSize;

				content.audioStream->sampleBufferSize   = 0;
				content.audioStream->sampleBufferOffset = 0;
			}

			if (frame->size == frame->capacity) {
				return 0;
			}
		}

		content.audioStream->_ffmpeg->codec->skip_frame = (AVDiscard)0;
		content.audioStream->sampleBufferTime = av_rescale(
				(pack->dts - content.audioStream->_ffmpeg->start_time) * 1000,
				content.audioStream->_ffmpeg->time_base.num,
				content.audioStream->_ffmpeg->time_base.den);

		if (content.audioStream->sampleBufferTime != AV_NOPTS_VALUE &&
				content.audioStream->sampleBufferTime <
						content.minimalTimestamp) {

			content.audioStream->_ffmpeg->codec->skip_frame = (AVDiscard)1;
		}

		while (size > 0) {
			int len = avcodec_decode_audio3(
					content.audioStream->_ffmpeg->codec,
					(int16_t*)content.audioStream->sampleBuffer,
					&audioSize, pack);

			if (len <= 0 || !audioSize) {
				cout << "SDL2ffmpeg::decodeAudioFrame ";
				cout << "error decoding audio frame" << endl;
				break;
			}

			data += len;
			size -= len;
		}

		if (!content.audioStream->_ffmpeg->codec->skip_frame) {
			if (!frame->size) {
				frame->pts = content.audioStream->sampleBufferTime;
			}

			int fs = frame->capacity - frame->size;

			if (fs) {
				if (fs < audioSize) {
					memcpy(
							frame->buffer + frame->size,
							content.audioStream->sampleBuffer,
							fs);

					content.audioStream->sampleBufferSize   = audioSize - fs;
					content.audioStream->sampleBufferOffset = fs;

					frame->size = frame->capacity;

				} else {
					memcpy(
							frame->buffer + frame->size,
							content.audioStream->sampleBuffer,
							audioSize);

					content.audioStream->sampleBufferSize   = 0;
					content.audioStream->sampleBufferOffset = 0;

					frame->size += audioSize;
				}

			} else {
				content.audioStream->sampleBufferSize   = audioSize;
				content.audioStream->sampleBufferOffset = 0;
			}
		}

		return 1;
	}

	int SDL2ffmpeg::decodeVideoFrame(AVPacket *pack, VideoFrame *frame) {
		int got_frame = 0;

		if (pack) {
			if (pack->dts == AV_NOPTS_VALUE) {
				frame->pts = content.videoStream->lastTimeStamp + av_rescale(
						1000 * pack->duration,
						content.videoStream->_ffmpeg->time_base.num,
						content.videoStream->_ffmpeg->time_base.den);

			} else {
				frame->pts = av_rescale(
						(pack->dts - content.videoStream->_ffmpeg->
								start_time) * 1000,
						content.videoStream->_ffmpeg->time_base.num,
						content.videoStream->_ffmpeg->time_base.den);
			}

			if (frame->pts != AV_NOPTS_VALUE &&
					frame->pts < content.minimalTimestamp) {
				content.videoStream->_ffmpeg->codec->skip_frame = (AVDiscard)1;

			} else {
				content.videoStream->_ffmpeg->codec->skip_frame = (AVDiscard)0;
			}

			avcodec_decode_video2(
					content.videoStream->_ffmpeg->codec,
					content.videoStream->decodeFrame,
					&got_frame,
					pack);

		} else {
			AVPacket temp;

			av_init_packet(&temp);
			temp.data = 0;
			temp.size = 0;

			temp.stream_index = content.videoStream->_ffmpeg->index;

			avcodec_decode_video2(
					content.videoStream->_ffmpeg->codec,
					content.videoStream->decodeFrame,
					&got_frame,
					&temp);
		}

		if (got_frame && !content.videoStream->_ffmpeg->codec->skip_frame) {
			if (frame->texture) {
				//void* pixels;
				uint8_t* pixels[3];
				int tpitch[3];

				SDL_LockTexture(
						frame->texture, NULL, (void**)&pixels, &tpitch[0]);

				if (content.videoStream->conversionContext == NULL) {
					content.videoStream->conversionContext = createContext(
							content.videoStream->_ffmpeg->codec->width,
							content.videoStream->_ffmpeg->codec->height,
							content.videoStream->_ffmpeg->codec->pix_fmt,
							frame->tempw,
							frame->temph,
							PIX_FMT_RGB24);
				}

				sws_scale(
						content.videoStream->conversionContext,
						(const uint8_t* const*)content.videoStream->decodeFrame->data,
						content.videoStream->decodeFrame->linesize,
						0,
						content.videoStream->_ffmpeg->codec->height,
						pixels,
						tpitch);

				SDL_UnlockTexture( frame->texture );
			}

			content.videoStream->lastTimeStamp = frame->pts;
			frame->ready = 1;
		}

		return frame->ready;
	}

	struct SwsContext* SDL2ffmpeg::createContext(
			int inWidth, int inHeight,
			enum PixelFormat inFormat,
			int outWidth, int outHeight,
			enum PixelFormat outFormat) {

		struct SwsContext *context;

		/* fill context with correct information */
		context = sws_getContext(
				inWidth, inHeight,
				inFormat,
				outWidth, outHeight,
				outFormat,
				SWS_BILINEAR,
				0, 0, 0);

		return context;
	}
}
}
}
}
}
}
