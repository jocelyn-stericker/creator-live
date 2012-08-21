/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                ( ( ( MIDI PLUGIN ) ) )

*******************************************************/

#ifndef MIDIWIZARD_H
#define MIDIWIZARD_H

#include <QStackedWidget>
#include <live/object>

namespace Ui {
class MidiWizard;
}

class MidiWizard : public QStackedWidget
{
    Q_OBJECT
    
    QList<live::ObjectPtr> s_midiIns;
    QList<live::ObjectPtr> s_returns;
    QList<live::ObjectPtr> s_aOuts;

    live::ObjectPtr s_midiIn;
    live::ObjectPtr s_return;
    live::ObjectPtr s_aOut;

public:
    explicit MidiWizard(QWidget *parent = 0);
    ~MidiWizard();
    
public slots:
    void next();
    void prev();
    void updateDevices();

signals:
    void instrumentUpdated(live::ObjectPtr m, live::ObjectPtr a);

private:
    Ui::MidiWizard *ui;
};

#endif // MIDIWIZARD_H
