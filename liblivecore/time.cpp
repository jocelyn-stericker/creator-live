/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/time.h>

#include <live/midi.h>
qint32 live::Time::toTime_ms() const
{
    if (sec==-1||nsec==-1) {
        return midi::getTime_msec();
    }
    return sec*1000+nsec/1000;
}

bool operator <(live::Time a,live::Time b)
{
    return a.toTime_ms()<b.toTime_ms();
}

bool operator <(live::Time a,int b)
{
    return a.toTime_ms()<b;
}

bool operator <(int a,live::Time b)
{
    return a<b.toTime_ms();
}

bool operator >(live::Time a,live::Time b)
{
    return b<a;
}

bool operator >(live::Time a,int b)
{
    return b<a;
}

bool operator >(int a,live::Time b)
{
    return b<a;
}

live::Time operator +(live::Time a,live::Time b)
{
    return live::Time(a.toTime_ms()+b.toTime_ms());
}

live::Time operator +(live::Time a,int b)
{
    return live::Time(a.toTime_ms()+b);
}

live::Time operator +(int a,live::Time b)
{
    return live::Time(a+b.toTime_ms());
}

live::Time operator -(live::Time a,live::Time b)
{
    return live::Time(a.toTime_ms()-b.toTime_ms());
}

live::Time operator -(live::Time a,int b)
{
    return live::Time(a.toTime_ms()-b);

}

live::Time operator -(int a,live::Time b)
{
    return live::Time(a-b.toTime_ms());

}
