/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiosecond>

#include <QtConcurrent>

void live::AudioContainer::clear(bool dying)
{
    if(!s_readLock.tryLockForWrite()&&!dying) {
        QtConcurrent::run(this,&AudioContainer::clear,0);
        return;
    }
    s_dataSize=0;
    while (s_data.size())
    {
        AudioDataDestroyer* add = new AudioDataDestroyer(s_data.takeFirst(),(dying?0:&s_readLock));
        QtConcurrent::run(add,&AudioDataDestroyer::run);
    }
    if(!dying) {
        s_readLock.unlock();
    }
}
