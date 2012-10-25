/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef EFFECTSTAB_H
#define EFFECTSTAB_H

#include <QWidget>
#include <live_widgets/bindableparent.h>

namespace Ui {
class effectstab;
}
class SamplerDJ;

class EffectsTab : public QWidget, public live_widgets::BindableParent
{
    Q_OBJECT

    SamplerDJ& s_samplerL;
    SamplerDJ& s_samplerR;
    int s_times[8][2];
    int s_activeButton[2];
    int s_rec[8][2];
    
public:
    explicit EffectsTab(QWidget *parent = 0);
    ~EffectsTab();

public slots:
    void buttonLogic();
    
private:
    Ui::effectstab *ui;
};

#endif // EFFECTSTAB_H
