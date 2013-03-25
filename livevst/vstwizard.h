#ifndef VSTWIZARD_H
#define VSTWIZARD_H

#include "hackwidget.h"
#include <QStackedWidget>
#include <QWizardPage>
#include <live/object>

namespace Ui {
class VstWizard;
}

class VstWizard : public QStackedWidget
{
    Q_OBJECT
    
private:
    live::ObjectPtr m_out, m_loopback;
    live::ObjectPtr m_vst;

public:
    explicit VstWizard(live::ObjectPtr out, live::ObjectPtr loopback, QWidget *parent);
    ~VstWizard();

public slots:
    void vstChanged(int);
    void pageChangeEvent(int);
    void finishUp();
    void stockChangedEvent();

    void next();
    void back();
    void restart();

signals:
    void instrumentUpdated(live::ObjectPtr m_out, live::ObjectPtr m_loopback);  /*!!!*/

private:
    Ui::VstWizard *ui;
};

#endif // VSTWIZARD_H
