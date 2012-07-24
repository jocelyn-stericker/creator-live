/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef DJCONTEXT_H
#define DJCONTEXT_H

#include <QWidget>

#include "effectstab.h"
#include "sampletab.h"
#include "tracktab.h"
#include "collectioncontext.h"

namespace Ui {
class Ui_DJContext;
}

class LiveBar;

class DJContext : public QWidget
{
    Q_OBJECT

    EffectsTab* s_et;
    SampleTab* s_st;
    TrackTab* s_tt;
    LiveBar* s_lb;
    CollectionContext* s_cc;

public:
    explicit DJContext (QWidget *parent = 0);
    ~DJContext();

public slots:
    void showSnarky();
    void showEffects();
    void showSample();
    void showTrack();
    void showCollection();

private:
    Ui::Ui_DJContext *ui;
};

#endif // DJCONTEXT_H
