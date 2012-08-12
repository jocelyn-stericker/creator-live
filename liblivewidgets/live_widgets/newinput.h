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
    bool s_allowMidi;
    bool s_allowAudio;
public:
    QStringList all;
    NewInput(QWidget*parent, bool popout, bool allowMidi, bool allowAudio);

    void resizeEvent(QResizeEvent *);

    live::Bound<QString> b_trackName;
    live::Bound<bool> b_audio;  // else midi

public slots:
    void internalStockChangedEvent();
    void go();
    void refresh();
    void rowChangedEvent();

    void maximize(bool reverse = false) { minimize(!reverse); }
    void minimize(bool reverse = false);

    void onSetAudio(bool);

    void squareCorners();
    void roundCorners();

signals:
    void created(live::ObjectPtr device);
    void newOutputRequested();

};

}

#endif // NEWINPUT_H
