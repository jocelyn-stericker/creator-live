/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live_widgets/chaintypewidget.h"
#include <QPainter>

live_widgets::ChainTypeWidget::ChainTypeWidget(QWidget* p) : QWidget(p) {}
void live_widgets::ChainTypeWidget::reset() { s_positions.clear(); s_midi.clear(); s_audio.clear(); update(); }
void live_widgets::ChainTypeWidget::setBack(int pos,bool midi,bool audio) { s_positions.push_back(pos); s_midi.push_back(midi); s_audio.push_back(audio); update();}
void live_widgets::ChainTypeWidget::paintEvent(QPaintEvent *e)
{
    bool a=0,b=0,c=0;
    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(e->rect(),QColor("white"));  //not actually white
    p.setCompositionMode(QPainter::CompositionMode_Source);
    int prev=0;
    for(int i=0;i<s_positions.size();i++)
    {
        if(s_midi[i]&&s_audio[i])
        {
            a=1;
            p.setBrush(QBrush("green"));
        }
        else if(s_midi[i])
        {
            b=1;
            p.setBrush(QBrush("blue"));
        }
        else if(s_audio[i])
        {
            c=1;
            p.setBrush(QBrush("yellow"));
        }
        p.drawRect(prev,0,s_positions[i]-prev,height());
        prev=s_positions[i];
    }
    if((a?1:0)+(b?1:0)+(c?1:0)<=1) {
        setFixedHeight(0);
    } else {
        setFixedHeight(2);
    }
}
