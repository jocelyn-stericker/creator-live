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
    
    QList<live::ObjectPtr> m_midiIns;
    QList<live::ObjectPtr> m_returns;
    QList<live::ObjectPtr> m_aOuts;

    live::ObjectPtr m_midiIn;
    live::ObjectPtr m_return;
    live::ObjectPtr m_aOut;

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
