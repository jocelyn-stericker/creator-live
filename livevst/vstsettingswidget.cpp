/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#include "vstsettingswidget.h"
#include "ui_vstsettingswidget.h"
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDirIterator>
#include "vst.h"

VstSettingsWidget::VstSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VstSettingsWidget)
{
    ui->setupUi(this);
    connect(ui->pushButton_addPath,SIGNAL(clicked()),this,SLOT(addPathAction()));
    connect(ui->pushButton_addSingle,SIGNAL(clicked()),this,SLOT(addSingleAction()));
    connect(ui->pushButton_del,SIGNAL(clicked()),this,SLOT(deleteAction()));
    connect(ui->listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(indexChangedLogic()));
    connect(ui->pushButton_close,SIGNAL(clicked()),this,SLOT(doneAction()));
}

void VstSettingsWidget::addPathAction()
{
    QString l=QFileDialog::getExistingDirectory(this,"Add Path","Select the path you wish to load");
    if (!l.isEmpty()) {
        QDirIterator i(l,QDirIterator::Subdirectories);
        QStringList v;
        while (i.hasNext()) {
            QString x=i.next();
            if (x.endsWith(".dll",Qt::CaseInsensitive)) v.push_back(x);
        }
        addSingleAction(v);
    }
}

void VstSettingsWidget::addSingleAction(QStringList l)
{
    if (l.isEmpty()) l=QFileDialog::getOpenFileNames(this,"Add VSTs","Select the vsts you wish to load");

    // In Windows-land, we can verify thingies
#ifdef _WIN32
    for (int i=0;i<l.size();i++) {
        if (!SecretVst::isValid(l[i])) {
            QMessageBox::warning(this,"Not a valid VST",l+" is not a valid VST. Note that only 32-bit VSTs are supported at this time.");
            l.removeAt(i--);
        }
    }
#endif
    for (int i=0;i<l.size();i++) {
        if (!l[i].isEmpty()) ui->listWidget->addItem(l[i]);
    }
}

void VstSettingsWidget::deleteAction()
{
    Q_ASSERT(ui->listWidget->currentRow()!=-1);
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void VstSettingsWidget::indexChangedLogic()
{
    ui->listWidget->setEnabled(ui->listWidget->currentRow()!=-1);
}

void VstSettingsWidget::doneAction()
{
    QStringList ret;
    for (int i=0;i<ui->listWidget->count();i++) {
        ret.push_back(ui->listWidget->item(i)->text());
    }
    QSettings("Nettek", "VSTi plugin for Creator Live").setValue("vstis",ret);
    qDebug()<<"!!"<<ret;
    emit done(ret);
    deleteLater();
}

VstSettingsWidget::~VstSettingsWidget()
{
    delete ui;
}
