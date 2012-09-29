/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef CL_TRACK_H
#define CL_TRACK_H

#include "live/ambition"

#include <live_widgets/track.h>
#include <live_widgets/appframe.h>

#include <live_widgets/rotatedlabel.h>
#include <live_widgets/midibindingqt.h>
#include <live_widgets/chaintypewidget.h>
#include <live_widgets/objectchooser.h>

#include <QPushButton>
#include <QBoxLayout>
#include <QToolButton>
#include <QInputDialog>
#include <QMessageBox>

#include "audiooutputchooser.h"

namespace live_widgets {
class TrackHint;
class AppFrame;
}

class Track : public AbstractTrack, public live_widgets::BindableParent {
    Q_OBJECT
public:
    live_widgets::TrackHint* s_th;
    live::Ambition& s_ambition;   /*003*/

    QList<live_widgets::AppFrame*> s_appUi_;  /*003B*/    //reintegrate

    QMutex x_me;

    int s_id;                   /*004*/
    static int s_lastId;

    bool s_busy;

    live_widgets::ObjectChooser* ui_outputChooser;
    live_widgets::ChainTypeWidget* ui_chainWidget;

    Track(live::ObjectPtr cinput, live::ObjectPtr coutput);
    Track(live::Ambition* bp);
protected:
    void initialize();
public:
    ~Track();

    int getMaximumWidthFor(QWidget*);

public slots:
    void resizeEvent(QResizeEvent *e = 0);
    void remakeChainWidget();

    void setOutputChooser(live_widgets::ObjectChooser *);

private:
    void clearUiPipeline();
    void makeUiPipeline();
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
public slots:
    void setOutput(live::ObjectPtr output,live::ObjectPtr loopback=0);
    void setInput(live::ObjectPtr input);
    void addWidget(int i,QWidget* frame);
    void addApp(int i,live_widgets::AppFrame* appUi,live::ObjectPtr app);
    void delApp(int i);
    void outputSelection();
    void logic_appBack();
    void logic_appDel();
    void logic_appNext();
    void updateGeometriesIfNeeded();
    void updateGeometriesOrDie();
public:
    QByteArray save();
    static Track* load(const QByteArray&);

private:
    Track(const Track&)
      : AbstractTrack()
      , BindableParent(this)
      , s_th(0)
      , s_ambition(*new live::Ambition)
      , s_appUi_()
      , x_me(QMutex::Recursive)
      , s_id(-1)
      , s_busy(0)
      , ui_outputChooser(0)
      , ui_chainWidget(0)
      { TCRASH();
    }
    Track& operator=(const Track&) {
        TCRASH();
        return *this;
    }
};

#endif // CL_TRACK_H
