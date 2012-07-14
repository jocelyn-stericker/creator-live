/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef ANABEAT_H
#define ANABEAT_H

#include "live/object.h"
#include "live/timesignature.h"
#include "live/metronome.h"

namespace live {

class AbEv;
class AbBeat;

class LIBLIVECORESHARED_EXPORT AnaBeat : public QObject, public live::Object
{
    Q_OBJECT

public:
    LIVE_MIDI
    LIVE_INPUT
    bool aOn() const {return 0;}
    bool mOn() const{return 1;}
    bool _started;
    int _bpm;
    int _bar;
    double _beatAprox;
    TimeSignature _ts;
    QList< AbEv* > _unplacedEvents;
    QList< AbBeat* > _unplacedBeats;
    QList< AbEv* > _allEvents;
    QList< Metronome* > _giveHintTo;
    int _i;

    explicit AnaBeat() :
        Object("MIDI Beat Analyzer",false,false),
        _started(0),
        _bpm(-1),
        _bar(-1),
        _beatAprox(-1),
        _ts(TimeSignature(4,4)),
        _unplacedEvents(),
        _unplacedBeats(),
        _allEvents(),
        _giveHintTo(),
        _i(-1)
    {

    }

public slots:
    AnaBeat* start()
    {
        _started = 1;
        return this;
    }

    void stop()
    {
        _started = 0;
    }

    void mIn(const Event *data, ObjectChain&p);

public:
    TimeSignature predictedTS()
    {
        _updatePredictions();
        return _ts;
    }

    // Returns -1 upon not enough info
    int predictedBPM()
    {
        _updatePredictions();
        return _bpm;
    }

    /*int predictedCurrentBar()
    {
        _updatePredictions();
        return _bar;
    }

    double predictedCurrentBeatAprox()
    {
        _updatePredictions();
        return _beatAprox;
    }

    TimeSignature predictedTS( int bar );

    int predictedBPM( int bar );*/

signals:
    void timePrediction( const SyncData& );

private:
    void _updatePredictions();

    bool _sortBeats();
    bool _makeLists( QList< double >& x_, QList< double >& y_, QList< double >& velocity );
    double* _rotate( QList< double >& x_, QList< double >& y_ );
    int* _makeHistogram( int& bins, double& highest, double& lowest, QList< double >& y );
    bool _correctRatios( int& noteID, QList< double >& last, int& bins, int* count, double &lowest, double &highest,
                         QList< double >& y_, QList< double >& yuncorrected, QList< double >& velocity, QList< double >& y );
    QList< double > _findBeatCandidates( QList< double >& last, QList< double >& y_, QList< double >& newX );
    double _findTruePulse( QList< double >& beatCandidats, QList< double >& newX );
    bool _findNum( int& oldFactor, int& factor, int& numLCD, QList< double >& newX, double& highestBC,
                   QList< double >& velocity);
public:
    static AnaBeat* load(const QByteArray&str);
    QByteArray save();
};

}

#endif // ANABEAT_H
