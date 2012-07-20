#include "effectstab.h"
#include "ui_effectstab.h"

effectstab::effectstab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::effectstab)
{
    ui->setupUi(this);
}

effectstab::~effectstab()
{
    delete ui;
}
