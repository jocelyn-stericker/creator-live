/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIDIOUTPUTCHOOSER_H
#define MIDIOUTPUTCHOOSER_H

#include <live/object>
#include <live_widgets/objectchooser.h>

#include <QWidget>

namespace Ui {
class MidiOutputChooser;
}

class MidiOutputChooser : public live_widgets::ObjectChooser
{
    Q_OBJECT
    
    bool m_busy;

public:
    explicit MidiOutputChooser(QWidget *parent = 0);
    ~MidiOutputChooser();

public slots:
    void step1();
    void step2();
    virtual void setMinimized(bool minimized = true);
    void checkedLogic(bool);
    
private:
    virtual void activateSelected() { Q_ASSERT(0); /* see step2() */ }
    virtual void updateObjects();

    Ui::MidiOutputChooser *m_ui;
    MidiOutputChooser(const MidiOutputChooser&);
    MidiOutputChooser& operator=(const MidiOutputChooser&);
};

#endif // MIDIOUTPUTCHOOSER_H
