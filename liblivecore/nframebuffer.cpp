/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live/nframebuffer>

#include <live/audio>

namespace live {

QList<NFrameBuffer_p*> nframebuffer::_u;
QList<NFrameBuffer_p*> nframebuffer::_free;

void nframebuffer::updateNFrames() {
    kill_kitten {
        for (int i = 0; i < _u.size(); ++i) {
            _u[i]->updateNFrames();
        }
    }
}

NFrameBuffer_p::NFrameBuffer_p()
  : data(0)
  { updateNFrames();
    nframebuffer::_u.push_back(this);
}

NFrameBuffer_p::~NFrameBuffer_p()
{
    nframebuffer::_u.removeOne(this);
}

void NFrameBuffer_p::updateNFrames()
{
    delete[] data;
    data = new float[live::audio::nFrames()];
}

NFrameBuffer::NFrameBuffer()
{
    if (nframebuffer::_free.size())
        data = nframebuffer::_free.takeLast();
    else
        data = new NFrameBuffer_p;

    for(unsigned int i = 0; i < live::audio::nFrames(); ++i)
        (*this)[i] = 0.0f;
}

NFrameBuffer::~NFrameBuffer()
{
    nframebuffer::_free.push_back(data);
}

}
