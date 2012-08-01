/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "live_widgets/appframe.h"
#include "live_widgets/toolbutton.h"
#include <QLayout>
#include <QMetaObject>

live_widgets::AppFrame::AppFrame(QWidget *parent) :
    QFrame(parent),
    BindableParent(this),
    s_minimized(0)
{
    setFrameStyle(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setLineWidth(0);
    setMidLineWidth(0);
    _tbBack = new ToolButton(this);
    _tbBack->show();
    _tbBack->setGeometry(2,2,8,8);
    _tbBack->setText("");
    _tbBack->setIcon(QIcon(":/icons/left.png"));
    _tbBack->setIconSize(QSize(8,8));
    _tbBack->setAutoRaise(1);
    _tbBack->setObjectName("_tbBack");
    _tbBack->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");
    _tbMini = new ToolButton(this);
    _tbMini->show();
    _tbMini->setGeometry(12,2,8,8);
    _tbMini->setText("");
    _tbMini->setIcon(QIcon(":/icons/minimize.png"));
    _tbMini->setIconSize(QSize(8,8));
    _tbMini->setAutoRaise(1);
    _tbMini->setObjectName("_tbMidi");
    _tbMini->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");
    connect(_tbMini,SIGNAL(clicked()),this,SLOT(toggleMinimized()));
    _tbNext = new ToolButton(this);
    _tbNext->show();
    _tbNext->setGeometry(22,2,8,8);
    _tbNext->setText("");
    _tbNext->setIcon(QIcon(":/icons/right.png"));
    _tbNext->setIconSize(QSize(8,8));
    _tbNext->setAutoRaise(1);
    _tbNext->setObjectName("_tbNext");
    _tbNext->setStyleSheet("padding: 0px; border-radius: 0px; border-width: 0px; background-color:4d4c4d;");

    _tbClose = new ToolButton(this);
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

live_widgets::AppFrame::~AppFrame()
{
    //parentsystem
}

void live_widgets::AppFrame::toggleMinimized()
{
    if(parentWidget()&&parentWidget()->layout())
    {
        parentWidget()->layout()->setAlignment( Qt::AlignLeft );
    }
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);

    if(s_minimized)
    {
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
    }
    else
    {
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

void live_widgets::AppFrame::resizeEvent(QResizeEvent *e)
{
    if(!s_minimized) {
        _tbClose->setGeometry(width()-4-_tbNext->width(),2,8,8);
    }
    QMetaObject::invokeMethod(parent(),"remakeChainWidget",Qt::DirectConnection);
    if(e) QWidget::resizeEvent(e);
}

void live_widgets::AppFrame::moveEvent(QMoveEvent *e)
{
    QMetaObject::invokeMethod(parent(),"remakeChainWidget",Qt::DirectConnection);
    QWidget::moveEvent(e);
}
