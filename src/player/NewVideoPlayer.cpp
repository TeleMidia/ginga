/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

//#include "ginga.h"
#include "NewVideoPlayer.h"

//#include "mb/Display.h"
//#include "mb/SDLWindow.h"

#define TRACE() g_debug ("%s",G_STRFUNC);


//#include "util/functions.h"
//using namespace ::ginga::util;

GINGA_PLAYER_BEGIN

NewVideoPlayer::NewVideoPlayer (const string &mrl) : Thread (), Player (mrl)
{
	//TRACE ();
  texture = NULL;  
  
  this->mutexInit ();
  this->condDisplayJobInit ();
  this->surface = new SDLSurface ();
  
  createPipeline ();
}

NewVideoPlayer::~NewVideoPlayer ()
{
  this->condDisplayJobClear ();
  this->mutexClear ();
}

void
NewVideoPlayer::createPipeline ()
{
  GstElement *scale;
  GstElement *sink;

  GstPad *pad;
 
  char *uri;
  
  gst_init (NULL, NULL);

  playbin = gst_element_factory_make ("playbin", NULL);
  g_assert_nonnull (playbin);
  
  uri = gst_filename_to_uri (mrl.c_str (), NULL);
  g_assert_nonnull (uri);

  g_object_set (G_OBJECT (playbin), "uri", uri, NULL);
  g_free (uri);

  bin = gst_bin_new (NULL);
  g_assert_nonnull (bin);

  filter = gst_element_factory_make ("capsfilter", NULL);
  g_assert_nonnull (filter);

  scale = gst_element_factory_make ("videoscale", NULL);
  g_assert_nonnull (scale);

  sink = gst_element_factory_make ("appsink", NULL);
  g_assert_nonnull (sink);

  g_assert (gst_bin_add (GST_BIN (bin), filter));
  g_assert (gst_bin_add (GST_BIN (bin), scale));
  g_assert (gst_bin_add (GST_BIN (bin), sink));

  g_assert (gst_element_link (filter, scale));
  g_assert (gst_element_link (scale, sink)); 

  pad = gst_element_get_static_pad (filter, "sink");
  gst_element_add_pad (bin, gst_ghost_pad_new ("sink", pad));

  g_object_set (G_OBJECT (playbin), "video-sink", bin, NULL);

  callbacks.eos = eosCB;
  callbacks.new_preroll = newPrerollCB;
  callbacks.new_sample = newSampleCB;
  gst_app_sink_set_callbacks (GST_APP_SINK (sink), &callbacks, this, NULL);

//  bus = gst_element_get_bus (playbin);
//  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
//                                    (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
//  gst_object_unref (playbin);
//  gst_object_unref (bin);
}

void 
NewVideoPlayer::eosCB (arg_unused (GstAppSink *appsink), arg_unused(gpointer data))
{
  g_print("eos NewVideoPlayer\n");
  
  NewVideoPlayer *player = (NewVideoPlayer *) data;

  player->eos ();
}

GstFlowReturn
NewVideoPlayer::newPrerollCB (arg_unused (GstAppSink *appsink), arg_unused(gpointer data))
{
  return GST_FLOW_OK;
}

GstFlowReturn
NewVideoPlayer::newSampleCB (arg_unused (GstAppSink *appsink), arg_unused(gpointer data))
{
  NewVideoPlayer *player = (NewVideoPlayer *) data;

  player->lock();
  
  if ( player->sample != NULL ){
    gst_sample_unref (player->sample);
    player->sample = NULL;
  }

  player->sample = gst_app_sink_pull_sample (appsink);
  g_assert_nonnull (player->sample);

  player->unlock ();

  return GST_FLOW_OK;
}
  
bool
NewVideoPlayer::displayJobCallbackWrapper (DisplayJob *job,
                                           SDL_Renderer *renderer,
                                           void *self)
{
  return ((NewVideoPlayer *) self)->displayJobCallback (job, renderer);
}

bool
NewVideoPlayer::displayJobCallback (arg_unused (DisplayJob *job),
                                    SDL_Renderer *renderer)
{
  
//  g_print ("%" G_GUINT64_FORMAT "\n" , GST_TIME_AS_MSECONDS (gst_clock_get_time ( gst_element_get_clock (playbin) )) );
  
//  if( gst_clock_get_time ( gst_element_get_clock (playbin) ) > 3474088000) //Apagar
//    return false;
//    stop ();
  
  GstVideoFrame v_frame;
  GstVideoInfo v_info;
  GstBuffer *buf;
  GstCaps *caps;
  guint8 *pixels;
  guint stride;
  
  SDLWindow *window;

  this->lock ();
  
  //g_assert_nonnull(surface);
  if ( sample != NULL ){

    window = surface->getParentWindow ();
    
    if (texture == NULL){
      texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_ARGB32,
                                SDL_TEXTUREACCESS_TARGET, 
                                surface->getParentWindow ()->getRect().w,
                                surface->getParentWindow ()->getRect().h);
      g_assert_nonnull (texture);
      
      this->condDisplayJobSignal ();
    }


    buf = gst_sample_get_buffer (sample);
    g_assert_nonnull (buf);

    caps = gst_sample_get_caps (sample);
    g_assert_nonnull (caps);

    g_assert (gst_video_info_from_caps(&v_info, caps));
    g_assert (gst_video_frame_map (&v_frame, &v_info, buf, GST_MAP_READ));

    pixels = (guint8 *) GST_VIDEO_FRAME_PLANE_DATA (&v_frame, 0);
    stride = GST_VIDEO_FRAME_PLANE_STRIDE (&v_frame, 0);
    g_assert (SDL_UpdateTexture(texture, NULL, pixels, stride) == 0);
  
    g_assert_nonnull (window);
    window->setTexture (texture);

    gst_video_frame_unmap (&v_frame);

    gst_sample_unref (sample);
    sample = NULL;
  }
  
  this->unlock ();

  //if (GST_ELEMENT_CAST(playbin)->current_state == GST_STATE_PAUSED)
  if (this->status == SLEEPING)
  {
    g_print("status: SLEEPING; return false;\n");
    //SDL_DestroyTexture (texture);
    //this->condDisplayJobSignal ();

    return false; //Remove job
  }

  return true; //Keep job
}

SDLSurface*
NewVideoPlayer::getSurface ()
{
	return this->surface;  
}

void
NewVideoPlayer::finished ()
{
	TRACE ();
}

double
NewVideoPlayer::getEndTime ()
{
	TRACE ();
  return 0;
}

void
NewVideoPlayer::initializeAudio (arg_unused (int numArgs), arg_unused (char *args[]))
{
	TRACE ();
}

void
NewVideoPlayer::releaseAudio ()
{
	g_debug ("%s", G_STRLOC);
}

void
NewVideoPlayer::getOriginalResolution (arg_unused(int *width), arg_unused(int *height))
{
	g_debug ("%s", G_STRLOC);
}

double
NewVideoPlayer::getTotalMediaTime ()
{
	g_debug ("%s", G_STRLOC);
  return 0;
}

int64_t
NewVideoPlayer::getVPts ()
{
	g_debug ("%s", G_STRLOC);
  return 0;
}

void
NewVideoPlayer::timeShift (arg_unused(const string &direction))
{
	TRACE ();
}

guint32
NewVideoPlayer::getMediaTime ()
{
  GstClock *clock = gst_element_get_clock (playbin);
  guint32 time =  GST_TIME_AS_MSECONDS (gst_clock_get_time ( clock ));
  
  gst_object_unref (clock);

  return time;
}

void
NewVideoPlayer::setMediaTime (arg_unused(guint32 pos))
{
	TRACE ();
}

void
NewVideoPlayer::setStopTime (arg_unused(double pos))
{
	TRACE ();
}

double
NewVideoPlayer::getStopTime ()
{
	TRACE ();
  return 0;
}

void
NewVideoPlayer::setScope (arg_unused(const string &scope), arg_unused(short type), arg_unused(double begin), arg_unused(double end),arg_unused(double outTransDur))
{
	TRACE ();
}

bool
NewVideoPlayer::play ()
{
  Player::play ();

  ret = gst_element_set_state (playbin, GST_STATE_PLAYING);
  g_assert (ret != GST_STATE_CHANGE_FAILURE);
  
  GstStateChangeReturn retWait = gst_element_get_state (playbin, NULL, NULL, GST_CLOCK_TIME_NONE);

  if ( retWait == GST_STATE_CHANGE_SUCCESS ){
    Ginga_Display->addJob(displayJobCallbackWrapper, this);
    this->condDisplayJobWait (); 
    Thread::startThread ();
    return true;
  }

  g_print ("failed to play the file\n");
  return false;
}

void
NewVideoPlayer::pause ()
{
  Player::pause ();

  ret = gst_element_set_state (playbin, GST_STATE_PAUSED);
  g_assert (ret != GST_STATE_CHANGE_FAILURE);

  GstStateChangeReturn retWait = gst_element_get_state (playbin, NULL, NULL, GST_CLOCK_TIME_NONE);

  if ( retWait != GST_STATE_CHANGE_SUCCESS )
  {
    g_print ("failed to pause the file\n");
  }
}

void
NewVideoPlayer::stop ()
{ //precisa estar entre locks
  ret = gst_element_set_state (playbin, GST_STATE_NULL);
  g_assert (ret != GST_STATE_CHANGE_FAILURE);

  GstStateChangeReturn retWait = gst_element_get_state (playbin, NULL, NULL, GST_CLOCK_TIME_NONE);

  if ( retWait != GST_STATE_CHANGE_SUCCESS )
  {
    g_print ("failed to stop the file\n");
  } 

  Player::stop ();

  gst_object_unref (playbin);
  gst_object_unref (bin);
}

void
NewVideoPlayer::resume ()
{
  Player::resume ();

  if (GST_ELEMENT_CAST(playbin)->current_state == GST_STATE_PAUSED)
  {
    this->play ();
  }
}

void
NewVideoPlayer::eos ()
{
  Player::stop ();

  gst_object_unref (playbin);
  gst_object_unref (bin);
}

string
NewVideoPlayer::getPropertyValue (arg_unused(const string &name))
{
	TRACE ();
  return NULL;
}

void
NewVideoPlayer::setPropertyValue (arg_unused(const string &name), arg_unused(const string &value))
{
	TRACE ();
}

void
NewVideoPlayer::addListener (IPlayerListener *listener)
{
  Player::addListener (listener);
}

//void
//NewVideoPlayer::release ()
//{
//	TRACE ();
//}

string
NewVideoPlayer::getMrl ()
{
  return mrl;
}

bool
NewVideoPlayer::isPlaying ()
{
  if (GST_ELEMENT_CAST(playbin)->current_state == GST_STATE_PLAYING)
  {
    return true;
  }
	return false;
}

bool
NewVideoPlayer::setOutWindow (arg_unused(SDLWindow* windowId))
{
  GstCaps *caps;
  GstStructure *st;

  if (surface != 0 && surface->getParentWindow () == 0){
    surface->setParentWindow (windowId);

    st = gst_structure_new_empty ("video/x-raw");
    gst_structure_set (st, "format", G_TYPE_STRING, "ARGB",
                      "width", G_TYPE_INT, surface->getParentWindow ()->getRect().w,
                      "height", G_TYPE_INT, surface->getParentWindow ()->getRect().h, 
                      NULL);

    caps = gst_caps_new_full (st, NULL);
    g_assert_nonnull (caps);

    g_object_set (filter, "caps", caps, NULL);
    gst_caps_unref (caps);
  }

	return true;
}

/*void
NewVideoPlayer::setAVPid (arg_unused(int aPid), arg_unused(int vPid))
{
	TRACE ();
}*/

bool
NewVideoPlayer::isRunning ()
{
  return this->isPlaying ();
}

void
NewVideoPlayer::run ()
{
}

GINGA_PLAYER_END
