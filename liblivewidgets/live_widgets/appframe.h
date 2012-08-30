/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef APPFRAME_H
#define APPFRAME_H

#include <QToolBox>
#include <QFrame>
#include <live/variantbinding>
#include "live_widgets/bindableparent.h"
#include "live_widgets/toolbutton.h"
#include "live_widgets/track.h"
#include "liblivewidgets_global.h"

class AbstractTrack;
class Track;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT AppFrame : public QFrame, public BindableParent {
    Q_OBJECT
    Q_PROPERTY(int desiredWidth READ getDesiredWidth WRITE setDesiredWidth)
    ToolButton* _tbBack;
    ToolButton* _tbClose;
    ToolButton* _tbNext;
    ToolButton* _tbMini;
    bool s_minimized;
    int s_desiredWidth;

public:
    friend class ::Track;
    explicit AppFrame(AbstractTrack* parent = 0);
    ~AppFrame();

    virtual void resizeEvent(QResizeEvent*);
    virtual void moveEvent(QMoveEvent *);

    virtual bool expanding() const = 0;

    int getDesiredWidth() const { return s_desiredWidth; }
    live::Bound<bool> b_resizing;

signals:
    void sizeChanged();
    void desiredWidthChanged();

public slots:
    virtual void toggleMinimized();
    void setDesiredWidth(int);

private:
    Q_DISABLE_COPY(AppFrame)
};

}

#endif // APPFRAME_H
