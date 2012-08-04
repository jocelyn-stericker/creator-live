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
    explicit VstSelectionWidget(live::ObjectPtr s_out, live::ObjectPtr s_loopback, QWidget *parent = 0);
    ~VstSelectionWidget();
public slots:
    void showEvent(QShowEvent *e=0);
    
private:
    live::ObjectPtr s_out, s_loopback;
    Ui::VstSelectionWidget *ui;

signals:
    void instrumentUpdated(live::ObjectPtr s_out, live::ObjectPtr s_loopback);  /*!!!*/

public slots:
    void vstChangedLogic();
    void instanceChangedLogic(int);
    void loopbackChangedLogic(int);
    void chanChangedLogic(int);
    void clickedLogic();
};

#endif // VSTSETTINGSWIDGET_H
