#ifndef LIVESF2WIZARD_H
#define LIVESF2WIZARD_H

#include "livesf2_global.h"

#include <QWizard>

namespace Ui {
    class LiveSF2Wizard;
}

class LIVESF2SHARED_EXPORT LiveSF2Wizard : public QWizard
{
    Q_OBJECT
public:
    LiveSF2Wizard();

private:
    Ui::LiveSF2Wizard* ui;

};

#endif // LIVESF2WIZARD_H
