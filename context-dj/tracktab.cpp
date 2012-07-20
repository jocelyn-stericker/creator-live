#include "tracktab.h"
#include "ui_tracktab.h"

tracktab::tracktab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tracktab)
{
    ui->setupUi(this);
}

tracktab::~tracktab()
{
    delete ui;
}
