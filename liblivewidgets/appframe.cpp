/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/app>
#include <live_widgets/appframe.h>
#include <live_widgets/toolbutton.h>
#include <QLayout>
#include <QMetaObject>

using namespace live_widgets;

AppFrame::AppFrame(AbstractTrack *parent)
  : QFrame(parent)
  , BindableParent(this)
  , _tbBack(new ToolButton(this))
  , _tbClose(new ToolButton(this))
  , _tbNext(new ToolButton(this))
  , m_minimized(0)
  , m_desiredWidth(200)
  , b_resizing(false)
  { setFrameStyle(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setLineWidth(0);
    setMidLineWidth(0);
    _tbBack->show();
    _tbBack->setGeometry(2,2,8,8);
    _tbBack->setText("");
    _tbBack->setIcon(QIcon(":/icons/left.png"));
    _tbBack->setIconSize(QSize(8,8));
    _tbBack->setAutoRaise(1);
    _tbBack->setObjectName("_tbBack");
    _tbBack->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");
    _tbNext->show();
    _tbNext->setGeometry(22,2,8,8);
    _tbNext->setText("");
    _tbNext->setIcon(QIcon(":/icons/right.png"));
    _tbNext->setIconSize(QSize(8,8));
    _tbNext->setAutoRaise(1);
    _tbNext->setObjectName("_tbNext");
    _tbNext->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");
    _tbClose->show();
    _tbClose->setGeometry(width()-40-_tbNext->width(),18,40,40);
    _tbClose->setText("");
    _tbClose->setIcon(QIcon(":/icons/close.png"));
    _tbClose->setIconSize(QSize(40,40));
    _tbClose->setAutoRaise(1);
    _tbClose->setObjectName("_tbClose");
    _tbClose->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");
    setContentsMargins(2,10,2,2);

    setFixedHeight(342);
    setMinimumWidth(40);

    connect(&live::app::b_mode, SIGNAL(changeObserved(int,int)), this, SLOT(onModeChanged(int)));
    onModeChanged(live::app::b_mode);
}

AppFrame::~AppFrame() {
    //parentsystem
}

void AppFrame::resizeEvent(QResizeEvent *e) {
    if (!m_minimized) {
        _tbClose->raise();
        _tbClose->setGeometry(width()-40-_tbNext->width(),18,40,40);
    }
    QMetaObject::invokeMethod(parent(),"remakeChainWidget",Qt::DirectConnection);

    emit sizeChanged();

    if (e) QWidget::resizeEvent(e);
}

void AppFrame::moveEvent(QMoveEvent *e) {
    QMetaObject::invokeMethod(parent(),"remakeChainWidget",Qt::DirectConnection);
    QWidget::moveEvent(e);
}

void AppFrame::setDesiredWidth(int w) {
    m_desiredWidth = w;
    emit desiredWidthChanged();
}

void AppFrame::onModeChanged(int m) {
    _tbBack->setVisible(!m);
    _tbClose->setVisible(!m);
    _tbNext->setVisible(!m);
}
