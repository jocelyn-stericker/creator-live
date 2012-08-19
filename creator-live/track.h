/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef CL_TRACK_H
#define CL_TRACK_H

#include <live_widgets/track.h>

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

    OutputChooser* ui_outputName;
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

    void setOutputChooser(OutputChooser *);

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
};

#endif // CL_TRACK_H
