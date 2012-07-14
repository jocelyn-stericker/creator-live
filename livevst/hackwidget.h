#include <QWizardPage>

#ifndef HACKWIDGET_H
#define HACKWIDGET_H

class HackWidget : public QWizardPage
{
    Q_OBJECT
    bool s_complete;
public:
    HackWidget(QWidget* p) : QWizardPage(p), s_complete(0)
    {

    }

    HackWidget() : QWizardPage(), s_complete(0) {}  //otherwise moc gets messed
    bool isComplete() const { return s_complete; }
public slots:
    void setComplete(bool s) {
        s_complete=s;
        emit completeChanged();
    }
};

#endif // HACKWIDGET_H
