/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef DJCONTEXT_H
#define DJCONTEXT_H

#include "live/object.h"
#include "live/variantbinding.h"

#include <QWidget>

class EffectsTab;
class SampleTab;
class TrackTab;
class CollectionContext;

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

    live::Watched< live::ObjectPtr > s_out;

public:
    explicit DJContext (QWidget *parent = 0);
    ~DJContext();

public slots:
    void showSnarky();
    void showEffects();
    void showSample();
    void showTrack();
    void showCollection();

    void audioChanged_logic(QString);
    void monitorChanged_logic(QString);

private:
    Ui::Ui_DJContext *ui;
};

#endif // DJCONTEXT_H
