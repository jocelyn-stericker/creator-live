#include <QWizardPage>

#ifndef HACKWIDGET_H
#define HACKWIDGET_H

class HackWidget : public QWizardPage
{
    Q_OBJECT
    bool m_complete;
public:
    HackWidget(QWidget* p) : QWizardPage(p), m_complete(0)
    {

    }

    HackWidget() : QWizardPage(), m_complete(0) {}  //otherwise moc gets messed
    bool isComplete() const { return m_complete; }
public slots:
    void setComplete(bool s) {
        m_complete=s;
        emit completeChanged();
    }
};

#endif // HACKWIDGET_H
