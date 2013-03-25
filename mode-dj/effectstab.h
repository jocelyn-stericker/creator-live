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

    SamplerDJ& m_samplerL;
    SamplerDJ& m_samplerR;
    int m_times[8][2];
    int m_activeButton[2];
    int m_rec[8][2];
    
public:
    explicit EffectsTab(QWidget *parent = 0);
    ~EffectsTab();

public slots:
    void buttonLogic();
    
private:
    Ui::effectstab *ui;
};

#endif // EFFECTSTAB_H
