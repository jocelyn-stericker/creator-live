/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/objectchooser.h>

#include <QPropertyAnimation>

live_widgets::ObjectChooser::ObjectChooser(QWidget* parent)
  : QFrame(parent)
  , ui_minimized(0)
  , ui_topFrame(0)
  , ui_bottomFrame(0)
  , s_alignedLeft(0)
  , b_trackName(0)
  {
    connect(live::object::singleton(),SIGNAL(stockChanged()),this,SLOT(updateObjects()));
}

void live_widgets::ObjectChooser::setMinimized(bool minimized) {
    ui_minimized->setChecked(!minimized);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QPropertyAnimation* qaa = new QPropertyAnimation(this, "fixedWidth");
    qaa->setStartValue(width());
    qaa->setEndValue(minimized ? 55 : 400);
    qaa->setDuration(200);
    qaa->setEasingCurve(QEasingCurve::InQuad);
    qaa->start(QAbstractAnimation::DeleteWhenStopped);
    connect(qaa, SIGNAL(destroyed()), this, SIGNAL(doneResizing()));
    if(minimized)
        connect(qaa, SIGNAL(destroyed()), this, SLOT(setCornersRounded()));
    else
        setCornersRounded(false);
}

void live_widgets::ObjectChooser::resizeEvent(QResizeEvent* e) {
    if (e) QWidget::resizeEvent(e);
    emit resized();
}

void live_widgets::ObjectChooser::setCornersRounded(bool rounded) {
    QString state[2] = { "0px;", "4px;" };
    QString alignment[2] = { "left", "right" };

    QString ss = ui_topFrame->styleSheet();
    ss.replace("border-top-" + alignment[s_alignedLeft?1:0] + "-radius: " + state[rounded?0:1]
             , "border-top-" + alignment[s_alignedLeft?1:0] + "-radius: " + state[rounded?1:0]);
    ui_topFrame->setStyleSheet(ss);

    ss = ui_bottomFrame->styleSheet();
    ss.replace("border-bottom-" + alignment[s_alignedLeft?1:0] + "-radius: " + state[rounded?0:1]
             , "border-bottom-" + alignment[s_alignedLeft?1:0] + "-radius: " + state[rounded?1:0]);
    ui_bottomFrame->setStyleSheet(ss);
}
