/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( SF2 PLUGIN ) ) )

*******************************************************/

#ifndef LIVESF2WIZARD_H
#define LIVESF2WIZARD_H

#include "livesf2_global.h"

#include <live/object>

#include <QStackedWidget>

namespace Ui {
    class LiveSF2Wizard;
}

class LIVESF2SHARED_EXPORT LiveSF2Wizard : public QStackedWidget
{
    Q_OBJECT
public:
    LiveSF2Wizard();

    live::ObjectPtr s_out, s_loopback;
    QList<live::ObjectPtr> s_loopbacks;

signals:
    void instrumentUpdated(live::ObjectPtr s_out, live::ObjectPtr s_loopback);  /*!!!*/

public slots:
    void updateLoopbacks();
    void next();
    void prev();

private:
    Ui::LiveSF2Wizard* ui;

};

#endif // LIVESF2WIZARD_H
