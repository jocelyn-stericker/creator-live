#include "livebar.h"
#include "ui_livebar.h"

LiveBar::LiveBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiveBar)
{
    ui->setupUi(this);
    connect(ui->comboBox_mode,SIGNAL(activated(int)),this,SLOT(modeChosen(int)));
}

LiveBar::~LiveBar()
{
    delete ui;
}

void LiveBar::modeChosen(int z)
{
    switch(z)
    {
    case 1:
        emit djMode();
        break;
    case 2:
        emit trackMode();
        break;
    case 3:
        emit sampleMode();
        break;
    }
}
