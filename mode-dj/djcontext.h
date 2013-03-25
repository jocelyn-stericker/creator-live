/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef DJCONTEXT_H
#define DJCONTEXT_H

#include <live/object>
#include <live/variantbinding>
#include <live/modeinterface>
#include <live_widgets/bindableparent.h>

#include <QIcon>
#include <QWidget>

class EffectsTab;
class SampleTab;
class TrackTab;
class CollectionContext;

namespace Ui {
class Ui_DJContext;
}

class LiveBar;

class DJContext : public QWidget, public live_widgets::BindableParent
{
    Q_OBJECT

    EffectsTab* m_et;
    SampleTab* m_st;
    TrackTab* m_tt;
    LiveBar* m_lb;
    CollectionContext* m_cc;

    live::ObjectPtr m_out;

public:
    explicit DJContext (QWidget *parent = 0);
    ~DJContext();

public slots:
    void showSnarky();
    void showEffects();
    void showSample();
    void showTrack();
    void showCollection();

    void audioanged_logic(QString);
    void monitorChanged_logic(QString);

private:
    Ui::Ui_DJContext *ui;
};

class DJModeInterface : public QObject, public live::ModeInterface
{
    Q_OBJECT
    Q_INTERFACES(live::ModeInterface)
    Q_PLUGIN_METADATA(IID "ca.nettek.live.dj")
public:
    virtual QString name() { return "DJ set"; }
    virtual QString description() { return "Everything you need to be a DJ"; }
    virtual QWidget* create() { return new DJContext; }
    virtual QWidget* load(const QByteArray&) { return new DJContext; }
    virtual QWidget* livebarExtFor(QWidget* parent) { return new QWidget(parent); }
    virtual QIcon icon() { return QIcon(":/icons/aud.png"); }
    virtual ModeInterface* next() { return 0; }
};

#endif // DJCONTEXT_H
