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

/* no AV sync correction is done if below the AV sync threshold */
#ifdef AV_SYNC_THRESHOLD
#undef AV_SYNC_THRESHOLD
#endif
#define AV_SYNC_THRESHOLD 0.01

/* no AV correction is done if too big error */
#ifdef AV_NOSYNC_THRESHOLD
#undef AV_NOSYNC_THRESHOLD
#endif
#define AV_NOSYNC_THRESHOLD 10.0

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	bool SDL2ffmpeg::initialized = false;

	SDL2ffmpeg::SDL2ffmpeg(const char* mrl) {
		memset(&content, 0, sizeof(Content));
		content.stMutex = SDL_CreateMutex();

		open(mrl);
	}

	SDL2ffmpeg::~SDL2ffmpeg() {
		flush();

		if (isVideoValid() &&content.videoSt->convCtx != NULL) {
			sws_freeContext(content.videoSt->convCtx);
			content.videoSt->convCtx = NULL;
		}

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

		SDL_DestroyMutex(content.stMutex);
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
						cout << "could not find video decoder" << endl;

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
		SDL_LockMutex(content.stMutex);

		if (!content.audioSt || !bytes) {
			SDL_UnlockMutex(content.stMutex);
			return NULL;
		}

		/* allocate new frame */
		AudioFrame *frame = new AudioFrame;
		memset(frame, 0, sizeof(AudioFrame));

		SDL_UnlockMutex(content.stMutex);

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
		SDL_LockMutex(content.stMutex);

		if (frame == NULL || !isVideoValid()) {
			SDL_UnlockMutex(content.stMutex);
			return 0;
		}

		SDL_LockMutex(content.videoSt->mutex);

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
			pack->next = content.videoSt->buffer;
			content.videoSt->buffer = pack;

		} else if (!frame->ready && frame->last) {
			decodeVideoFrame(NULL, frame);
		}

		SDL_UnlockMutex(content.videoSt->mutex);
		SDL_UnlockMutex(content.stMutex);

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

		SDL_LockMutex(content.stMutex);

		if (!content.audioStreams || audioID >= (int)content.audioStreams) {
			SDL_UnlockMutex(content.stMutex);

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
			content.audioSt = 0;

		} else {
			content.audioSt = content.as;
			for (i = 0; i < audioID && content.audioSt; i++) {
				content.audioSt = content.audioSt->next;
			}

			content.audioSt->_ffmpeg->discard = AVDISCARD_DEFAULT;
		}

		SDL_UnlockMutex(content.stMutex);

		return 0;
	}

	void SDL2ffmpeg::refreshVideo(VideoFrame* videoFrame) {
		if (!videoFrame->ready) {
			if (content.frame_last_dropped_pts != AV_NOPTS_VALUE &&
					content.frame_last_dropped_pts > content.frame_last_pts) {

				updateVideoPts(
						content.frame_last_dropped_pts,
						content.frame_last_dropped_pos);

				content.frame_last_dropped_pts = AV_NOPTS_VALUE;
			}

		} else if (videoFrame->pts <= getClock()) {
			double time, last_duration, duration, delay;

			/* compute nominal last_duration */
			last_duration = videoFrame->pts - content.frame_last_pts;
			if (last_duration > 0 && last_duration < 10.0) {
				/* if duration of the last frame was sane,
				 * update last_duration in video state
				 */
				content.frame_last_duration = last_duration;
			}

			delay = computeTargetDelay(content.frame_last_duration);

			time = av_gettime()/1000000.0;
			if (time < content.frame_timer + delay) {
				return;
			}

			if (delay > 0) {
				content.frame_timer += delay *
						FFMAX(1, floor((time-content.frame_timer) / delay));
			}

			updateVideoPts(videoFrame->pts, videoFrame->pos);
			videoFrame->ready = 0;
		}
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

		/* when changing audio/video stream, stMutex should be locked */
		SDL_LockMutex(content.stMutex);

		if (videoID >= (int)content.videoStreams) {
			SDL_UnlockMutex(content.stMutex);

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
			content.videoSt = 0;

		} else {
			content.videoSt = content.vs;

			for (i = 0; i < videoID && content.videoSt; i++) {
				content.videoSt = content.videoSt->next;
			}

			content.videoSt->_ffmpeg->discard = AVDISCARD_DEFAULT;
		}

		SDL_UnlockMutex(content.stMutex);

		return 0;
	}

	void SDL2ffmpeg::play() {
		av_read_play(content._ffmpeg);
		SDL_PauseAudio(0);
	}

	void SDL2ffmpeg::pause() {
		if (!content.paused) {
			content.read_pause_return = av_read_pause(content._ffmpeg);
			content.paused = true;
		}
	}

	void SDL2ffmpeg::resume() {
		if (content.paused) {
			content.frame_timer += (av_gettime() / 1000000.0 +
					content.video_cur_pts_drift - content.video_cur_pts);

			if (content.read_pause_return != AVERROR(ENOSYS)) {
				content.video_cur_pts = (content.video_cur_pts_drift +
						av_gettime() / 1000000.0);
			}

			content.video_cur_pts_drift = (content.video_cur_pts -
					av_gettime() / 1000000.0);

			av_read_play(content._ffmpeg);
		}

		content.paused = false;
	}

	int SDL2ffmpeg::seek(uint64_t timestamp ) {
		if (getDuration() < timestamp) {
			cout << "SDL2ffmpeg::seek ";
			cout << "can not seek past end of file" << endl;
			return -1;
		}

		uint64_t seekPos = timestamp * (AV_TIME_BASE / 1000);

		av_seek_frame(content._ffmpeg, -1, seekPos, AVSEEK_FLAG_BACKWARD);

		content.minTimestamp = timestamp;

		flush();

		return 0;
	}

	int SDL2ffmpeg::seekRelative(int64_t timestamp ) {
		return seek(getPosition() + timestamp);
	}

	int SDL2ffmpeg::flush() {
		SDL_LockMutex(content.stMutex);

		if (content.audioSt) {
			SDL_LockMutex(content.audioSt->mutex);

			Packet* pack = content.audioSt->buffer;

			while (pack) {
				Packet* old = pack;

				pack = pack->next;

				av_free(old->data);
				delete old;
			}

			content.audioSt->buffer = 0;

			if (content.audioSt->_ffmpeg) {
				avcodec_flush_buffers(content.audioSt->_ffmpeg->codec);
			}

			SDL_UnlockMutex(content.audioSt->mutex);
		}

		if (content.videoSt) {
			SDL_LockMutex(content.videoSt->mutex);

			Packet* pack = content.videoSt->buffer;

			while (pack) {
				Packet* old = pack;

				pack = pack->next;

				av_free(old->data);
				delete old;
			}

			content.videoSt->buffer = 0;

			if (content.videoSt->_ffmpeg) {
				avcodec_flush_buffers(content.videoSt->_ffmpeg->codec);
			}

			SDL_UnlockMutex(content.videoSt->mutex);
		}

		SDL_UnlockMutex(content.stMutex);

		return 0;
	}

	int SDL2ffmpeg::getAudioFrame(AudioFrame *frame) {
		if (!frame ) {
			return -1;
		}

		SDL_LockMutex(content.stMutex);

		if (!content.audioSt) {
			SDL_UnlockMutex(content.stMutex);

			cout << "SDL2ffmpeg::getAudioFrame ";
			cout << "no valid audio stream selected" << endl;
			return 0;
		}

		SDL_LockMutex(content.audioSt->mutex);

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
			pack->next = content.audioSt->buffer;
			content.audioSt->buffer = pack;
		}

		SDL_UnlockMutex(content.audioSt->mutex);
		SDL_UnlockMutex(content.stMutex);

		return (frame->size == frame->capacity);
	}

	int64_t SDL2ffmpeg::getPosition() {
		SDL_LockMutex(content.stMutex);

		int64_t pos = 0;

		if (content.audioSt) {
			pos = content.audioSt->lastTimeStamp;
		}

		if (content.videoSt && content.videoSt->lastTimeStamp > pos) {
			pos = content.videoSt->lastTimeStamp;
		}

		SDL_UnlockMutex(content.stMutex);

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

		SDL_LockMutex(content.stMutex);

		if (content.audioSt) {
			spec.format   = AUDIO_S16SYS;
			spec.samples  = samples;
			spec.userdata = &content;
			spec.callback = callback;
			spec.freq     = content.audioSt->_ffmpeg->codec->sample_rate;
			spec.channels = (uint8_t)
					content.audioSt->_ffmpeg->codec->channels;

		} else {
			cout << "SDL2ffmpeg::getAudioSpec ";
			cout << "no valid audio stream selected" << endl;
		}

		SDL_UnlockMutex(content.stMutex);

		return spec;
	}

	double SDL2ffmpeg::getAudioClock() {
		if (content.paused) {
			return content.audio_cur_pts;

		} else {
			return content.audio_cur_pts_drift + av_gettime() / 1000000.0;
		}
	}

	double SDL2ffmpeg::getVideoClock() {
		if (content.paused) {
			return content.video_cur_pts;

		} else {
			return content.video_cur_pts_drift + av_gettime() / 1000000.0;
		}
	}

	double SDL2ffmpeg::getClock() {
		double val = -1;

		if (isAudioValid()) {
			val = getAudioClock();

		} else if (isVideoValid()) {
			val = getVideoClock();
		}

		return val;
	}

	double SDL2ffmpeg::computeTargetDelay(double delay) {
	    double sync_threshold, diff;

		if (isAudioValid()) {
			diff = getVideoClock() - getClock();

			sync_threshold = FFMAX(AV_SYNC_THRESHOLD, delay);
			if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
				if (diff <= -sync_threshold) {
					delay = 0;
				} else if (diff >= sync_threshold) {
					delay = 2 * delay;
				}
			}
		}

	    return delay;
	}

	void SDL2ffmpeg::updateVideoPts(double pts, int64_t pos) {
	    double time                 = av_gettime() / 1000000.0;
	    content.video_cur_pts       = pts;
	    content.video_cur_pts_drift = content.video_cur_pts - time;
	    content.video_cur_pos       = pos;
	    content.frame_last_pts      = pts;
	}

	uint64_t SDL2ffmpeg::getDuration() {
		return content._ffmpeg->duration / ( AV_TIME_BASE / 1000 );
	}

	uint64_t SDL2ffmpeg::getAudioDuration() {
		SDL_LockMutex(content.stMutex);

		uint64_t duration = 0;

		if (!content.audioSt) {
		   duration = av_rescale(
				   1000 * content.audioSt->_ffmpeg->duration,
				   content.audioSt->_ffmpeg->time_base.num,
				   content.audioSt->_ffmpeg->time_base.den);

		} else {
			cout << "SDL2ffmpeg::getAudioDuration ";
			cout << "no valid audio stream selected" << endl;
		}

		SDL_UnlockMutex(content.stMutex);

		return duration;
	}

	uint64_t SDL2ffmpeg::getVideoDuration() {
		SDL_LockMutex(content.stMutex);

		uint64_t duration = 0;

		if (content.videoSt) {
			duration = av_rescale(
					1000 * content.videoSt->_ffmpeg->duration,
					content.videoSt->_ffmpeg->time_base.num,
					content.videoSt->_ffmpeg->time_base.den);

		} else {
			cout << "SDL2ffmpeg::getVideoDuration ";
			cout << "no valid video stream selected" << endl;
		}

		SDL_UnlockMutex(content.stMutex);

		return duration;
	}

	int SDL2ffmpeg::getVideoSize(int *w, int *h ) {
		if (!w || !h ) {
			return -1;
		}

		SDL_LockMutex(content.stMutex);

		if (content.videoSt) {
			*w = content.videoSt->_ffmpeg->codec->width;
			*h = content.videoSt->_ffmpeg->codec->height;

			SDL_UnlockMutex(content.stMutex);

			return 0;

		} else {
			cout << "no valid video stream selected" << endl;
		}

		*w = 0;
		*h = 0;

		SDL_UnlockMutex(content.stMutex);

		return -1;
	}

	bool SDL2ffmpeg::isAudioValid() {
		return (content.audioSt != NULL);
	}

	bool SDL2ffmpeg::isVideoValid() {
		return (content.videoSt != NULL);
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
			if (content.audioSt &&
					pack->stream_index == content.audioSt->id) {

				Packet* temp = new Packet;

				temp->data = pack;
				temp->next = 0;

				Packet** p = &content.audioSt->buffer;

				while (*p) {
					p = &(*p)->next;
				}

				*p = temp;

			} else if (content.videoSt &&
					pack->stream_index == content.videoSt->id) {

				Packet* temp = new Packet;

				temp->data = pack;
				temp->next = 0;

//				SDL_LockMutex( content.videoSt->mutex );

				Packet** p = &content.videoSt->buffer;

				while (*p) {
					p = &(*p)->next;
				}

				*p = temp;

//				SDL_UnlockMutex( content.videoSt->mutex );

			} else {
				av_free_packet(pack);
			}
		}

		return 0;
	}

	Packet* SDL2ffmpeg::getAudioPacket() {
		if (!content.audioSt) {
			return 0;
		}

		Packet* pack = 0;

		if (content.audioSt->buffer) {
			pack = content.audioSt->buffer;
			content.audioSt->buffer = pack->next;
		}

		return pack;
	}

	Packet* SDL2ffmpeg::getVideoPacket() {
		if (!content.videoSt) {
			return 0;
		}

		Packet* pack = 0;

		if (content.videoSt->buffer) {
			pack = content.videoSt->buffer;
			content.videoSt->buffer = pack->next;
		}

		return pack;
	}

	int SDL2ffmpeg::decodeAudioFrame(AVPacket *pack, AudioFrame *frame) {
		uint8_t *data = pack->data;
		int size      = pack->size;
		int audioSize = AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(int16_t);
		int len       = 0;

		if (content.audioSt->sampleBufferSize) {
			if (!frame->size) {
				frame->pts = content.audioSt->sampleBufferTime;
			}

			int fs = frame->capacity - frame->size;

			if (fs < content.audioSt->sampleBufferSize) {
				memcpy(
						frame->buffer + frame->size,
						content.audioSt->sampleBuffer +
								content.audioSt->sampleBufferOffset,
						fs);

				content.audioSt->sampleBufferSize   -= fs;
				content.audioSt->sampleBufferOffset += fs;

				frame->size = frame->capacity;

			} else {
				memcpy(
						frame->buffer + frame->size,
						content.audioSt->sampleBuffer +
								content.audioSt->sampleBufferOffset,
						content.audioSt->sampleBufferSize);

				frame->size += content.audioSt->sampleBufferSize;

				content.audioSt->sampleBufferSize   = 0;
				content.audioSt->sampleBufferOffset = 0;
			}

			if (frame->size == frame->capacity) {
				return 0;
			}
		}

		content.audioSt->_ffmpeg->codec->skip_frame = (AVDiscard)0;
		content.audioSt->sampleBufferTime = av_rescale(
				(pack->dts - content.audioSt->_ffmpeg->start_time) * 1000,
				content.audioSt->_ffmpeg->time_base.num,
				content.audioSt->_ffmpeg->time_base.den);

		if (content.audioSt->sampleBufferTime != AV_NOPTS_VALUE &&
				content.audioSt->sampleBufferTime <
						content.minTimestamp) {

			content.audioSt->_ffmpeg->codec->skip_frame = (AVDiscard)1;
		}

		while (size > 0) {
			len = avcodec_decode_audio3(
					content.audioSt->_ffmpeg->codec,
					(int16_t*)content.audioSt->sampleBuffer,
					&audioSize, pack);

			if (len <= 0 || !audioSize) {
				cout << "SDL2ffmpeg::decodeAudioFrame ";
				cout << "error decoding audio frame" << endl;
				break;
			}

			data += len;
			size -= len;
		}

		if (!content.audioSt->_ffmpeg->codec->skip_frame) {
			if (!frame->size) {
				frame->pts = content.audioSt->sampleBufferTime;
			}

			int fs = frame->capacity - frame->size;

			if (fs) {
				if (fs < audioSize) {
					memcpy(
							frame->buffer + frame->size,
							content.audioSt->sampleBuffer,
							fs);

					content.audioSt->sampleBufferSize   = audioSize - fs;
					content.audioSt->sampleBufferOffset = fs;

					frame->size = frame->capacity;

				} else {
					memcpy(
							frame->buffer + frame->size,
							content.audioSt->sampleBuffer,
							audioSize);

					content.audioSt->sampleBufferSize   = 0;
					content.audioSt->sampleBufferOffset = 0;

					frame->size += audioSize;
				}

			} else {
				content.audioSt->sampleBufferSize   = audioSize;
				content.audioSt->sampleBufferOffset = 0;
			}
		}

		return 1;
	}

	int SDL2ffmpeg::decodeVideoFrame(AVPacket *pack, VideoFrame *frame) {
		int got_frame = 0;

		if (pack) {
			if (pack->dts == AV_NOPTS_VALUE) {
				frame->pts = content.videoSt->lastTimeStamp + av_rescale(
						1000 * pack->duration,
						content.videoSt->_ffmpeg->time_base.num,
						content.videoSt->_ffmpeg->time_base.den);

			} else {
				frame->pts = av_rescale(
						(pack->dts - content.videoSt->_ffmpeg->
								start_time) * 1000,
						content.videoSt->_ffmpeg->time_base.num,
						content.videoSt->_ffmpeg->time_base.den);
			}

			if (frame->pts != AV_NOPTS_VALUE &&
					frame->pts < content.minTimestamp) {

				content.videoSt->_ffmpeg->codec->skip_frame = (AVDiscard)1;

			} else {
				content.videoSt->_ffmpeg->codec->skip_frame = (AVDiscard)0;
			}

			avcodec_decode_video2(
					content.videoSt->_ffmpeg->codec,
					content.videoSt->decodeFrame,
					&got_frame,
					pack);

		} else {
			AVPacket temp;

			av_init_packet(&temp);
			temp.data = 0;
			temp.size = 0;

			temp.stream_index = content.videoSt->_ffmpeg->index;

			avcodec_decode_video2(
					content.videoSt->_ffmpeg->codec,
					content.videoSt->decodeFrame,
					&got_frame,
					&temp);
		}

		if (got_frame && !content.videoSt->_ffmpeg->codec->skip_frame) {

			int64_t pts_int = *(int64_t*)av_opt_ptr(
					avcodec_get_frame_class(),
					content.videoSt->decodeFrame,
					"best_effort_timestamp");

			frame->pos = pts_int * av_q2d(content.videoSt->_ffmpeg->time_base);

			if (frame->texture) {
				uint8_t* pixels[3];
				int tpitch[3];

				SDL_LockTexture(
						frame->texture, NULL, (void**)&pixels, &tpitch[0]);

				if (content.videoSt->convCtx == NULL) {
					content.videoSt->convCtx = createContext(
							content.videoSt->_ffmpeg->codec->width,
							content.videoSt->_ffmpeg->codec->height,
							content.videoSt->_ffmpeg->codec->pix_fmt,
							frame->tempw,
							frame->temph,
							PIX_FMT_RGB24);
				}

				sws_scale(
						content.videoSt->convCtx,
						(const uint8_t* const*)(
								content.videoSt->decodeFrame->data),

						content.videoSt->decodeFrame->linesize,
						0,
						content.videoSt->_ffmpeg->codec->height,
						pixels,
						tpitch);

				SDL_UnlockTexture(frame->texture);
			}

			content.videoSt->lastTimeStamp = frame->pts;
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
