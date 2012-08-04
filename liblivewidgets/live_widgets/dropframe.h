/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef DROPFRAME_H
#define DROPFRAME_H

#include <QFrame>

#include "liblivewidgets_global.h"

class QLabel;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT DropFrame : public QFrame {
    Q_OBJECT
    QLabel* s_dl;
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
public:
    DropFrame(QWidget*p);
    ~DropFrame();
    QString text();
    void setText(QString);
};

}

#endif // DROPFRAME_H
