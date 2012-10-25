/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/
#ifndef LIVEBAR_H
#define LIVEBAR_H

#include <QWidget>
#include <live_widgets/bindableparent.h>

namespace Ui {
class LiveBar;
}

class LiveBar : public QWidget, public live_widgets::BindableParent
{
    Q_OBJECT
    
public:
    explicit LiveBar(QWidget *parent = 0);
    ~LiveBar();
    
public slots:
    void modeChosen(int z);

signals:
    void noMode();
    void djMode();
    void trackMode();
    void sampleMode();
    void collectionMode();

private:
    Ui::LiveBar *ui;
};

#endif // LIVEBAR_H
