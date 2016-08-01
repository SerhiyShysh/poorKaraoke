#ifndef LYRICMANAGER_H
#define LYRICMANAGER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
//
#include "TextLine.h"


class LyricManager{

    //
    //need to replace vector by list
    std::vector<TextLine*> text_lines;

    std::vector<TextLine*> text_lines_1;
    std::vector<TextLine*> text_lines_2;

    int curr_line;
    int curr_line_1;
    int curr_line_2;

    TextLine second_one;
    TextLine second_two;
    TextLine second_three;

    TextLine title;
    char title_txt[400];

public:

    void prepareTwoLinesLayout_2(); // modifies lext_lines
    void prepareTwoLinesLayout(); // modifies lext_lines
    void drawByTimeTwoLine(cairo_t *cr, uint64_t time);
    void drawByTime(cairo_t *cr, uint64_t time);
    static void testDraw(cairo_t *cr);
    void loadSRT(char *fname);
    void loadLyricFile(char *fname);

    void prepareRewind();

    void drawCountdown(cairo_t *cr, uint64_t time);
    void drawTitle(cairo_t *cr, uint64_t time);

};


#endif // LYRICMANAGER_H
