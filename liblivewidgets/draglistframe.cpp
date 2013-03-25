/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/draglistframe.h>

live_widgets::DragListFrame::DragListFrame(QStringList x,QWidget*p) : QFrame(p), m_list(), m_layout(new QGridLayout(this)) {
    int r=0,c=-1;
    for (int i=0;i<x.size();i++) {
        m_list.push_back(new DragLabel);
        m_list.back()->setParent(this);
        m_list.back()->setText(x[i]);
        if (++c==2) {
            ++r;
            c=0;
        }
        m_layout->addWidget(m_list.back(),r,c);
    }
    setMinimumHeight(m_list.size()*40);
}
