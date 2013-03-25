/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#ifndef VSTSETTINGSWIDGET_H
#define VSTSETTINGSWIDGET_H

#include <QWidget>
#include <live/object>

namespace Ui {
class VstSelectionWidget;
}

class VstSelectionWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit VstSelectionWidget(live::ObjectPtr m_out, live::ObjectPtr m_loopback, QWidget *parent = 0);
    ~VstSelectionWidget();
public slots:
    void showEvent(QShowEvent *e=0);
    
private:
    live::ObjectPtr m_out, m_loopback;
    Ui::VstSelectionWidget *ui;

signals:
    void instrumentUpdated(live::ObjectPtr m_out, live::ObjectPtr m_loopback);  /*!!!*/

public slots:
    void vstChangedLogic();
    void instanceChangedLogic(int);
    void loopbackChangedLogic(int);
    void chanChangedLogic(int);
    void clickedLogic();
};

#endif // VSTSETTINGSWIDGET_H
