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
  , m_boxCount(live::audio::sampleRate() / boxSize + ((live::audio::sampleRate() % boxSize) ? 1 : 0))
  , m_dropout(live::audio::sampleRate() % boxSize)
  , m_mins(new float[m_boxCount])
  , m_maxs(new float[m_boxCount])
  , m_box(0)
  , m_pos(0)
  , m_refCount(1)
  { for (int i = 0; i < m_boxCount; ++i) {
        m_mins[i] = 0.0f; m_maxs[i] = -0.0f;
  }
}

void AudioGraph::setPos(long pos) {
    if (m_box == pos/m_boxSize && pos <= m_pos) {
        // we can fast forward
        append(pos - m_pos);
        return;
    }
    m_box = pos / m_boxSize;
    resetBox(m_box);
    append(pos % m_boxSize);
//    Q_ASSERT(m_pos == pos % m_boxSize);
}

void AudioGraph::append(int length) {
    Q_ASSERT(m_pos >= 0);
    if (!length) return;
    float* data = &m_second->s_data[m_box*m_boxSize + m_pos];
    float* MIN = &m_mins[m_box];
    float* MAX = &m_maxs[m_box];
    while(1) {
        for (; m_pos < m_boxSize && --length; ++m_pos) {
            if ((m_pos + 1 == m_boxSize) && (m_pos + m_box*m_boxSize > live::audio::sampleRate())) {
                m_pos = m_boxSize - 1;
                return;
            }
            *MIN = qMin(*data, *MIN);
            *MAX = qMax(*data, *MAX);
            ++data;
        }
        if (!length) {
            if(m_pos) ++m_pos;
            return;
        }
        ++MIN;
        ++MAX;
        Q_ASSERT(m_pos >= 0);
        m_pos = 0;
        Q_ASSERT(m_box+1 < m_boxCount);
        resetBox(++m_box);
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
