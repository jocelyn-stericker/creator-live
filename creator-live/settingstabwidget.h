/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SETTINGSTABWIDGET_H
#define SETTINGSTABWIDGET_H

#include <QTabWidget>
#include <live_widgets/bindableparent.h>

namespace live_widgets {
class InputDragWidget;
class OutputDragWidget;
}

namespace Ui {
    class SettingsTabWidget;
}

class SettingsTabWidget : public QTabWidget, public live_widgets::BindableParent
{
    Q_OBJECT

public:
    explicit SettingsTabWidget(QWidget *parent = 0);
    ~SettingsTabWidget();
    live_widgets::InputDragWidget* inWidget();
    live_widgets::OutputDragWidget* outWidget();

public slots:
    void addVst();
    void delVst();

signals:
    void modified();

private:
    Ui::SettingsTabWidget *ui;
    SettingsTabWidget(const SettingsTabWidget&);
    SettingsTabWidget& operator=(const SettingsTabWidget&);
};

#endif // SETTINGSTABWIDGET_H
