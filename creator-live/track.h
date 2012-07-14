/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACK_H
#define TRACK_H

#include "live_widgets/appframe.h"
#include "ambition.h"

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
}

class Track : public QWidget, public live_widgets::BindableParent
{
    Q_OBJECT
public:
    live_widgets::TrackHint* s_th;
    Ambition& s_ambition;   /*003*/

    QList<live_widgets::AppFrame*> s_appUi_;  /*003B*/    //reintegrate

    int s_id;                   /*004*/
    static int s_lastId;

    live_widgets::RotatedLabel* ui_outputName;
    QHBoxLayout* ui_mainLayout;
    QSpacerItem* ui_spacerItem;
    live_widgets::ChainTypeWidget* ui_chainWidget;

    Track(live::ObjectPtr cinput, live::ObjectPtr coutput);
    Track(Ambition* bp);
    ~Track();
    void resizeEvent(QResizeEvent *e);
public slots:
    void remakeChainWidget();

private:
    void clearUiPipeline();
    void makeUiPipeline();
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
public slots:
    void setOutput(live::ObjectPtr output,live::ObjectPtr loopback=0);
    void setInput(live::ObjectPtr input);
    void addApp(int i,live_widgets::AppFrame* appUi,live::ObjectPtr app);
    void delApp(int i);
    void outputSelection();
    void logic_appBack();
    void logic_appDel();
    void logic_appNext();
public:
    QByteArray save();
    static Track* load(const QByteArray&);
};

#endif // TRACK_H
