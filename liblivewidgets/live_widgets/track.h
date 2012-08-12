/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACK_H
#define TRACK_H

#include "live_widgets/appframe.h"
#include "live/ambition"

#include "live_widgets/rotatedlabel.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QToolButton>
#include <QInputDialog>
#include <QMessageBox>
#include "live_widgets/midibindingqt.h"
#include "live_widgets/chaintypewidget.h"

namespace live_widgets {
class TrackHint;
class AppFrame;
}

class AbstractTrack : public QWidget {
    Q_OBJECT
protected:
    AbstractTrack(QWidget* parent = 0);
public:
    virtual int getMaximumWidthFor(QWidget*) = 0;
    virtual void remakeChainWidget() = 0;
public slots:
    virtual void setOutput(live::ObjectPtr output,live::ObjectPtr loopback=0) = 0;
    virtual void setInput(live::ObjectPtr input) = 0;
    virtual void addApp(int i,live_widgets::AppFrame* appUi,live::ObjectPtr app) = 0;
    virtual void delApp(int i) = 0;
    virtual void outputSelection() = 0;
    virtual void logic_appBack() = 0;
    virtual void logic_appDel() = 0;
    virtual void logic_appNext() = 0;
    virtual void updateGeometriesIfNeeded() = 0;
    virtual void updateGeometriesOrDie() = 0;
};

class Track; // let app define

#endif // TRACK_H
