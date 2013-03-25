/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "apptester.h"
#include "ui_apptester.h"

using namespace live;
using namespace live_widgets;

AppTester::AppTester(live::AppInterface *a, QWidget *parent) :
    QWidget(parent),
    m_app(a->newBackend()),
    m_frame(qobject_cast<AppFrame*>(a->newFrontend(m_app))),
    ui(new Ui::AppTester)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog);
    setWindowTitle("Live AppTest: "+a->name());
    QHBoxLayout* lay=new QHBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    ui->widget_appHolder->setLayout(lay);
    lay->addWidget(m_frame);

    QList<ObjectPtr> ins=object::get(MidiOnly|InputOnly)+object::get(AudioOnly|InputOnly|NoRefresh);
    QStringList insStr;
    for(int i=0;i<ins.size();i++) insStr.push_back(ins[i]->name());
    ui->comboBox_input->addItems(insStr);
    qDebug()<<insStr;

    QList<ObjectPtr> outs=object::get(MidiOnly|OutputOnly)+object::get(AudioOnly|OutputOnly);
    QStringList outStr;
    for(int i=0;i<outs.size();i++) outStr.push_back(outs[i]->name());
    ui->comboBox_output->addItems(outStr);

    connect(ui->comboBox_input,SIGNAL(activated(int)),this,SLOT(settingsChangedEvent()));
    connect(ui->comboBox_output,SIGNAL(activated(int)),this,SLOT(settingsChangedEvent()));
    settingsChangedEvent();

    connect(this,SIGNAL(destroyed()),qApp,SLOT(quit()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(close()));
}

void AppTester::settingsChangedEvent()
{
    for (int i = 0; i < m_connections.size(); ++i) {
        if (m_connections[i].a == m_in && m_connections[i].b == m_app)
            m_connections.removeAt(i--);
        if (m_connections[i].a == m_app && m_connections[i].b == m_out)
            m_connections.removeAt(i--);
    }
    m_in=object::request(ui->comboBox_input->currentText(),InputOnly|NoRefresh);
    m_out=object::request(ui->comboBox_output->currentText(),OutputOnly|NoRefresh);
    m_connections.push_back(Connection(m_in, m_app, live::HybridConnection));
    m_connections.push_back(Connection(m_app, m_out, live::HybridConnection));
}

AppTester::~AppTester()
{
    delete ui;
}
