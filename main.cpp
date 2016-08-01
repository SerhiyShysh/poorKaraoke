

/*
 * Example showing usage of the cairooverlay element
 */


//
// gcc cairo_gst2.c `pkg-config --cflags --libs glib-2.0`  `pkg-config --cflags --libs pangocairo` `pkg-config --cflags --libs pango`  `pkg-config --cflags --libs gstreamer-1.0` `pkg-config --cflags --libs cairo` -o cairo_test
//




#include <gst/gst.h>
#include <gst/video/video.h>

#include <cairo.h>
#include <cairo-gobject.h>

#include <pango/pangocairo.h>

#include <glib.h>

#include <stdio.h>
#include <stdlib.h>

#include "MediaPipeline.h"




int
main (int argc, char **argv)
{

int c;
MediaPipeline a(argc, argv);

c = getchar();

  return 0;
}
