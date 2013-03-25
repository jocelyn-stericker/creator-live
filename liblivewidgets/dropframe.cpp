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

live_widgets::DropFrame::DropFrame(QWidget*p) : QFrame(p), m_dl(0) {
    setAcceptDrops(1);
}

live_widgets::DropFrame::~DropFrame() {
    delete m_dl;
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
    return(m_dl?m_dl->text():"");
}

void live_widgets::DropFrame::setText(QString x) {
    if (m_dl) {
        delete m_dl;
        m_dl=0;
    }
    m_dl=new QLabel;
    m_dl->setText(x);
    if (!layout()) {
        setLayout(new QVBoxLayout());
    }
    layout()->setMargin(0);
    layout()->addWidget(m_dl);
    layout()->setAlignment(m_dl,Qt::AlignCenter);
}
