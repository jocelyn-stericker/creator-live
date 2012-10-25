/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef SAMPLETAB_H
#define SAMPLETAB_H

#include <QWidget>
#include <live_widgets/bindableparent.h>

namespace Ui {
class sampletab;
}

class SampleTab : public QWidget, public live_widgets::BindableParent
{
    Q_OBJECT
    
public:
    explicit SampleTab(QWidget *parent = 0);
    ~SampleTab();
    
private:
    Ui::sampletab *ui;
};

#endif // SAMPLETAB_H
