/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef NEWINPUT_H
#define NEWINPUT_H

#include <QDesktopWidget>
#include <QSettings>
#include <QMainWindow>
#include "live_widgets/midibindingqt.h"
#include "liblivewidgets_global.h"

class Ui_Frame;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT NewInput : public QFrame {
    Q_OBJECT
private:
    Ui_Frame* s_ui;
public:
    QStringList all;
    NewInput(QWidget*parent);

    void resizeEvent(QResizeEvent *);

public slots:
    void internalStockChangedEvent();
    void go();
    void refresh();
    void rowChangedEvent();

    void maximize(bool reverse = false) { minimize(!reverse); }
    void minimize(bool reverse = false);

signals:
    void created(live::ObjectPtr device);

};

}

#endif // NEWINPUT_H
