/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "settingstabwidget.h"
#include "ui_settingstabwidget.h"

#include <QFileDialog>
#include <QSettings>

SettingsTabWidget::SettingsTabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::SettingsTabWidget)
{
    ui->setupUi(this);
    setCurrentIndex(0);
//    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(addVst()));
//    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(delVst()));
//    for (int i=0;i<Vst::s_vstpaths_linux.size();i++)
//    {
//        ui->listWidget->addItem(Vst::s_vstpaths_linux[i]);
//    }
    connect(inWidget(),SIGNAL(modified()),this,SIGNAL(modified()));
    connect(outWidget(),SIGNAL(modified()),this,SIGNAL(modified()));
}

SettingsTabWidget::~SettingsTabWidget()
{
    QStringList vsts;
//    for (int i=0;i<ui->listWidget->count();i++)
//    {
//        vsts+=ui->listWidget->item(i)->text();
//    }

//    QSettings settings;
//    settings.setValue("vstis",vsts);

    // FIXME!
//    if (liveApp&&liveApp->ribbon())
//    {
//        liveApp->ribbon()->reactOnVstChange();
//    }

    delete ui;
}

live_widgets::InputDragWidget* SettingsTabWidget::inWidget()
{
    return ui->frame_2;
}

live_widgets::OutputDragWidget* SettingsTabWidget::outWidget()
{
    return ui->frame;
}

void SettingsTabWidget::addVst()
{
//    QStringList x=QFileDialog::getOpenFileNames(0,"Add VSTs",QString(),"*.dll");
//    for (int i=0;i<x.size();i++)
//    {
//        if (!Vst::s_vstpaths_linux.contains(x[i]))
//        {
//            Vst::s_vstpaths_linux.push_back(x[i]);
//            ui->listWidget->addItem(x[i]);
//        }
//    }
}

void SettingsTabWidget::delVst()
{
//    if (ui->listWidget->currentIndex().row()==-1)
//    {
//        return;
//    }
//    QString text=ui->listWidget->currentItem()->text();
//    for (int i=0;i<Vst::s_vstpaths_linux.size();i++)
//    {
//        if (Vst::s_vstpaths_linux[i]==text)
//        {
//            Vst::s_vstpaths_linux.removeAt(i--);
//        }
//    }
}
