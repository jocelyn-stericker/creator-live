#include "audiosettingswidget.h"
#include "ui_audiosettingswidget.h"

AudioSettingsWidget::AudioSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioSettingsWidget)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(launch()));
}

void AudioSettingsWidget::setASIOs(QStringList s)
{
    ui->asioDevice_comboBox->clear();
    ui->asioDevice_comboBox->insertItems(0,s);
    ui->asioDevice_comboBox->setEnabled(s.size());
}

void AudioSettingsWidget::setFP(int fp)
{
    QString s = QString::number(fp);
    for(int i=0;i<ui->fp_comboBox->count();i++) {
        if(ui->fp_comboBox->itemText(i).startsWith(s)) ui->fp_comboBox->setCurrentIndex(i);
    }
}

void AudioSettingsWidget::setFR(int fr)
{
    QString s = QString::number(fr);
    for(int i=0;i<ui->sr_comboBox->count();i++) {
        if(ui->sr_comboBox->itemText(i).startsWith(s)) ui->sr_comboBox->setCurrentIndex(i);
    }

}

void AudioSettingsWidget::setMode(int mode)
{
    ui->devType_comboBox->setCurrentIndex(mode);
}

void AudioSettingsWidget::launch()
{
    if(!ui->asioDevice_comboBox->count()) return;
    QString a=ui->fp_comboBox->currentText(),b=ui->sr_comboBox->currentText();
    int c=ui->devType_comboBox->currentIndex();
    if(a.contains(' ')) a.truncate(a.indexOf(' '));
    if(b.contains(' ')) b.truncate(b.indexOf(' '));
    emit launchRequested(ui->asioDevice_comboBox->currentText(),a.toInt(),b.toInt(),c,ui->dontAsk->isChecked());
}

AudioSettingsWidget::~AudioSettingsWidget()
{
    delete ui;
}
