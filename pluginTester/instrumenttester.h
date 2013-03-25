/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef INSTRUMENTTESTER_H
#define INSTRUMENTTESTER_H

#include <QWidget>
#include "live/instrumentinterface"

namespace Ui {
class InstrumentTester;
}

class InstrumentTester : public QWidget
{
    Q_OBJECT
    live::InstrumentInterface* m_inst;
    live::ObjectPtr m_in, m_instObj, m_out;
    QWidget* m_sett;
    QList<live::Connection> m_connections;

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
