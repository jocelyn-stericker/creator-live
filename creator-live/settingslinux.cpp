/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "settingslinux.h"
#include "ui_settingslinux.h"

#include <live/audio>
#include <live_widgets/dropframe.h>
#include <live_widgets/iodragwidget.h>
#include <QLineEdit>
#include <QSettings>

using namespace live;

bool LiveAudioSettingsWidget::started=0;
QString LiveAudioSettingsWidget::m_in,LiveAudioSettingsWidget::m_out;
QStringList LiveAudioSettingsWidget::m_inl, LiveAudioSettingsWidget::m_outl;

LiveAudioSettingsWidget::LiveAudioSettingsWidget(QWidget *parent, bool automatic)
  : QDialog(parent)
  , live_widgets::BindableParent(this)
  , m_inputMappings()
  , m_inputMappingNames()
  , m_outputMappings()
  , m_outputMappingNames()
  , ui(new Ui::SettingsLinux)
  , m_automatic(automatic)
{
    setModal(true);
    if (!automatic)
    {
        // CHECK
        audio::resetMappings();
    }
    QString labelstr;
    {
        labelstr="Audio inputs are: ";
        m_inl=audio::getInputChanStringList();
        m_inl.replaceInStrings(":","_");
        for (int i=0;i<m_inl;i++)
        {
            if (i)
            {
                m_in+=",";
            }
            m_in+="\""+m_inl[i]+"\" ";
        }
        labelstr+=m_in+"\n";
    }
    {
        m_outl=audio::getOutputChanStringList();
        for (int i=0;i<m_outl;i++)
        {
            if (i)
            {
                m_out+=",";
            }
            m_out+="\""+m_outl[i]+"\" ";
        }
        labelstr+=m_out;
    }
    if (!automatic)
    {
        ui->setupUi(this);
        connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(applyAndQuit()));
    }
    started=1;
    connect(ui->tabWidget,SIGNAL(modified()),this,SLOT(verify()));
    verify();
}

LiveAudioSettingsWidget::~LiveAudioSettingsWidget()
{
    delete ui;
}

void LiveAudioSettingsWidget::hideEvent(QHideEvent *)
{
    //CHECK
    audio::resetMappings();
}

void LiveAudioSettingsWidget::verify()
{
    QSet<QString> names;
    bool ok=1;
    for (int i=0;i<ui->tabWidget->inWidget()->groupNames.size();i++) {
        if (names.contains(ui->tabWidget->inWidget()->groupNames[i]->text())) {
            ok=0; break;
        }
        names.insert(ui->tabWidget->inWidget()->groupNames[i]->text());
    }
    if (ok) for (int i=0;i<ui->tabWidget->outWidget()->groupNames.size();i++) {
        if (names.contains(ui->tabWidget->outWidget()->groupNames[i]->text())) {
            ok=0; break;
        }
        names.insert(ui->tabWidget->outWidget()->groupNames[i]->text());
    }
    if (!ok) {
        ui->label_status->setText("NAMES ARE NOT UNIQUE!");
        ui->pushButton->setEnabled(0);
    }
    if (ok) {
        ui->label_status->setText("Ok.");
        ui->pushButton->setEnabled(1);
    }
}

void LiveAudioSettingsWidget::applyAndQuit()
{
    audio::resetMappings();
    for (int i=0;i<ui->tabWidget->inWidget()->groupNames.size();i++)
    {
        QStringList mapping;
        if (ui->tabWidget->inWidget()->dropFrameAs[i]->text().size())
        {
            mapping<<ui->tabWidget->inWidget()->dropFrameAs[i]->text();
        }
        if (ui->tabWidget->inWidget()->dropFrameBs[i]->text().size())
        {
            mapping<<ui->tabWidget->inWidget()->dropFrameBs[i]->text();
        }
        if (mapping.size())
        {
            audio::addMapping(mapping,1,ui->tabWidget->inWidget()->groupNames[i]->text());
            m_inputMappings<<mapping;
            m_inputMappingNames<<ui->tabWidget->inWidget()->groupNames[i]->text();
        }
    }
    for (int i=0;i<ui->tabWidget->outWidget()->groupNames.size();i++)
    {
        QStringList mapping;
        if (ui->tabWidget->outWidget()->dropFrameAs[i]->text().size())
        {
            mapping<<ui->tabWidget->outWidget()->dropFrameAs[i]->text();
        }
        if (ui->tabWidget->outWidget()->dropFrameBs[i]->text().size())
        {
            mapping<<ui->tabWidget->outWidget()->dropFrameBs[i]->text();
        }
        if (mapping.size())
        {
            audio::addMapping(mapping,0,ui->tabWidget->outWidget()->groupNames[i]->text());
            m_outputMappings<<mapping;
            m_outputMappingNames<<ui->tabWidget->outWidget()->groupNames[i]->text();
        }
    }
    audio::refresh();

    QSettings settings;
    settings.setValue("audio_inputMappingNames",m_inputMappingNames);
    settings.setValue("audio_inputMappings",m_inputMappings);
    settings.setValue("audio_outputMappingNames",m_outputMappingNames);
    settings.setValue("audio_outputMappings",m_outputMappings);
    deleteLater();
}

bool LiveAudioSettingsWidget::apply()
{

    QSettings settings;
    QList<QVariant> inMap=settings.value("audio_inputMappings").toList();
    QList<QVariant> outMap=settings.value("audio_outputMappings").toList();


    audio::resetMappings();
    for (int i=0; i<inMap; i++)
    {
        Q_ASSERT(inMap[i].toStringList().size());
        audio::addMapping(inMap[i].toStringList(),1,settings.value("audio_inputMappingNames").toStringList()[i]);
    }

    for (int i=0; i<outMap; i++)
    {
        Q_ASSERT(outMap[i].toStringList().size());
        audio::addMapping(outMap[i].toStringList(),0,settings.value("audio_outputMappingNames").toStringList()[i]);
    }
    audio::refresh();

    return 1;
}
