#include "live_widgets/trackinputwidget.h"
#include "ui_trackinputwidget.h"

namespace live_widgets {

TrackInputWidget::TrackInputWidget(QWidget *parent)
  : QFrame(parent)
  , b_audio(false)
  , ui(new Ui::TrackInputWidget)
{
    ui->setupUi(this);

    connect(&b_trackName, SIGNAL(changeObserved(QString,QString)), ui->inputName, SLOT(setText(QString)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), this, SLOT(onSetAudio(bool)));
    connect(&b_audio, SIGNAL(changeObserved(bool,bool)), ui->inputPaint, SLOT(setHidden(bool)));
}

TrackInputWidget::~TrackInputWidget()
{
    delete ui;
}

void TrackInputWidget::onSetAudio(bool b) {
    qDebug() << "X!";
    if (b) {
        ui->inputType->setIcon(QIcon(":/icons/microphone.png"));
    } else {
        ui->inputType->setIcon(QIcon(":/icons/midi.png"));
    }
}
}
