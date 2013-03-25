/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/chaintypewidget.h>
#include <QPainter>

live_widgets::ChainTypeWidget::ChainTypeWidget(QWidget* p)
  : QWidget(p)
  , m_positions()
  , m_midi()
  , m_audio()
  {
}
void live_widgets::ChainTypeWidget::reset() { m_positions.clear(); m_midi.clear(); m_audio.clear(); update(); }
void live_widgets::ChainTypeWidget::setBack(int pos,bool midi,bool audio) { m_positions.push_back(pos); m_midi.push_back(midi); m_audio.push_back(audio); update();}
void live_widgets::ChainTypeWidget::paintEvent(QPaintEvent *e) {
    bool a=0,b=0,c=0;
    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(e->rect(),QColor("white")); // not actually white
    p.setCompositionMode(QPainter::CompositionMode_Source);
    int prev=0;
    for (int i=0;i<m_positions.size();i++) {
        if (m_midi[i]&&m_audio[i]) {
            a=1;
            p.setBrush(QBrush("green"));
        } else if (m_midi[i]) {
            b=1;
            p.setBrush(QBrush("blue"));
        } else if (m_audio[i]) {
            c=1;
            p.setBrush(QBrush("yellow"));
        }
        p.drawRect(prev,0,m_positions[i]-prev,height());
        prev=m_positions[i];
    }
    if ((a?1:0)+(b?1:0)+(c?1:0)<=1) {
        setFixedHeight(0);
    } else {
        setFixedHeight(2);
    }
}
