/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

#ifndef VST_H
#define VST_H

#include "live/object"
#include <QMutex>
#include <QDir>

class VstR;

class Vst : public QObject, public live::Object
{
public:
    LIVE_HYBRID
    LIVE_EFFECT
    static int lastVstId;
    static QList<Vst*> s_vst;
    static QMultiMap<QString, QPair<QString, live::ObjectPtr> > s_map;
    static QStringList s_vstpaths_linux;
    static QStringList getVstPaths();
    static QStringList* s_vstCache;

protected:
    int vstId;         /*003*/
    float* buffer[32];  /*N/A*/
    float* incache[2];  /*N/A*/
    QList<live::Connection> connections;
public:
    VstR* rep;          /*N/A*/
    bool mOn() const{ return 1; }
    bool aOn() const { return 1; }
    bool ok() const;
    friend class VstSidekick;
    QString filename, instname;   /*CONSTRUCT*/

    // Path is a valid path, and points to a valid win32 vst
    Vst( QString path, QString instance )
      : live::Object("VST("+path+')',false,false,2)
      , vstId(++lastVstId)
      , rep(0)
      , filename(path)
      , instname(instance)
    {
        setTemporary(0);
        init();
        s_vst.push_back(this);
        s_map.insertMulti(path, qMakePair(instance,live::ObjectPtr(this)));
    }

    virtual ~Vst();

    int getId() const { return vstId; }

    QObject* qoThis() { return this; }

    void init();

    void show();
    void hide();

    void aIn(const float *data, int chan, live::Object*p);
    void mIn(const live::Event *data, live::ObjectChain* p);

#ifdef _WIN32
    float** channelData;
    float** outData;
    QList<live::ObjectPtr> lasts;
    void PROC_VST();
#endif
public:
    static Vst* load(const QByteArray&);
    QByteArray save();
};

#endif // VST_H
