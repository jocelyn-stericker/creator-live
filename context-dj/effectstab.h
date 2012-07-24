/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef EFFECTSTAB_H
#define EFFECTSTAB_H

#include <QWidget>

namespace Ui {
class effectstab;
}

class EffectsTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit EffectsTab(QWidget *parent = 0);
    ~EffectsTab();
    
private:
    Ui::effectstab *ui;
};

#endif // EFFECTSTAB_H
