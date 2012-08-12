/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIDIOUTPUTCHOOSER_H
#define MIDIOUTPUTCHOOSER_H

#include <live/object>

#include <QWidget>

namespace Ui {
class MidiOutputChooser;
}

class MidiOutputChooser : public QWidget
{
    Q_OBJECT
    
    bool s_busy;

public:
    explicit MidiOutputChooser(QWidget *parent = 0);
    ~MidiOutputChooser();

public slots:
    void resizeEvent(QResizeEvent *e);
    void step1();
    void step2();
    void maximize(bool reverse = false) { minimize(!reverse, sender()); }
    void minimize(bool reverse = false, QObject* sender = 0);

signals:
    void outputChosen(live::ObjectPtr, live::ObjectPtr);
    void resized();
    
private:
    Ui::MidiOutputChooser *s_ui;
};

#endif // MIDIOUTPUTCHOOSER_H
