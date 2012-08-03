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
#include "live_widgets/track.h"
#include "liblivewidgets_global.h"

class AbstractTrack;
class Track;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT AppFrame : public QFrame, public BindableParent
{
    Q_OBJECT
    Q_PROPERTY(int desiredWidth READ getDesiredWidth WRITE setDesiredWidth)
    ToolButton* _tbBack,* _tbClose,* _tbNext,* _tbMini;
    bool s_minimized;
    int s_desiredWidth;

public:
    friend class ::Track;
    explicit AppFrame(AbstractTrack* parent = 0);
    ~AppFrame();

    void resizeEvent(QResizeEvent*);
    void moveEvent(QMoveEvent *);

    virtual bool expanding() const = 0;

    int getDesiredWidth() const { return s_desiredWidth; }

signals:
    void sizeChanged();
    void desiredWidthChanged(int);

public slots:
    virtual void toggleMinimized();
    void setDesiredWidth(int);
};

}

#endif // APPFRAME_H
