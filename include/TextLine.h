#ifndef TEXTLINE_H
#define TEXTLINE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>


#include <cairo.h>
#include <cairo-gobject.h>
#include <pango/pangocairo.h>


typedef struct _char_time_range{
    uint16_t index;
    uint64_t start_time;
    uint64_t end_time;
}  char_time_range;

class TextLine{

public:

    char text[256]; // contains text

    bool show_progress_bar;

    uint64_t start_time;
    uint64_t end_time;

    //hack(((((
    float old_part;
    int old_index;

    int curr_char_idx;

    float alpha;
    float progress;

    float border_width;

    PangoFontDescription *font_descr;
    std::vector<char_time_range> chars_time;



    void setBorderWidth(float w);
    void setAlpha(float a);
    void setFontDescription(PangoFontDescription *fd);
    // for smooth show/hide
    float calcAlpha(uint64_t time);
    void enableProgressBar();
    float calcProgressRemains(uint64_t time);
    void drawNextByTime(cairo_t *cr, uint64_t time);
    // WARNING: each time performs search
    void drawByTime(cairo_t *cr, uint64_t time);
    void addCharTime(char_time_range ctr);
    void setStartTime(uint64_t s);
    void setEndTime(uint64_t e);
    void setStartEnd(uint64_t s, uint64_t e);
    TextLine();
    TextLine(char *str);
    void setText(char *str);
    PangoLayout* prepareLayout(cairo_t * cr);
    void drawTextWithProgress(cairo_t * cr, int offset, float part); // part in (0..1)
    void drawTextNoBg(cairo_t * cr);
    void drawTextWithBg(cairo_t * cr);

    int getWidth(cairo_t *cr);
    int getHeight(cairo_t *cr);
};


#endif // TEXTLINE_H
