/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef APPTESTER_H
#define APPTESTER_H

#include <QWidget>
#include "live/appinterface"
#include "live_widgets/appframe.h"

namespace Ui {
class AppTester;
}

class AppTester : public QWidget
{
    Q_OBJECT
    
    live::ObjectPtr s_app, s_in, s_out;
    QList<live::Connection> s_connections;
    live_widgets::AppFrame* s_frame;
public:
    explicit AppTester(live::AppInterface*a,QWidget *parent = 0);
    ~AppTester();
public slots:
    void settingsChangedEvent();

private:
    Ui::AppTester *ui;
};

#endif // APPTESTER_H
