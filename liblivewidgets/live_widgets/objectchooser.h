/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef OBJECTCHOOSER_H
#define OBJECTCHOOSER_H

#include <live/object>
#include <live/variantbinding>
#include <live_widgets/toolbutton.h>
#include <live_widgets/bindableparent.h>

#include <QFrame>
#include <QWidget>

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT ObjectChooser : public QFrame, public live_widgets::BindableParent
{
    Q_OBJECT
    Q_PROPERTY(int fixedWidth READ width WRITE setFixedWidth)
    Q_PROPERTY(int fixedHeight READ height WRITE setFixedHeight)

    live_widgets::ToolButton* ui_minimized;
    QWidget* ui_topFrame;
    QWidget* ui_bottomFrame;
    bool m_alignedLeft;
protected:
    int m_oldHeight;
    int m_maximizedWidth;
    int m_maximizedHeight;

public:
    live::Bound<QString> b_trackName;
    ObjectChooser(QWidget* parent);

public slots:
    void setMaximized(bool maximized = true) { setMinimized(!maximized); }
    virtual void setMinimized(bool minimized = true);

    virtual void activateSelected() = 0;
    virtual void updateObjects() = 0;

    void setCornersRounded(bool rounded = true);

protected:
    void setMinimizedButton(live_widgets::ToolButton* u) { ui_minimized = u; }
    void setTopFrame(QWidget* u) { ui_topFrame = u; }
    void setBottomFrame(QWidget* u) { ui_bottomFrame = u; }
    void setAlignedLeft(bool alignLeft) { m_alignedLeft = alignLeft; }

private:
    void resizeEvent(QResizeEvent *);

signals:
    void resized();
    void doneResizing();
    void objectChosen(live::ObjectPtr);
    void objectChosen(live::ObjectPtr, live::ObjectPtr);
};

}

#endif // OBJECTCHOOSER_H
