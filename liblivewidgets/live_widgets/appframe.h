/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef APPFRAME_H
#define APPFRAME_H

#include <QToolBox>
#include <QFrame>
#include "live_widgets/bindableparent.h"
#include "live_widgets/toolbutton.h"
#include "liblivewidgets_global.h"

class Track;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT AppFrame : public QFrame, public BindableParent
{
    Q_OBJECT
    ToolButton* _tbBack,* _tbClose,* _tbNext,* _tbMini;
    bool s_minimized;

public:
    friend class ::Track;
    explicit AppFrame(QWidget *parent = 0);
    ~AppFrame();

    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);

public slots:
    virtual void toggleMinimized();
};

}

#endif // APPFRAME_H
