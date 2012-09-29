/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef TRACKINPUTSELECT_H
#define TRACKINPUTSELECT_H

#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSettings>
#include <QMainWindow>
#include <live_widgets/midibindingqt.h>
#include <live_widgets/objectchooser.h>
#include "liblivewidgets_global.h"

class Ui_Frame;

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT TrackInputSelect : public live_widgets::ObjectChooser {
    Q_OBJECT
private:
    Ui_Frame* s_ui;
    bool s_allowMidi;
    bool s_allowAudio;

    int s_selectedFilter;       /*005*/
    int s_selectedMode;         /*006*/

    int s_inputId;
    static int ss_inputId;
public:
    QStringList all;
    TrackInputSelect(QWidget*parent, bool popout, bool allowMidi, bool allowAudio);

    live::Bound<QString> b_trackName;
    live::Bound<bool> b_audio;  // else midi

    QGraphicsScene* ui_instScene;
    QGraphicsView* ui_instView;
    live_widgets::ToolButton* ui_colourSelect[5];
    live_widgets::ToolButton* ui_modeSelect[5];
    live::MidiFilter* s_midiFilters[5];   /*007*/

    void drawKeyboard();

public slots:
    void reactToPianoKeyUpdate();
    void changeActiveFilter(bool really);
    void changeActiveMode(bool really);
    void changeActiveMode_2(int mode);

public slots:
    void updateObjects();
    void activateSelected();

    void onSetAudio(bool);

    void enableAddTrackButton();
    void showKeyboard(bool);

signals:
    void newOutputRequested();

private:
    Q_DISABLE_COPY(TrackInputSelect)
};

}

#endif // TRACKINPUTSELECT_H
