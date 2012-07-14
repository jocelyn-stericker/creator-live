/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <iostream>
#include <math.h>
#include <cmath>
#include "live/anabeat.h"
#include "live/midievent.h"

/**
 * A denominator cannot be detected, but numerators have common
 * demonators by convention. For example, one would rarely find 6/4...
 */
int denForNum( int num )
{
    if ( num == 3 ) return 4;
    if ( num == 4 ) return 4;
    if ( num == 5 ) return 4;
    if ( num == 6 ) return 8;
    if ( num == 7 ) return 4;   // Note: 7/8 would probably be detected with 14 num... FIX ME
    // 8 doesn't exist. should be 4
    if ( num == 9 ) return 8;
    // 10 doesn't exist. should be 4
    if ( num == 11 ) return 8;  // Really... that's just getting a bit silly...
    else return -1;
}

/**
 * Accepts a log2 of the difference (which is really a ratio) in timing between two notes.
 * Returns a perfect ratio.
 */
double closestRatio( double aprox )
{
    //std::cerr << aprox;
    QList< double > ratios;
    ratios << 0 << 0.41504 << 0.58496
           << 1 << 1.41504 << 1.58496
           << 2 << 2.41504 << 3.58496
           << 3 << 3.41504 << 3.58496
           << 4 << 4.41504 << 4.58496
           << 5 << 5.41504 << 5.58496
           << 6 << 6.41504 << 6.58496;

    for ( int i = 1; i < ratios.size(); i++ ) if ( ratios[i-1] <= aprox && aprox <= ratios[i] )
        {

            if ( fabs( aprox - ratios[i-1] ) < fabs( aprox - ratios[i] ) ) return fabs( ratios[i-1] );
            else return fabs( ratios[ i ] );

        }
    return -100;
}

///////////////////////////////////////////////

/**
 * Little class which represents an live::Event.
 */
class live::AbEv
{
public:
    AbEv() : velocity( 0 ), time( 0 ), beat( NULL ) {} int velocity;
    double time;
    live::AbBeat* beat;
};

/**
 * live::Events that are close together are fitted into beats.
 * A problem I still have is actually figuring out to fit them into beats acurately (with rolls,etc)...
 */
class live::AbBeat
{
public:
    AbBeat() : beat( 0 ), barNo( 0 ), emp( true ) {}
    int summativeVelocity()
    {
        int vel = 0;
        foreach( live::AbEv* ev, events ) vel += ev->velocity;
        return vel;
    }

    double startTime()
    {
        double time = 9999999;
        foreach( live::AbEv* ev, events ) if ( ev->time < time ) time = ev->time;
        return time;
    }

    double endTime()
    {
        double time = 0;
        foreach( live::AbEv* ev, events ) if ( ev->time > time ) time = ev->time;
        return time;
    }

    double beat;
    QList< live::AbEv* > events;
    int barNo;
    bool emp;
};

///////////////////////////////////////////////

void live::AnaBeat::mIn(const live::Event *data, live::ObjectChain&p)
{
    if ( data->velocity() < 5 ) return;
    AbEv* ev = new AbEv;
    ev->velocity = data->velocity();
    //ev->time = (double)data->time.sec + (((double)data->time.nsec)/1000000000);
    ev->time = (double)data->time.toTime_ms()/1000.0f;
    // FIX ME:: Above may be incorrect.
    _unplacedEvents.append( ev );

    _updatePredictions();
    p.push_back(this);
    mOut(data,p);
    p.pop_back();
}

void live::AnaBeat::_updatePredictions()
{
    qDebug()<<"_up";
    if ( !_sortBeats() ) return;

    QList< double > x_, y_, velocity;   // time, length, summativeVelocities
    if ( !_makeLists( x_, y_, velocity ) ) return;
    qDebug()<<"_up2";

    QList< double > yuncorrected = y_;
    double* highAndLow = _rotate( x_, y_ );
    if ( highAndLow == NULL ) return;
    qDebug()<<"_up3"<<x_<<y_;
    double highest = highAndLow[0];
    double lowest = highAndLow[1];

    int bins = 0;
    QList< double > y = y_;
    qSort( y );
    int* count = _makeHistogram( bins, highest, lowest, y );
    if ( count == NULL ) return;
    qDebug()<<"_up4";

    int noteID = 0;
    QList< double > last;
    if ( !_correctRatios( noteID, last, bins, count, lowest, highest, y_, yuncorrected, velocity, y ) ) return;
    qDebug()<<"_up4b"<<last<<y_;


    QList< double > newX;
    QList< double > beatCandidats = _findBeatCandidates( last, y_, newX );
    if ( beatCandidats.isEmpty() ) return;
    qDebug()<<"_up5";

    double highestBC = _findTruePulse( beatCandidats, newX );
    qDebug() << "PULSE IS " << (double)60/highestBC; // Yay!!! XD


    int oldFactor, factor, numLCD;
    if ( !_findNum( oldFactor, factor, numLCD, newX, highestBC, velocity ) ) return;

    oldFactor = factor;
    if ( numLCD == 2 )
    {
        oldFactor /= 2;    // shouldn't happen. But picking up a 2 SHOULD probably be 4...
        numLCD     = 4;
    }
    if ( factor == 8 )
    {
        numLCD    /= 2;    // 8/8 doesn't exist.
        oldFactor /= 4;
    }

    while ( (double)60/highestBC / oldFactor < 60 ) oldFactor /= 2;
    while ( (double)60/highestBC / oldFactor > 140 ) oldFactor *= 2;

    qCritical() << "Detected tempo at " << (double)60/highestBC / oldFactor << " @ " << numLCD << "/" <<
             denForNum( numLCD ) << " from pulse " << (double)60/highestBC;
    return;

    for(int i=0; i<_giveHintTo; i++)
    {
        int cbpm=_giveHintTo[i]->bpm();
        int tries=0;
        bool ok=1;
        while(qAbs((double)60/highestBC / oldFactor - cbpm)>40)
        {
            oldFactor /= ((double)60/highestBC / oldFactor) < cbpm ? 0.5 : 2.0;
            if(++tries==4)
            {
                ok=0;
                break;
            }
        }
        if(!ok)
        {
            continue;
        }
        _giveHintTo[i]->setBpm(((double)60/highestBC / oldFactor));
    }

    delete [] highAndLow;
    delete [] count;
}

bool live::AnaBeat::_sortBeats()
{
    while ( !_unplacedEvents.isEmpty() )
    {
        if ( _unplacedBeats.isEmpty() || (_unplacedBeats.last()->startTime() - _unplacedEvents.first()->time) < -0.0625 )
        {
            // 0.125 is A SIXTEENTH NOTE at 4/4 120 BPM

            if ( !_unplacedBeats.isEmpty() )
            {
                double a = _unplacedBeats.last()->startTime();
                double b = _unplacedEvents.first()->time;
                a+=0.0625;
                while( a < b )
                {
                    a+=0.0625;
                    //_unplacedBeats.append( new AbBeat );
                }
            }

            _unplacedBeats.append( new AbBeat );
        }
        _unplacedBeats.last()->events.append( _unplacedEvents.takeFirst());
        _unplacedBeats.last()->emp = 0;
    }

    //for ( int i = 0; i < beats.size(); i++ ) std::cerr << beats[i]->startTime() << " " << beats[i]->summativeVelocity() << std::endl;
    //while ( 1 ) {
    //    if ( _unplacedBeats.last()->startTime() - _unplacedBeats.first()->startTime() > 15 ) _unplacedBeats.removeFirst();
    //    else break;
    //}     the correct solution is to have a offset & tempo shift check

    if ( _unplacedBeats.size() < 40 ) return 0;   // not enough info to make a valid guess.
    else return true;
}

bool live::AnaBeat::_makeLists( QList< double >& x_, QList< double >& y_, QList< double >& velocity )
{
    double c = 0;
    foreach( AbBeat* beat, _unplacedBeats )
    {
        velocity.append( beat->summativeVelocity() );
        if ( c == 0 )
        {
            c = beat->startTime();
            continue;
        }
        else
        {
            x_.append( c );
            y_.append( log(beat->startTime()-c )/log(2.0f) );
            c = beat->startTime();
        }
    }

    if ( x_.isEmpty() ) return 0;
    else return true;
}

double* live::AnaBeat::_rotate( QList<double> &x_, QList<double> &y_ )
{
    // FIX ME:: Can crash... Maybe if all the beats are of similar length?

    bool ok = 0;

    double lowest = 9999;   //Pass
    double highest= -1;     //Pass
    double l = 0.01;
    double lastlb = 0, lasthb = 0;
    int tryNo = 0;

    QList< double > x;          // temporary form of x_
    QList< double > y;          // temporary form of y_

    while ( !ok )
    {
        if ( ++tryNo == 6000 )
        {
            ok = true;
            l = 0;
        }
        x = x_;
        y = y_;
        lowest = 9999;
        highest= -1;
        foreach( double a, y )
        {
            if ( a < lowest ) lowest = a;
            if ( a > highest) highest= a;
        }

        for ( int i = 0; i < y.size(); i++ )
        {
            y[i] += l*i;
        }
        l -= 0.00001;

        double lowestb = 9999;
        double highestb= -1;
        foreach( double a, y )
        {
            if ( a < lowestb ) lowestb = a;
            if ( a > highestb) highestb= a;
        }
        if ( ( highestb-lowestb ) < ( highest - lowest ) && ( lasthb - lastlb ) < ( highestb - lowestb ) )
        {
            lowest = lowestb;
            highest = highestb;
            ok = true;
        }
        lastlb = lowestb;
        lasthb = highestb;
    }
    y_ = y;
    x_ = x;

    double* highAndLow = new double[2];
    highAndLow[0] = highest;
    highAndLow[1] = lowest;
    return highAndLow;
}

int* live::AnaBeat::_makeHistogram( int &bins, double &highest, double &lowest, QList< double > &y )
{
    std::cerr << ( highest-lowest ) << "*\n";

    bins = rint( ( highest-lowest )*48.0384 );  // I forget where I got this number, but it's really smart! I promise!
    int bina = 0;

    int j = -1;
    int* count = new int[bins];
    for ( int i = 0; i < bins; i++ ) count[i] = 0;

    for ( int i = 0; i < 61; i++ )
    {
        double T = lowest + (highest-lowest)/bins * (double)(i+1);
        if ( j+1 < y.size() && T > y[j+1] ) bina++;
        while ( ++j < y.size() && T > y[j] ) count[i]++;
        j--;
    }

    //std::cerr << bina << "/" << bins << "!\n"; // most bins should be empty...
    for ( int i = 0; i < bins; i++ )
    {
        std::cerr << count[i] << " ";
    }

    return count;
}

bool live::AnaBeat::_correctRatios(int &noteID, QList<double> &last, int &bins, int *count, double &lowest,
                             double &highest, QList<double> &y_, QList<double> &yuncorrected,
                             QList<double> &velocity, QList<double>& y )
{

    int goodCount = 0;
    int fixed = 0;
    noteID = 0; // Pass
    for ( int i = 0; i < bins; i++ )
    {
        if ( count[i] == 0 ) goodCount++;   // FIX ME:: CREATE ALTERNATIVES
        else goodCount = 0;

        if ( goodCount == 10 || i == bins-1 )
        {
            noteID++;
            double Tmax = lowest + (highest-lowest)/bins * (double)(i+1);
            double Tmin = lowest + (highest-lowest)/bins * (double)(fixed);
            for ( int j = 0; j < y_.size(); j++ )
            {
                //std::cerr << Tmin << " " << Tmax << " " << y_[j] << std::endl;
                if ( Tmin <= y_[j] && y_[j] <= Tmax ) y_[j] = noteID;
            }
            fixed = i;
        }
    }

    std::cerr << std::endl << highest << " " << lowest << std::endl;

    // last is new.

    for ( int i = 1; i <= noteID; i++ )
    {
        int count = 0;
        double sum = 0;
        int velocityT = 0;
        for ( int j = y.size()/2; j < y.size(); j++ )
        {
            if ( y_[j] == i )
            {
                count++;
                sum += yuncorrected[j];
                velocityT += velocity[j];
            }
        }
        last.append( sum/count );
        //std::cerr << i << " " << sum/count << std::endl;
    }

    QList< double > currentDif;
    QList< double > correctDif;
    for ( int i = 1; i < last.size(); i++ ) currentDif.append( last[ i ] - last[ i-1 ] );
    for ( int i = 0; i < currentDif.size(); i++ )
    {
        correctDif.append( closestRatio( currentDif[i] ) );
    }
    for ( int i = 1; i < last.size(); i++ )
    {
        last[ i ] = last[ i-1 ] + correctDif[i-1];
    }

    return true;
}

QList< double > live::AnaBeat::_findBeatCandidates(QList<double> &last, QList<double> &y_, QList< double >& newX )
{
    QList< double > beatCandidats;
    for ( int i = 0; i < last.size(); i++ )
    {
        beatCandidats << pow((double)2.0, last[i] ) << pow(2, last[i] -1 ); //exp?
    }

    double tm = 0;
    // newX is there
    for ( int i = 0; i < y_.size(); i++ )
    {
        newX.append( tm );
        tm += pow(2, last[ y_[i]-1 ] ); //really exp
    }
    return beatCandidats;
}

double live::AnaBeat::_findTruePulse(QList<double> &beatCandidats, QList<double> &newX)
{
    QList< int > bcHits;
    QList< int > phase;
    for ( int i = 0; i < beatCandidats.size(); i++ )
    {
        QList< int > phaseHits;
        double tnow;
        for ( int j = 0; j < 20 && j < newX.size(); j++ )
        {
            phaseHits.append( 0 );
            tnow = newX[j];
            int w = 0;
            for ( double k = tnow; k < newX.last() - 0.005; k += beatCandidats[i] )
            {
                if ( beatCandidats[i] < 0.01 ) break;
                while ( newX[w] - 0.00001 < k )    // :) yay!
                {
                    if ( newX[w] - 0.00001 < k && newX[w] + 0.00001 > k )
                    {
                        phaseHits.last()++;
                    }
                    w++;
                }
            }
        }
        int highest = 0;
        int highestW;
        for ( int w = 0; w < phaseHits.size(); w++ )
        {
            if ( phaseHits[w] > highest )
            {
                highest = phaseHits[w];
                highestW= w;
            }
        }
        bcHits.append( highest );
        std::cerr << "!" << bcHits.last() << std::endl;
        phase.append( highestW );
    }

    for ( int i = 0; i< 0; i++ ) {} // !?

    int highestV = 0;
    double highestBC = 0;
    for ( int p = 0; p < bcHits.size(); p++ )
    {
        //std::cerr << bcHits[p] << " IS " << (double)60/beatCandidats[p] << std::endl;
        if ( bcHits[p] > highestV || ( bcHits[p] == highestV && beatCandidats[p] > highestBC ) )
        {
            highestV = bcHits[p];
            highestBC = beatCandidats[p];
        }
    }
    return highestBC;
}

bool live::AnaBeat::_findNum(int &oldFactor, int &factor, int &numLCD, QList<double> &newX,
                       double &highestBC, QList<double> &velocity)
{
    bool done = 0;
    bool doneCLOOP = true;
    factor = 1;
    oldFactor = 1;  // Pass
    numLCD = 1;
    while ( !done && doneCLOOP )
    {
        doneCLOOP = 0;
        int cBest = 0;
        double cScore = 0;
        for ( int c = 2; c < 20 && c < newX.size()/2 && !doneCLOOP; c++ )
        {
            int summativeVelocities[20];    //e(0,c)
            for ( int i = 0; i < c; i++ ) summativeVelocities[i] = 0;

            int w = 0;
            int ci = 0;
            for ( double k = 0; k < newX.last() - 0.005; k += highestBC*factor )
            {
                if ( highestBC*factor < 0.1 ) break;
                while ( newX[w] - 0.00001 < k )    // :) yay!
                {
                    if ( newX[w] - 0.00001 < k && newX[w] + 0.00001 > k )
                    {
                        summativeVelocities[ci] += velocity[w];
                    }
                    w++;
                }
                ci++;
                if ( ci == c ) ci = 0;
            }
            //std::cerr << std::endl << c << ":";

            double sum = 0;
            double tlowest = 999999;
            double thighest = 0;
            for ( int i = 0; i < c; i++ )
            {
                sum += summativeVelocities[i];
                if ( summativeVelocities[i] < tlowest ) tlowest = summativeVelocities[i];
                if ( summativeVelocities[i] > thighest ) thighest = summativeVelocities[i];
                std::cerr << summativeVelocities[i] << " ";
            }

            std::cerr << std::endl;
            double sdsum = 0.0;
            for ( int i = 0; i < c; i++ )
            {
                sdsum += ( (sum/c) - summativeVelocities[i] )*( (sum/c) - summativeVelocities[i] );
            }
            double sd = sqrt( sdsum/c );

            //std::cerr << "mean: " << sum/c << " sd: " << sd << " z-value of max " << (thighest-(sum/c))/sd << std::endl;

            if ( (thighest-(sum/c))/sd > cScore )
            {
                cScore = (thighest-(sum/c))/sd;
                cBest = c;
            }

            //std::cerr << std::endl;
        }

        if ( cBest != 0 )
        {
            if ( cScore > 0.99 )
            {
                oldFactor = factor;
                factor *= cBest;
                //if ( numLCD != 2 )//
                numLCD *= cBest;
                //else numLCD = c;
                doneCLOOP = true;
            }
        }
    }
    return true;
}

// Yay!
