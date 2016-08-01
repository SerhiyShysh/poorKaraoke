#include "../include/MediaPipeline.h"

//#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>


//
//#include "TextLine.h"

#include "../include/LyricManager.h"


GstElement *global_pipeline;
GstElement *global_audio_pipeline;

//
// settings
//

//#define KARAOKE2FILE


char *back_video_file = "/home/gray/Videos/poi/poi_4_1.avi";
//char *back_video_file = "/home/gray/Videos/voiny_sveta.mp4";
char *audio_file      = "/home/gray/karaoke_playground/samaya.ogg";
char *lyrics_file     = "/home/gray/karaoke_playground/OnGst/samaya.txt";
char *out_video_file  = "asdf.mp4";

bool tmp_bool = false;
//
//
//


//LyricManager LM;
MediaPipeline *media_pipeline;

static gboolean
on_message (GstBus * bus, GstMessage * message, gpointer user_data)
{
  GMainLoop *loop = (GMainLoop *) user_data;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err = NULL;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_critical ("Got ERROR: %s (%s)", err->message, GST_STR_NULL (debug));
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_WARNING:{
      GError *err = NULL;
      gchar *debug;

      gst_message_parse_warning (message, &err, &debug);
      g_warning ("Got WARNING: %s (%s)", err->message, GST_STR_NULL (debug));
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }

  return TRUE;
}

/* Store the information from the caps that we are interested in. */
static void
prepare_overlay (GstElement * overlay, GstCaps * caps, gpointer user_data)
{
  CairoOverlayState *state = (CairoOverlayState *) user_data;

  state->valid = 1; //gst_video_info_from_caps (&state->vinfo, caps);
}



/* Draw the overlay.
 * This function draws a cute "beating" heart. */
static void
draw_overlay (GstElement * overlay, cairo_t * cr, guint64 timestamp,
    guint64 duration, gpointer user_data)
{
//cairo_scale (cr, 1920, 1080);
cairo_translate(cr, 600, 400); //text center

gint64 pos;

gst_element_query_position (global_audio_pipeline, GST_FORMAT_TIME, &pos);
//gst_element_query_position (global_audio_pipeline, GST_FORMAT_TIME, &pos);

/*
if(pos/1000000 > 25000 && tmp_bool == false)
{
    media_pipeline->setSongFile("/home/gray/karaoke_playground/maxim.ogg");
    tmp_bool = true;
}
*/

//LM.drawByTimeTwoLine(cr, pos/1000000);
media_pipeline->getLyricManager()->drawByTimeTwoLine(cr, pos/1000000);

//LM.drawByTime(cr, pos/1000000);


  CairoOverlayState *s = (CairoOverlayState *) user_data;
//  double scale;
  int width, height;

 // if (!s->valid)
 //   return;

  //width = GST_VIDEO_INFO_WIDTH (&s->vinfo);
  //height = GST_VIDEO_INFO_HEIGHT (&s->vinfo);



}

void MediaPipeline::setup_gst_pipeline ()
{

    media_pipeline = this;

  //GstElement *pipeline;
  GstElement *cairo_overlay;
  GstElement *source, *adaptor1, *adaptor2, *sink;


#ifdef KARAOKE2FILE
  global_pipeline = pipeline = gst_parse_launch (  "filesrc name=vid_src ! \
                                                    decodebin ! \
                                                    aspectratiocrop aspect-ratio=160/67 ! \
                                                    videoscale ! \
                                                    video/x-raw, width=1280,height=536, pixel-aspect-ratio=1/1! \
                                                    videoscale add-borders=1 ! \
                                                    videoconvert ! \
                                                    cairooverlay name=cro  !  \
                                                    videoconvert  ! x264enc !queue ! mux. \
                                                    filesrc name=mp3_src ! \
                                                    decodebin ! audioconvert ! voaacenc ! queue ! mux. \
                                                    mp4mux name=mux ! filesink name=out_file", NULL);

    GstElement *out_file = gst_bin_get_by_name(GST_BIN(pipeline), "out_file");
    g_object_set (G_OBJECT(out_file), "location", out_video_file, NULL);

#else
//

global_pipeline = pipeline = gst_parse_launch (  "filesrc name=vid_src ! \
                                                    decodebin ! \
                                                    aspectratiocrop aspect-ratio=120/52 ! \
                                                    videoscale ! \
                                                    video/x-raw, width=1200,height=520, pixel-aspect-ratio=1/1! \
                                                    videoscale add-borders=1 ! \
                                                    videoconvert ! \
                                                    cairooverlay name=cro  !  \
                                                    eglglessink", NULL);




/*
  global_pipeline = pipeline = gst_parse_launch (  "filesrc name=vid_src ! \
                                                    decodebin ! \
                                                    aspectratiocrop aspect-ratio=120/52 ! \
                                                    videoscale ! \
                                                    video/x-raw, width=1200,height=520, pixel-aspect-ratio=1/1! \
                                                    videoscale add-borders=1 ! \
                                                    videoconvert ! \
                                                    cairooverlay name=cro  !  \
                                                    eglglessink \
                                                    filesrc name=mp3_src ! \
                                                    decodebin name=audio_decoder ! \
                                                    pulsesink", NULL);
                                                 //
*/
#endif // KARAOKE2FILE

    //audio_decoder = global_audio_pipeline = gst_bin_get_by_name(GST_BIN(pipeline), "audio_decoder");

     vid_src = gst_bin_get_by_name(GST_BIN(pipeline), "vid_src");
     g_object_set (G_OBJECT(vid_src), "location", back_video_file, NULL);


     //mp3_src = gst_bin_get_by_name(GST_BIN(audio_decoder), "mp3_src");
     //g_object_set (G_OBJECT(mp3_src), "location", audio_file, NULL);

    setSongFile(audio_file);

     cairo_overlay = gst_bin_get_by_name(GST_BIN(pipeline), "cro");


  /* If failing, the element could not be created */
  g_assert (cairo_overlay);

  /* Hook up the neccesary signals for cairooverlay */
  g_signal_connect (cairo_overlay, "draw",
      G_CALLBACK (draw_overlay), overlay_state);
  g_signal_connect (cairo_overlay, "caps-changed",
      G_CALLBACK (prepare_overlay), overlay_state);


  //return pipeline;
}




MediaPipeline::MediaPipeline(int argc, char **argv)
{


    //LM.loadLyricFile(lyrics_file);
    //LM.prepareTwoLinesLayout();

    lyric_manager = new LyricManager();
    lyric_manager->loadLyricFile(lyrics_file);
    lyric_manager->prepareTwoLinesLayout();


    audio_decoder = NULL;

  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);

  /* allocate on heap for pedagogical reasons, makes code easier to transfer */
  overlay_state = g_new0 (CairoOverlayState, 1);

  //pipeline = setup_gst_pipeline ();

    setup_gst_pipeline();


  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message", G_CALLBACK (on_message), loop);
  gst_object_unref (GST_OBJECT (bus));

  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  //gst_element_set_state (audio_decoder, GST_STATE_PLAYING);



  //g_main_loop_run (loop);




}

MediaPipeline::~MediaPipeline()
{
    //dtor
}


void MediaPipeline::setSongFile(char* fname)
{
    if(audio_decoder)
    {
        gst_element_set_state (audio_decoder, GST_STATE_NULL);
    }
    global_audio_pipeline = audio_decoder = gst_parse_launch (  "filesrc name=mp3_src ! \
                                                    decodebin name=audio_decoder ! \
                                                    pulsesink", NULL);

    mp3_src = gst_bin_get_by_name(GST_BIN(audio_decoder), "mp3_src");
    g_object_set (G_OBJECT(mp3_src), "location", fname, NULL);

    gst_element_set_state (audio_decoder, GST_STATE_PLAYING);

    gint64 pos;
    gst_element_query_position (pipeline, GST_FORMAT_TIME, &pos);

    mp3_init_pos = pos / 1000000;

    //gst_element_set_state (pipeline, GST_STATE_PLAYING);
}


void MediaPipeline::setVideoFile(char* fname)
{
    g_object_set (G_OBJECT(vid_src), "location", fname, NULL);
}




void MediaPipeline::setSongAndLyricsFiles(char* sname, char *lname)
{

}


LyricManager* MediaPipeline::getLyricManager()
{
    return lyric_manager;
}



void MediaPipeline::setLyricManager(LyricManager* lm)
{
    delete lyric_manager;
    lyric_manager = lm;
}


void MediaPipeline::pause()
{
    if(audio_decoder)
    {
        gst_element_set_state (audio_decoder, GST_STATE_PAUSED);
    }
}

void MediaPipeline::stop()
{
    rewind(0);

    if(audio_decoder)
    {
        gst_element_set_state (audio_decoder, GST_STATE_PAUSED);
    }
}

void MediaPipeline::play()
{
    if(audio_decoder)
    {
        gst_element_set_state (audio_decoder, GST_STATE_PAUSED);
    }
}

void MediaPipeline::rewind(float time_nsec)
{
    if (!gst_element_seek (audio_decoder, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                             GST_SEEK_TYPE_SET, time_nsec,
                             GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
        g_print ("Seek failed!\n");
      }
}
