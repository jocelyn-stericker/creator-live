#include "sampletab.h"
#include "ui_sampletab.h"

sampletab::sampletab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sampletab)
{
    ui->setupUi(this);
}

sampletab::~sampletab()
{
    delete ui;
}
