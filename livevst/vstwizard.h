#ifndef VSTWIZARD_H
#define VSTWIZARD_H

#include "hackwidget.h"
#include <QWizard>
#include <QWizardPage>
#include <live/object>

namespace Ui {
class VstWizard;
}

class VstWizard : public QWizard
{
    Q_OBJECT
    
private:
    live::ObjectPtr s_out, s_loopback;
    live::ObjectPtr s_vst;

public:
    explicit VstWizard(live::ObjectPtr out, live::ObjectPtr loopback, QWidget *parent);
    ~VstWizard();

public slots:
    void vstChanged(int);
    void pageChangeEvent(int);
    void finishUp();
    void stockChangedEvent();

signals:
    void instrumentUpdated(live::ObjectPtr s_out, live::ObjectPtr s_loopback);  /*!!!*/

private:
    Ui::VstWizard *ui;
};

#endif // VSTWIZARD_H
