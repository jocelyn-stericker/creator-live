/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef VSTEFFECT_H
#define VSTEFFECT_H

#include "src/Pipeline/Vst.h"

class VstEffectApp : public QObject, public LObject
{
    Q_OBJECT
    Vst* s_internal;
    int s_id;               /*003*/
    bool s_isInit;          /*004*/
    QString s_filename;     /*005*/
    static int s_lastId;
public:
    LIVE_HYBRID
    LIVE_EFFECT
    RELOADABLE(VstEffectApp)
    friend class VstEffectFrame;

    explicit VstEffectApp();
    ~VstEffectApp();

signals:

public slots:
    void aIn(const float *data, int chan, ObjectChain*p);
    void mIn(const Event *data, ObjectChain*p);
    void init(QString path);
    void show();
};

#endif // VSTEFFECT_H
