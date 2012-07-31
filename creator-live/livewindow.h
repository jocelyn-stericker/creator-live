/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVEWINDOW_H
#define LIVEWINDOW_H

#include <QMainWindow>
#include "live/object.h"
#include "ambition.h"
#include "live_widgets/midibindingqt.h"
#include "live_widgets/bindableparent.h"

namespace Ui {
class LiveWindow;
}

namespace live_widgets {
class VScrollContainer;
class IntroWizard;
}

class LiveWindow : public QWidget, public live_widgets::BindableParent
{
    Q_OBJECT
public:
    class Patch {
    public:
        QList< Ambition* > ambitions;
        QList< QWidget* > widgets;
        void deactivate() {
            foreach(Ambition* a, ambitions) a->unset();
            foreach(QWidget* w, widgets) w->hide();
        }
        void activate() {
            foreach(Ambition* a, ambitions) a->set();
            foreach(QWidget* w, widgets) w->show();
        }
    };
private:
    QList<Patch*> s_patches;
    int s_curPatch;
    QString s_fileName;
    QList< QAction* > s_recent;
    QMenu* s_recentMenu;
    live_widgets::IntroWizard* s_iw;

public:
    explicit LiveWindow(QWidget *parent = 0);
    static LiveWindow* singleton;
    ~LiveWindow();
    bool askForClose(QString title="Quit Creator Live?",QString question="Quit anyway?");

    Patch* curPatch() { return s_patches[s_curPatch]; }
    live_widgets::VScrollContainer* hathorView();
    
public slots:
    void newProject(bool ask=1);
    void selectMode();
    void newInput();
    void reactOnCreation(live::ObjectPtr);
    void hideInsert(bool animate = true);
    void showInsert();
    void setMode(int a);
    void setKey(int a);
    void setBPM(int a);
    void toggleMetro(bool m);

    void setCurrentPatch(int a);
    void setCurrentPatchName(QString s);
    void editCurrentPatchName();
    void insertPatch();

    void editVst();
    void editVstDone();
    void editAudioSetup();
    void editAudioSetupDone();

    void saveAct();
    void saveAs();
    void open();

    void updateRecent();
    void loadRecent();

private:
    void closeEvent(QCloseEvent *);
    Ui::LiveWindow *ui;

    RELOADABLE(LiveWindow)
};

#endif // LIVEWINDOW_H
