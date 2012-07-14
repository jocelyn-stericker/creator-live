/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#ifndef VSTPATHCHOOSER_H
#define VSTPATHCHOOSER_H

#include <QWidget>

namespace Ui {
class VstSettingsWidget;
}

class VstSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VstSettingsWidget(QWidget *parent = 0);
    ~VstSettingsWidget();

private:
    Ui::VstSettingsWidget *ui;

public slots:
    void addPathAction();
    void addSingleAction(QStringList l=QStringList());
    void deleteAction();
    void doneAction();
    void indexChangedLogic();

signals:
    void done(QStringList vsts);

};

#endif // VSTPATHCHOOSER_H
