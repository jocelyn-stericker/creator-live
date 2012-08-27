/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/audiograph>

#include <live/audio>
#include <live/audiosecond>

namespace live {

AudioGraph::AudioGraph(live::AudioSecond* second, long boxSize)
  : m_second(second)
  , m_boxSize(boxSize)
  , m_boxCount(live::audio::sampleRate() / boxSize)
  , m_mins(new float[m_boxCount])
  , m_maxs(new float[m_boxCount])
  , m_box(0)
  , m_pos(0)
  , m_refCount(1)
  { for (int i = 0; i < m_boxCount; ++i)
        m_mins[i] = m_maxs[i] = 0.0f;
}

void AudioGraph::setPos(long pos) {
    if (m_box == pos/m_boxSize && pos <= m_pos) {
        // we can fast forward
        append(pos - m_pos);
        return;
    }
    m_box = pos / m_boxSize;
    m_pos = 0;
    resetBox(m_box);
    append(pos % m_boxSize);
    m_pos = pos % m_boxSize;
}

void AudioGraph::append(int length) {
    float* data = &(*m_second)[m_box*m_boxSize + m_pos];
    float* MIN = &m_mins[m_box];
    float* MAX = &m_maxs[m_box];
    while(1) {
        for (int i = m_pos; i < m_boxSize; ++i) {
            *m_mins = qMin(*data, *m_mins);
            *m_maxs = qMax(*data, *m_maxs);
            if (!--length) return;
            ++data;
        }
        ++MIN;
        ++MAX;
        resetBox(++m_box);
        m_pos = 0;
    }
}

void AudioGraph::updateBoxWithFrame(long pos) {
    long oldPos = m_pos;
    long oldBox = m_box;
    m_box = pos / m_boxSize;
    m_pos = 0;
    append(m_boxSize - 1);
    m_pos = oldPos;
    m_box = oldBox;
}

}
