/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef INSTRUMENTTESTER_H
#define INSTRUMENTTESTER_H

#include <QWidget>
#include "live/instrumentinterface.h"

namespace Ui {
class InstrumentTester;
}

class InstrumentTester : public QWidget
{
    Q_OBJECT
    live::InstrumentInterface* s_inst;
    live::ObjectPtr s_in, s_instObj, s_out;
    QWidget* s_sett;

public:
    explicit InstrumentTester(live::ObjectPtr in,live::InstrumentInterface* inst,QWidget *parent = 0);
    ~InstrumentTester();

public slots:
    void configAction(bool a);
    void setupAction(bool a);
    void changeInst(live::ObjectPtr, live::ObjectPtr);
    
private:
    Ui::InstrumentTester *ui;
};

#endif // INSTRUMENTTESTER_H
