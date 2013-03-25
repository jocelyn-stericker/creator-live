/*****************************************************
                                           rev. 110720

  Part of the Creator Live Music Workstation Project
  Copyright Joshua Netterfield  (trintith@gmail.com)

                 All rights reserved.
*****************************************************/

#include "src/Apps/Vst/VstEffectFrame.h"
#include "ui_VstEffectFrame.h"

VstEffectCreator VstEffectCreator::evil;

VstEffectFrame::VstEffectFrame(VstEffectApp* capp, QWidget *parent) :
    AppFrame(parent),
   m_app(capp),
    ui(new Ui::VstEffectFrame)
{
    ui->setupUi(this);
    ui->comboBox_vstSelect->addItems(Vst::getVstPaths());
    connect(ui->pushButton_show,SIGNAL(clicked()),s_app,SLOT(show()));
    ui->pushButton_show->hide();
    connect(ui->comboBox_vstSelect,SIGNAL(currentIndexChanged(QString)),this,SLOT(init(QString)));

    setObjectName("VstEffectFrame_"+QString::number(s_app->s_id));
}

void VstEffectFrame::init(QString path)
{
   m_app->init(path);
    ui->comboBox_vstSelect->hide();
    ui->pushButton_show->show();
    ui->label->setText("Init VST...");
}

VstEffectFrame::~VstEffectFrame()
{
    deletem_app;
    delete ui;
}
