/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

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
  , _tbMini(new ToolButton(this))
  , s_minimized(0)
  , s_desiredWidth(200)
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
    _tbMini->show();
    _tbMini->setGeometry(12,2,8,8);
    _tbMini->setText("");
    _tbMini->setIcon(QIcon(":/icons/minimize.png"));
    _tbMini->setIconSize(QSize(8,8));
    _tbMini->setAutoRaise(1);
    _tbMini->setObjectName("_tbMidi");
    _tbMini->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");
    connect(_tbMini,SIGNAL(clicked()),this,SLOT(toggleMinimized()));
    _tbNext->show();
    _tbNext->setGeometry(22,2,8,8);
    _tbNext->setText("");
    _tbNext->setIcon(QIcon(":/icons/right.png"));
    _tbNext->setIconSize(QSize(8,8));
    _tbNext->setAutoRaise(1);
    _tbNext->setObjectName("_tbNext");
    _tbNext->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");

    _tbClose->show();
    _tbClose->setGeometry(width()-2-_tbNext->width(),2,8,8);
    _tbClose->setText("");
    _tbClose->setIcon(QIcon(":/icons/close.png"));
    _tbClose->setIconSize(QSize(8,8));
    _tbClose->setAutoRaise(1);
    _tbClose->setObjectName("_tbClose");
    _tbClose->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");
    setContentsMargins(2,10,2,2);

//    setMinimumSize(maximumSize());
//    setMaximumSize(minimumSize());

    setFixedHeight(342);
    setMinimumWidth(40);
}

AppFrame::~AppFrame() {
    //parentsystem
}

void AppFrame::toggleMinimized() {
    if (parentWidget()&&parentWidget()->layout()) {
        parentWidget()->layout()->setAlignment( Qt::AlignLeft );
    }
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);

    if (s_minimized) {
        setMinimumWidth(40);
        _tbBack->setGeometry(0,0,10,10);
        _tbClose->setGeometry(10,0,10,10);
        _tbMini->setGeometry(20,0,10,10);
        _tbMini->setText("");
        _tbMini->setIcon(QIcon(":/icons/minimize.png"));
        _tbNext->setGeometry(30,0,10,10);
        setMaximumWidth(16777215);
        _tbBack->show();
        _tbClose->show();
        _tbNext->show();
        s_minimized=0;
        resizeEvent(0);
    } else {
        _tbMini->setGeometry(0,0,10,10);
//        _tbBack->setGeometry(0,10,10,10);
//        _tbClose->setGeometry(0,20,10,10);
//        _tbNext->setGeometry(0,30,10,10);
        _tbBack->hide();
        _tbClose->hide();
        _tbNext->hide();
        _tbMini->setText("+");
        _tbMini->setIcon(QIcon());
        setMinimumWidth(10);
        setMaximumWidth(10);
        s_minimized=1;
    }
}

void AppFrame::resizeEvent(QResizeEvent *e) {
    if (!s_minimized) {
        _tbClose->setGeometry(width()-4-_tbNext->width(),2,8,8);
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
    s_desiredWidth = w;
    emit desiredWidthChanged();
}
