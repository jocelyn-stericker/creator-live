/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiosecond>

#include <QtConcurrent>

void live::AudioContainer::clear(bool dying)
{
    if(!m_readLock.tryLockForWrite()&&!dying) {
        QtConcurrent::run(this,&AudioContainer::clear,0);
        return;
    }
    m_dataSize=0;
    while (m_data.size())
    {
        AudioDataDestroyer* add = new AudioDataDestroyer(m_data.takeFirst(),(dying?0:&m_readLock));
        QtConcurrent::run(add,&AudioDataDestroyer::run);
    }
    if(!dying) {
        m_readLock.unlock();
    }
}
