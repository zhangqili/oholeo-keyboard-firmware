/*
 * lefl_nav.c
 */
#include "lefl.h"

void lefl_frame_init(lefl_frame_t* frame, lefl_page_t* *data,uint8_t len)
{
    frame->pages = data;
    frame->len = len;
}
void lefl_frame_go_forward(lefl_frame_t* frame)
{
    if(frame->index<frame->len)
        frame->index++;
}
void lefl_frame_go_back(lefl_frame_t* frame)
{
    if(frame->index>0)
        frame->index--;
}
void lefl_frame_navigate(lefl_frame_t* frame,lefl_page_t* page)
{
    if(frame->index+1<frame->len)
    {
        frame->index++;
        frame->pages[frame->index]=page;
    }
}
void lefl_frame_go_home(lefl_frame_t* frame)
{
    frame->index=0;
}
void lefl_frame_logic(lefl_frame_t* frame)
{
    frame->pages[frame->index]->page_logic_cb(frame->pages[frame->index]);
}
void lefl_frame_draw(lefl_frame_t* frame)
{
    frame->pages[frame->index]->page_draw_cb(frame->pages[frame->index]);
}

void lefl_link_frame_go_home(lefl_link_frame_t* frame)
{
    while(frame->current_page->back!=NULL)
        frame->current_page=frame->current_page->back;
}


void lefl_link_frame_go_forward(lefl_link_frame_t* frame)
{
    if(frame->current_page->forward!=NULL)
        frame->current_page=frame->current_page->forward;
    if(frame->current_page->page_load_cb!=NULL)
        frame->current_page->page_load_cb(frame->current_page);
}

void lefl_link_frame_go_back(lefl_link_frame_t* frame)
{
    if(frame->current_page->back!=NULL)
        frame->current_page=frame->current_page->back;
    if(frame->current_page->page_load_cb!=NULL)
        frame->current_page->page_load_cb(frame->current_page);
}

void lefl_link_frame_navigate(lefl_link_frame_t* frame,lefl_page_t* page)
{
    page->back=frame->current_page;
    frame->current_page->forward=page;
    frame->current_page=frame->current_page->forward;
    if(frame->current_page->page_load_cb!=NULL)
        frame->current_page->page_load_cb(frame->current_page);
}

void lefl_link_frame_logic(lefl_link_frame_t* frame)
{
    frame->current_page->page_logic_cb(frame->current_page);
}

void lefl_link_frame_draw(lefl_link_frame_t* frame)
{
    frame->current_page->page_draw_cb(frame->current_page);
}
