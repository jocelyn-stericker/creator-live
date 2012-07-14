/*****************************************************

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "settingslinux.h"
#include "ui_settingslinux.h"
#include "live_widgets/dropframe.h"
#include <QSettings>
#include "live_widgets/iodragwidget.h"
#include "live/audio.h"

#include <QLineEdit>
using namespace live;

bool LiveAudioSettingsWidget::started=0;
QString LiveAudioSettingsWidget::s_in,LiveAudioSettingsWidget::s_out;
QStringList LiveAudioSettingsWidget::s_inl, LiveAudioSettingsWidget::s_outl;

LiveAudioSettingsWidget::LiveAudioSettingsWidget(QWidget *parent, bool automatic) :
    QDialog(parent),
    ui(new Ui::SettingsLinux),
    s_automatic(automatic)
{
    setModal(true);
    if(!automatic)
    {
        // CHECK
        audio::resetMappings();
    }
    QString labelstr;
    {
        labelstr="Audio inputs are: ";
        s_inl=audio::getInputChanStringList();
        s_inl.replaceInStrings(":","_");
        for(int i=0;i<s_inl;i++)
        {
            if(i)
            {
                s_in+=",";
            }
            s_in+="\""+s_inl[i]+"\" ";
        }
        labelstr+=s_in+"\n";
    }
    {
        s_outl=audio::getOutputChanStringList();
        for(int i=0;i<s_outl;i++)
        {
            if(i)
            {
                s_out+=",";
            }
            s_out+="\""+s_outl[i]+"\" ";
        }
        labelstr+=s_out;
    }
    if(!automatic)
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
    for(int i=0;i<ui->tabWidget->inWidget()->groupNames.size();i++) {
        if(names.contains(ui->tabWidget->inWidget()->groupNames[i]->text())) {
            ok=0; break;
        }
        names.insert(ui->tabWidget->inWidget()->groupNames[i]->text());
    }
    if(ok) for(int i=0;i<ui->tabWidget->outWidget()->groupNames.size();i++) {
        if(names.contains(ui->tabWidget->outWidget()->groupNames[i]->text())) {
            ok=0; break;
        }
        names.insert(ui->tabWidget->outWidget()->groupNames[i]->text());
    }
    if(!ok) {
        ui->label_status->setText("NAMES ARE NOT UNIQUE!");
        ui->pushButton->setEnabled(0);
    }
    if(ok) {
        ui->label_status->setText("Ok.");
        ui->pushButton->setEnabled(1);
    }
}

void LiveAudioSettingsWidget::applyAndQuit()
{
    audio::resetMappings();
    for(int i=0;i<ui->tabWidget->inWidget()->groupNames.size();i++)
    {
        QStringList mapping;
        if(ui->tabWidget->inWidget()->dropFrameAs[i]->text().size())
        {
            mapping<<ui->tabWidget->inWidget()->dropFrameAs[i]->text();
        }
        if(ui->tabWidget->inWidget()->dropFrameBs[i]->text().size())
        {
            mapping<<ui->tabWidget->inWidget()->dropFrameBs[i]->text();
        }
        if(mapping.size())
        {
            audio::addMapping(mapping,1,ui->tabWidget->inWidget()->groupNames[i]->text());
            s_inputMappings<<mapping;
            s_inputMappingNames<<ui->tabWidget->inWidget()->groupNames[i]->text();
        }
    }
    for(int i=0;i<ui->tabWidget->outWidget()->groupNames.size();i++)
    {
        QStringList mapping;
        if(ui->tabWidget->outWidget()->dropFrameAs[i]->text().size())
        {
            mapping<<ui->tabWidget->outWidget()->dropFrameAs[i]->text();
        }
        if(ui->tabWidget->outWidget()->dropFrameBs[i]->text().size())
        {
            mapping<<ui->tabWidget->outWidget()->dropFrameBs[i]->text();
        }
        if(mapping.size())
        {
            audio::addMapping(mapping,0,ui->tabWidget->outWidget()->groupNames[i]->text());
            s_outputMappings<<mapping;
            s_outputMappingNames<<ui->tabWidget->outWidget()->groupNames[i]->text();
        }
    }
    audio::refresh();

    QSettings settings;
    settings.setValue("audio_inputMappingNames",s_inputMappingNames);
    settings.setValue("audio_inputMappings",s_inputMappings);
    settings.setValue("audio_outputMappingNames",s_outputMappingNames);
    settings.setValue("audio_outputMappings",s_outputMappings);
    deleteLater();
}

bool LiveAudioSettingsWidget::apply()
{

    QSettings settings;
    QList<QVariant> inMap=settings.value("audio_inputMappings").toList();
    QList<QVariant> outMap=settings.value("audio_outputMappings").toList();


    audio::resetMappings();
    for(int i=0; i<inMap; i++)
    {
        Q_ASSERT(inMap[i].toStringList().size());
        audio::addMapping(inMap[i].toStringList(),1,settings.value("audio_inputMappingNames").toStringList()[i]);
    }

    for(int i=0; i<outMap; i++)
    {
        Q_ASSERT(outMap[i].toStringList().size());
        audio::addMapping(outMap[i].toStringList(),0,settings.value("audio_outputMappingNames").toStringList()[i]);
    }
    audio::refresh();

    return 1;
}
