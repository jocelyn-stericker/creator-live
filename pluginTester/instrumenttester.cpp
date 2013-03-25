/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "instrumenttester.h"
#include "ui_instrumenttester.h"

using namespace live;

InstrumentTester::InstrumentTester(live::ObjectPtr in, live::InstrumentInterface *inst, QWidget *parent) :
    QWidget(parent),
    m_inst(inst),
    m_in(in),
    m_sett(0),
    ui(new Ui::InstrumentTester)
{
    ui->setupUi(this);
    connect(ui->radioButton_select,SIGNAL(toggled(bool)),this,SLOT(configAction(bool)));
    connect(ui->radioButton_settings,SIGNAL(toggled(bool)),this,SLOT(setupAction(bool)));
    ui->widget_configuration->setLayout(new QVBoxLayout);
}

InstrumentTester::~InstrumentTester()
{
    delete ui;
}

void InstrumentTester::configAction(bool a)
{
    if(!a) return;

    if(m_sett) {
        m_sett->deleteLater();
        m_sett=0;
    }

    (m_sett=qobject_cast<QWidget*>(m_inst->selectionWidget(m_instObj,m_out)))->show();
    ui->widget_configuration->layout()->addWidget(m_sett);
    connect(m_sett,SIGNAL(instrumentUpdated(live::ObjectPtr, live::ObjectPtr)),this,SLOT(changeInst(live::ObjectPtr, live::ObjectPtr)));
}

void InstrumentTester::setupAction(bool a)
{
    if(!a) return;
    if(m_sett) {
        m_sett->deleteLater();
        m_sett=0;
    }
    (m_sett=qobject_cast<QWidget*>(m_inst->settingsWidget()))->show();
    ui->widget_configuration->layout()->addWidget(m_sett);
}

void InstrumentTester::changeInst(ObjectPtr inst, ObjectPtr loop)
{
    qDebug() << "CHANGE_INST";
    for (int i = 0; i < m_connections.size(); ++i) {
        if (m_connections[i].a==m_in && m_connections[i].b == m_instObj)
            m_connections.removeAt(i--);
        if (m_connections[i].a==m_instObj && m_connections[i].b == m_out)
            m_connections.removeAt(i--);
    }
    m_instObj=inst;
    m_out=loop;
    if(m_in.valid()&&m_instObj.valid()) m_connections.push_back(Connection(m_in,m_instObj,live::MidiConnection));
    if(m_instObj.valid()&&m_out.valid()) m_connections.push_back(Connection(m_instObj, m_out,live::HybridConnection));
}
