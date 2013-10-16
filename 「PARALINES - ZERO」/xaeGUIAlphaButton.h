#ifndef XAEGUIALPHABUTTON_H
#define XAEGUIALPHABUTTON_H
#pragma once
#include <gui\xaeguibutton.h>

class xaeGUIAlphaButton : public xaeGUIButton
{
public:
    xaeGUIAlphaButton(int id, string key, string path_format, int x, int y, string sound_hover = "", string sound_click = "", string enter_mouse = "");
    virtual ~xaeGUIAlphaButton(void);

    virtual void                on_render(int x, int y);
};
#endif