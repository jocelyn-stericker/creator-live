/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/dropframe.h>

#include <QDebug>
#include <QDropEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QMimeData>

live_widgets::DropFrame::DropFrame(QWidget*p) : QFrame(p), s_dl(0) {
    setAcceptDrops(1);
}

live_widgets::DropFrame::~DropFrame() {
    delete s_dl;
}

void live_widgets::DropFrame::dragEnterEvent(QDragEnterEvent *e) {
    qDebug() << e->mimeData();
    if (e->mimeData()->hasFormat("text/plain")) {
        e->acceptProposedAction();
    }
}

void live_widgets::DropFrame::dropEvent(QDropEvent *e) {
    setText(e->mimeData()->text());
}

QString live_widgets::DropFrame::text() {
    return(s_dl?s_dl->text():"");
}

void live_widgets::DropFrame::setText(QString x) {
    if (s_dl) {
        delete s_dl;
        s_dl=0;
    }
    s_dl=new QLabel;
    s_dl->setText(x);
    if (!layout()) {
        setLayout(new QVBoxLayout());
    }
    layout()->setMargin(0);
    layout()->addWidget(s_dl);
    layout()->setAlignment(s_dl,Qt::AlignCenter);
}
