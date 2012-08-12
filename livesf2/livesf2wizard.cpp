#include "livesf2wizard.h"
#include "ui_livesf2wizard.h"

LiveSF2Wizard::LiveSF2Wizard()
    : QWizard()
    , ui(new Ui::LiveSF2Wizard)
{
    ui->setupUi(this);
}
