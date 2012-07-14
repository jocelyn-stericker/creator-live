#include "midiwizard.h"
#include "ui_midiwizard.h"

MidiWizard::MidiWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MidiWizard)
{
    ui->setupUi(this);
}

MidiWizard::~MidiWizard()
{
    delete ui;
}
