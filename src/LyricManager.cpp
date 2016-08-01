#include "../include/LyricManager.h"

void LyricManager::prepareTwoLinesLayout_2() // modifies lext_lines
    {

        curr_line_1 = 0;
        curr_line_2 = 0;

        bool add_to_first = true;

        int len = text_lines.size();
        //bool is_pair = (len%2)?(false):(true);


        uint64_t first_start = std::max( text_lines[0]->start_time - 1000  ,  (uint64_t)0 );


        for(int i = 0; i<len-1; i++) // go not to end
        {
            if(add_to_first)
            {
                if(0 == i) //special case
                {
                    text_lines[0]->start_time = first_start;
                    text_lines[1]->start_time = first_start;
                    text_lines_1.push_back(text_lines[i]);
                    add_to_first = false;
                    continue;
                }

                //if next line is not too far
                //if()

                //text_lines[i]->start_time = text_lines[i-1]->end_time;
                //text_lines[i]->end_time = (text_lines[i]->end_time + text_lines[i+1]->start_time) / 2;

                text_lines_1.push_back(text_lines[i]);
                add_to_first = false;
            }
            else
            {
                text_lines_2.push_back(text_lines[i]);
                add_to_first = true;
            }
        }

    }












    void LyricManager::prepareTwoLinesLayout() // modifies lext_lines
    {
        curr_line_1 = 0;
        curr_line_2 = 0;

        int len = text_lines.size();


        std::vector< std::pair<uint64_t, uint64_t> > time_bounds;
        for(int i = 0; i < text_lines.size(); i++)
        {
            time_bounds.push_back( std::make_pair(text_lines[i]->start_time, text_lines[i]->end_time) );
        }

        bool is_pair = (len%2)?(false):(true);

        uint64_t first_start = text_lines[0]->start_time;

        for(int i = 0; i<text_lines.size(); i+=2)
        {
            text_lines_1.push_back(text_lines[i]);
            if(i == 0)
            {
                //text_lines[0]->start_time -= 10;
                //text_lines[1]->start_time = text_lines[0]->start_time;
                //text_lines[0]->end_time = (text_lines[1]->start_time + text_lines[1]->end_time);
            }
            else
            {
                uint64_t s1, e1, s2, e2;
                s1 = time_bounds[i-1].first;//text_lines[i-1]->start_time;
                e1 = time_bounds[i-1].second;//text_lines[i-1]->end_time;

                s2 = time_bounds[i].first;//text_lines[i]->start_time;
                e2 = time_bounds[i].first;//text_lines[i]->end_time;


                text_lines[i-2]->end_time = text_lines[i]->start_time = std::min((s1+e1) / 2, s1 + 3000);
                //if(!(i == len-1)) text_lines[i-1]->end_time = text_lines[i+1]->start_time = std::min((s2+e2) / 2, s2 + 3000);
                if(!(i == len-1)) text_lines[i-1]->end_time = text_lines[i+1]->start_time = std::min((s2+e2) / 2, e1 + 3000);
            }

            if(!(i == len-1))text_lines_2.push_back(text_lines[i+1]);
        }
        text_lines_1[0]->start_time = 0; //first_start-10;
        text_lines_2[0]->start_time = 0; //first_start-10;


       // printf("%i\n", text_lines_2.back()->end_time);

    }

    void LyricManager::drawCountdown(cairo_t *cr, uint64_t time)
    {
        uint64_t diff = text_lines_1[0]->chars_time[0].start_time - time;
        if(diff >= 0 && diff <= 3000)
        {
            cairo_save(cr);
            cairo_translate(cr, 0, -200);

            if(diff <= 1000)
            {
                second_one.drawTextNoBg(cr);
            }
            else if(diff <= 2000)
            {
                second_two.drawTextNoBg(cr);
            }
            else
            {
                second_three.drawTextNoBg(cr);
            }

            cairo_restore(cr);
        }
    }

    void LyricManager::drawByTimeTwoLine(cairo_t *cr, uint64_t time)
    {
        drawCountdown(cr, time);
        drawTitle(cr,time);

        //
        //check if need rewind
        if(text_lines_1[curr_line_1]->start_time > time)
        {
            prepareRewind();
        }

        for(int i = curr_line_1; i < text_lines_1.size(); i++)
        {
            if((text_lines_1[i]->start_time <= time) && (text_lines_1[i]->end_time >= time))
            {
                text_lines_1[i]->drawNextByTime(cr,time);
                curr_line_1 = i;

                //printf("1: time:   %i    %i_%i\n",time, text_lines_1[i]->start_time,text_lines_1[i]->end_time);

                break;
            }
        }

        for(int i = curr_line_2; i < text_lines_2.size(); i++)
        {
            if((text_lines_2[i]->start_time <= time) && (text_lines_2[i]->end_time >= time))
            {

                //cairo_save(cr);
                cairo_translate(cr, 0, 70);

                text_lines_2[i]->drawNextByTime(cr,time);

                //cairo_restore(cr);

                curr_line_2 = i;

                //printf("2: time:   %i    %i_%i\n",time, text_lines_2[i]->start_time,text_lines_2[i]->end_time);

                break;
            }
        }
    }






    void LyricManager::drawByTime(cairo_t *cr, uint64_t time)
    {
        //find text_line
        for(int i = 0; i < text_lines.size(); i++)
        {
            if(text_lines[i]->start_time <= time && text_lines[i]->end_time >= time)
            {
                text_lines[i]->drawByTime(cr,time);
                return;
            }
        }
    }

    void LyricManager::testDraw(cairo_t *cr)
    {
        TextLine tl("asdfasdf sss");
        tl.drawTextNoBg(cr);

        cairo_translate(cr, 10, 100);

        tl.drawTextWithBg(cr);

        cairo_translate(cr, 10, 100);

        tl.drawTextWithProgress(cr,6,0.4);
    }


    void LyricManager::loadSRT(char *fname)
    {
        curr_line = 0;

        FILE *lyr_file = fopen(fname, "r");

        char* tmp_str = (char*)malloc(256);
        char  tmp_SRT[255];
                tmp_SRT[0] = 0; // empty
        size_t tmp_len = 255;

        // 0 - start
        // 1 - time
        // 2 - subtitle
        int state = 0; // initial

        TextLine *curr_tl = NULL;
        while(!feof(lyr_file))
        {
            int bt = getdelim(&tmp_str, &tmp_len, '\n' ,lyr_file);
            if(state == 0) // sub number
            {
                state = 1;
                curr_tl = new TextLine();
                continue;
            }
            else if(state == 1) //read time
            {
                int hs,ms,ss, he,me,se,   mss, mse;
                sscanf(tmp_str, "%i:%i:%i,%i --> %i:%i:%i,%i", &hs,&ms,&ss, &mss,   &he,&me,&se, &mse);
                curr_tl->setStartEnd(hs*3600000 + ms*60000 + ss*1000 + mss,  he*3600000 + me*60000 + se*1000 + mse);
                state = 2;
            }
            else if(state == 2) // read SRT text
            {
                if(tmp_str[0] == '\r') // end
                {
                    state = 0;
                    curr_tl->setText(tmp_SRT);
                    text_lines.push_back(curr_tl);
                    tmp_SRT[0] = 0; // empty
                    continue;
                }
                else
                {
                    strcat(tmp_SRT, tmp_str);
                }
            }
        }
    }

    void LyricManager::loadLyricFile(char *fname)
    {
        curr_line = 0;

        FILE *lyr_file = fopen(fname, "r");

        char* tmp_str = (char*)malloc(256);
        size_t tmp_len = 255;

        TextLine *curr_tl = NULL;

        PangoFontDescription *fd = pango_font_description_from_string ("Courier Bold 40");


        //
        // coundown
        PangoFontDescription *fd_countdown = pango_font_description_from_string ("Serif Bold 95");

        second_one.setFontDescription(fd_countdown);
        second_one.setText("1");

        second_two.setFontDescription(fd_countdown);
        second_two.setText("2");

        second_three.setFontDescription(fd_countdown);
        second_three.setText("3");

        //
        // title
        PangoFontDescription *fd_title = pango_font_description_from_string ("Sans 24");
        strcpy(title_txt, "хуета\nхуета");
        title.setText(title_txt);
        title.setBorderWidth(1.0);
        title.setFontDescription(fd_title);


        while(!feof(lyr_file))
        {
            int bt = getdelim(&tmp_str, &tmp_len, '\n' ,lyr_file);
            if(bt == -1)
            {
                puts("error");
            }
            else
            {
                puts(tmp_str);
            }

            if(tmp_str[0] != ' ') // probably start of text string (not time marks that starts with spaces)
            {
                // save current
                //if(curr_tl != NULL)
                //{
                   // text_lines.push_back(curr_tl);
                //}
                //start new
                curr_tl = new TextLine(tmp_str);
                curr_tl->setFontDescription(fd);

                curr_tl->enableProgressBar();

                text_lines.push_back(curr_tl);
            }
            else if(tmp_str[0] == ' ' &&
                    tmp_str[1] == ' ' &&
                    tmp_str[2] == ' ' &&
                    tmp_str[3] == ' ' &&
                    tmp_str[4] == ' ' &&
                    tmp_str[5] == ' ' &&
                    tmp_str[6] == ' ' &&
                    tmp_str[7] == ' ')
            {
                //index => time positions
                char_time_range ctr;
                int s,e,idx;
                sscanf(tmp_str, "        %i:%i:%i", &s, &e, &idx);
                ctr.start_time = s;
                ctr.end_time = e;
                ctr.index = idx;

                curr_tl->addCharTime(ctr);
            }

            else if(tmp_str[0] == ' ' &&
                    tmp_str[1] == ' ' &&
                    tmp_str[2] == ' ' &&
                    tmp_str[3] == ' ')
            {
                // entire line => line positions
                int s, e;
                sscanf(tmp_str, "    %i:%i",&s,&e);
                curr_tl->setStartEnd(s, e);
            }
        }

        free(tmp_str);
    }


    void LyricManager::drawTitle(cairo_t *cr, uint64_t time)
    {
        int64_t diff = text_lines_1[0]->chars_time[0].start_time - time;
        if(diff < 0)
            return;

        //calc alpha
        if(diff < 1500)
        {
            title.setAlpha(diff / 1500.0);
        }

        int h = title.getHeight(cr);
        int w = title.getWidth(cr);


        //find text_line
        cairo_save(cr);

        cairo_translate(cr, -200,-400);
        cairo_move_to(cr, 800, 10);
        cairo_line_to(cr, 800-w-80, 10);
        cairo_line_to(cr, 800-w-40, 10+h);
        cairo_line_to(cr, 800, 10+h);
        cairo_line_to(cr, 800, 10);
        cairo_set_source_rgba(cr, 0,0,0, ((0.6 < title.alpha)?(0.6):(title.alpha)));
        cairo_fill_preserve(cr);
        cairo_set_source_rgba(cr, 0.8,0.8,0.8, title.alpha);
        cairo_set_line_width(cr,2.5);
        cairo_stroke(cr);

        cairo_move_to(cr, 780 - w, 10);
        title.drawTextNoBg(cr);

        cairo_restore(cr);

    }

void LyricManager::prepareRewind()
{
    curr_line = 0;
    curr_line_1 = 0;
    curr_line_2 = 0;
    for(int i = 0; i < text_lines.size(); i++)
    {
        text_lines[i]->old_part = 0.0;
        text_lines[i]->old_index = 0;
        text_lines[i]->curr_char_idx = 0;
    }
}
