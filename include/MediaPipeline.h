#ifndef MEDIAPIPELINE_H
#define MEDIAPIPELINE_H


#include <gst/gst.h>
#include <gst/video/video.h>

#include <cairo.h>
#include <cairo-gobject.h>

#include <pango/pangocairo.h>

#include <glib.h>

#include <stdint.h>

#include "../include/LyricManager.h"

typedef struct
{
  gboolean valid;
  GstVideoInfo vinfo;
} CairoOverlayState;


class MediaPipeline
{
private:
      GMainLoop *loop;
      GstElement *pipeline;
      GstBus *bus;
      CairoOverlayState *overlay_state;

      GstElement *audio_decoder;

       GstElement *mp3_src;
       uint64_t mp3_init_pos;

       GstElement *vid_src;


       LyricManager *lyric_manager;

    public:
        MediaPipeline(int argc, char **argv);
        ~MediaPipeline();

        void setSongFile(char* fname);
        void setVideoFile(char* fname);


        void setup_gst_pipeline();



        void setSongAndLyricsFiles(char* sname, char *lname);
        LyricManager *getLyricManager();
        void setLyricManager(LyricManager* lm);

        void pause();
        void stop();
        void play();
        void rewind(float percent);

    protected:
    private:
};

#endif // MEDIAPIPELINE_H
