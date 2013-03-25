/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVEMETROSPINBOX_H
#define LIVEMETROSPINBOX_H

#include "live_widgets/spinbox.h"
#include "live/metronome"
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT MetroSpinbox : public SpinBox {
    Q_OBJECT
    int m_toggle;
    int m_lastBeat;
public:
    explicit MetroSpinbox(QWidget *parent = 0);
    virtual ~MetroSpinbox();

    void sync(const live::SyncData& data);

private:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

};

}

#endif // LIVEMETROSPINBOX_H
