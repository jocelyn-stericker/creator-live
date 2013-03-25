/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVEWINDOW_H
#define LIVEWINDOW_H

#include <live/object>
#include <live/ambition>
#include <live_widgets/bindableparent.h>
#include <live_widgets/midibindingqt.h>
#include <QMainWindow>

namespace Ui {
class LiveWindow;
}

namespace live_widgets {
class VScrollContainer;
class IntroWizard;
}

namespace live {
class Ambition;
}

using live::Ambition;

class LiveWindow : public QWidget, public live_widgets::BindableParent
{
    Q_OBJECT
public:
    class Patch {
    public:
        Patch()
          : ambitions()
          , widgets()
          {
        }
        QList< live::Ambition* > ambitions;
        QList< QWidget* > widgets;
        void deactivate() {
            foreach(live::Ambition* a, ambitions) a->unset();
            foreach(QWidget* w, widgets) w->hide();
        }
        void activate() {
            foreach(live::Ambition* a, ambitions) a->set();
            foreach(QWidget* w, widgets) w->show();
        }
    };
private:
    QList<Patch*> m_patches;
    int m_curPatch;
    QString m_fileName;
    QList< QAction* > m_recent;
    QMenu* m_recentMenu;
    live_widgets::IntroWizard* m_iw;

public:
    explicit LiveWindow(QWidget *parent = 0);
    static LiveWindow* singleton;
    ~LiveWindow();
    bool askForClose(QString title="Quit Creator Live?",QString question="Quit anyway?");

    Patch* curPatch() { return m_patches[m_curPatch]; }
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

    void onAmbitionCreated(Ambition*);
    void onAmbitionDestroyed(Ambition*);

private:
    void closeEvent(QCloseEvent *);
    Ui::LiveWindow *ui;

    LiveWindow(const LiveWindow&)
      : QWidget()
      , BindableParent(this)
      , m_patches()
      , m_curPatch(-1)
      , m_fileName("")
      , m_recent()
      , m_recentMenu(0)
      , m_iw(0)
      , ui(0)
      { TCRASH();
    }

    LiveWindow& operator=(const LiveWindow&) {
        TCRASH();
        return *this;
    }

    RELOADABLE(LiveWindow)
};

#endif // LIVEWINDOW_H
