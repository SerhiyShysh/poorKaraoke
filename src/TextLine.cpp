#include "../include/TextLine.h"



    void TextLine::setFontDescription(PangoFontDescription *fd)
    {
        font_descr = fd;
    }


    // for smooth show/hide
    float TextLine::calcAlpha(uint64_t time)
    {
        const float time_delta = 250.0; // ms
        //at show
        if(time - start_time < time_delta)
        {
            return (time - start_time) / time_delta;
        }

        //at hide
        if(end_time - time < time_delta)
        {
            return (end_time - time) / time_delta;
        }
        return 1.0;
    }

    void TextLine::enableProgressBar()
    {
        show_progress_bar = true;
    }

    float TextLine::calcProgressRemains(uint64_t time)
    {
        if(time > chars_time[0].start_time)
            return 0.0;
        return float(chars_time[0].start_time - time) / float(chars_time[0].start_time - start_time);
    }

    void TextLine::drawNextByTime(cairo_t *cr, uint64_t time)
    {


        int sz = chars_time.size();
        if(!sz)
        {
            return;
        }


        alpha = calcAlpha(time);
        progress = calcProgressRemains(time);



        //find char
        for(int i=curr_char_idx; i< sz; i++)
        {
            if(chars_time[i].start_time <= time && chars_time[i].end_time >= time)
            {
                uint64_t s , e;
                s = chars_time[i].start_time;
                e = chars_time[i].end_time;

                float part = old_part = float(time - s) / float(e-s);
                old_index = chars_time[i].index;

                //printf("%f\n", part);

                drawTextWithProgress(cr,chars_time[i].index, part);
                curr_char_idx = i;
                return;
            }
        }
        if(time > chars_time[sz-1].end_time)
        {
            drawTextWithProgress(cr,chars_time[sz-1].index, 1);
            return;
        }
        if(time < chars_time[0].start_time)
        {
            drawTextWithProgress(cr,0, 0);
            return;
        }
        // fix blinking
        drawTextWithProgress(cr,old_index, old_part);

    }

    // WARNING: each time performs search
    void TextLine::drawByTime(cairo_t *cr, uint64_t time)
    {
        //find char
        int sz = chars_time.size();
        for(int i=0; i< sz; i++)
        {
            if(chars_time[i].start_time <= time && chars_time[i].end_time >= time)
            {
                uint64_t s,e;
                s = chars_time[i].start_time;
                e = chars_time[i].end_time;

                float part = old_part = float(time - s) / float(e-s);
                old_index = chars_time[i].index;

                //printf("%f\n", part);

                drawTextWithProgress(cr,chars_time[i].index, part);
                return;
            }
        }
        if(old_index == sz -1)
            drawTextNoBg(cr);
        else
            drawTextWithProgress(cr,old_index, old_part);
    }

    void TextLine::addCharTime(char_time_range ctr)
    {
        chars_time.push_back(ctr);
    }

    void TextLine::setStartTime(uint64_t s)
    {
        start_time = s;
    }

    void TextLine::setEndTime(uint64_t e)
    {
        end_time = e;
    }

    void TextLine::setStartEnd(uint64_t s, uint64_t e)
    {
        start_time = s;
        end_time = e;
    }

    TextLine::TextLine()
    {
        curr_char_idx = 0;
        old_index = 0;
        old_part = 0.0;
        alpha = 1.0;
        border_width = 2.0;
        show_progress_bar = false;
    }

    TextLine::TextLine(char *str)
    {
        strcpy(text, str);
        curr_char_idx = 0;
        old_index = 0;
        old_part = 0.0;
        alpha = 1.0;
        border_width = 2.0;
        show_progress_bar = false;
    }

    void TextLine::setText(char *str)
    {
        strcpy(text, str);
    }

    PangoLayout* TextLine::prepareLayout(cairo_t * cr)
    {
        PangoLayout *layout;


        layout = pango_cairo_create_layout (cr);

        pango_layout_set_text (layout, text, -1);

        pango_layout_set_font_description (layout, font_descr);

        PangoRectangle rect;
        pango_layout_get_pixel_extents(layout,NULL,&rect);
        cairo_save(cr);
        cairo_translate(cr, -rect.width/2,0);

        pango_cairo_layout_path(cr, layout);

        cairo_restore(cr);

        return layout;
    }

    void TextLine::drawTextWithProgress(cairo_t * cr, int offset, float part) // part in (0..1)
    {
        PangoLayout *layout;

        layout = pango_cairo_create_layout (cr);

        pango_layout_set_text (layout, text, -1);

        pango_layout_set_font_description (layout, font_descr);


        //// center align
        PangoRectangle rect;
        pango_layout_get_pixel_extents(layout,NULL,&rect);
        cairo_save(cr);
        cairo_translate(cr, -rect.width/2,0);


        //
        // show progress bar
        // progress [0..1]
        if(show_progress_bar && progress > 0.0)
        {
            cairo_save(cr);

            cairo_move_to(cr, rect.x, rect.y+rect.height/4);
            cairo_line_to(cr, rect.x-80 * progress, rect.y+rect.height/4);

            cairo_set_source_rgba(cr, 1, 0, 0,alpha);
            cairo_set_line_width(cr,4.0);
            cairo_stroke(cr);

            cairo_restore(cr);
        }

        //// border aroung glyphs
        pango_cairo_layout_path(cr, layout);



        //// highlighted glyph rect
        PangoRectangle pos;
        pango_layout_index_to_pos(layout, offset, &pos);



        int x_pos = (pos.x + part*pos.width)/PANGO_SCALE;

        //

        cairo_pattern_t* grad = cairo_pattern_create_linear(x_pos  ,1, x_pos+1 , 1);

        cairo_pattern_add_color_stop_rgba(grad, 0.9,   1,0,0,alpha);
        cairo_pattern_add_color_stop_rgba(grad, 0.99,  1,1,1,alpha);

        //ua  flag colors
        //cairo_pattern_add_color_stop_rgba(grad, 0.9,   0,  0.341,  0.7215,  alpha);
        //cairo_pattern_add_color_stop_rgba(grad, 0.99,  1,  0.843,  0,  alpha);



        cairo_set_source(cr, grad);
        cairo_fill_preserve(cr);

        //stroke
        //cairo_set_operator(cr,CAIRO_OPERATOR_DIFFERENCE);
        cairo_set_source_rgba(cr, 0, 0, 0,alpha);
        cairo_set_line_width(cr, border_width);
        cairo_stroke(cr);

        cairo_restore(cr);
    }

    void TextLine::drawTextNoBg(cairo_t * cr)
    {
        prepareLayout(cr);
        //fill
        cairo_set_source_rgba(cr, 1,1,1, alpha);
        cairo_fill_preserve(cr);
        //stroke
        cairo_set_source_rgba(cr, 0, 0, 0, alpha);
        cairo_set_line_width(cr,border_width);
        cairo_stroke(cr);
    }

    void TextLine::drawTextWithBg(cairo_t * cr)
    {
        prepareLayout(cr);
        //fill
        cairo_set_source_rgba(cr, 1,0,0, alpha);
        cairo_fill_preserve(cr);
        //stroke
        cairo_set_source_rgba(cr, 0, 0, 0, alpha);
        cairo_set_line_width(cr,border_width);
        cairo_stroke(cr);
    }


    int TextLine::getWidth(cairo_t *cr)
    {
        PangoLayout *layout;
        layout = pango_cairo_create_layout (cr);
        pango_layout_set_text (layout, text, -1);
        pango_layout_set_font_description (layout, font_descr);
        PangoRectangle rect;
        pango_layout_get_pixel_extents(layout,NULL,&rect);
        return rect.width;
    }

    int TextLine::getHeight(cairo_t *cr)
    {
        PangoLayout *layout;
        layout = pango_cairo_create_layout (cr);
        pango_layout_set_text (layout, text, -1);
        pango_layout_set_font_description (layout, font_descr);
        PangoRectangle rect;
        pango_layout_get_pixel_extents(layout,NULL,&rect);
        return rect.height;
    }

    void TextLine::setAlpha(float a)
    {
        alpha = a;
    }


    void TextLine::setBorderWidth(float w)
    {
        border_width = w;
    }
