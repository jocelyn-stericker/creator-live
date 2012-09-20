#include <live/object>
#include <live/audio>
#include <live/midi>
#include <live/audiotrack>
#include "gtest/gtest.h"
#include "live/../audiosystem_p.h"

#include <QApplication>

TEST(AudioSanity, Init) {
    live::lthread::uiInit(false); // the testing thread is the UI thread.

    { // live expects a QApplication. It doesn't need to actually run.
        int* argc = new int(1);
        char*** argv = new char**;
        *argv = new char*[1];
        (*argv)[0] = new char[4];
        (*argv)[0][0] = 'l';
        (*argv)[0][0] = 'i';
        (*argv)[0][0] = 'v';
        (*argv)[0][0] = 'e';
        new QApplication(*argc, *argv);
    }

    live::audio::registerInterface(new live_private::SecretAudio);
    EXPECT_TRUE(live::audio::valid());

    // we can't test nFrames yet.
    EXPECT_GT(live::audio::sampleRate(), 0);
    EXPECT_GT(live::audio::sampleRate(), 22049);
    EXPECT_LT(live::audio::sampleRate(), 352801);

    live::lthread::uiInit(true); // the testing thread is the UI thread.
}

TEST(AudioSanity, Mapping) {
    int xruns = live_private::SecretAudio::XRUNS;

    QStringList in = live::audio::getInputChanStringList();
    QStringList out = live::audio::getOutputChanStringList();
    EXPECT_GT(in.size(), 0);  // while this can be valid, our tests
    EXPECT_GT(out.size(), 0); // are useless if this these fail.

    EXPECT_EQ(xruns, live_private::SecretAudio::XRUNS);

    for (int h = 0; h < 2; ++h) {
        for (int i = 0; i < (h ? out : in).size(); i += 2) {
            QStringList l( (h ? out : in)[i]);
            l.push_back( (h ? out : in)[i+1]);
            live::audio::addMapping(l, !h, (QString("TML") + QString(h ? "out" : "in") + QString::number(i)));
            std::cerr << "[ INFO     ] Creating mapped " << (h ? "out" : "in") << "put with " << l[0].toAscii().data() << " and " << l[1].toAscii().data() << std::endl;
        }
    }

    EXPECT_LT(live_private::SecretAudio::XRUNS, xruns + 3);     // accept some xruns for creating mappings.

    xruns = live_private::SecretAudio::XRUNS;

    EXPECT_EQ(in.size() / 2, live::object::get(live::AudioOnly | live::InputOnly | live::NoRefresh).size());
    EXPECT_EQ(out.size() / 2, live::object::get(live::AudioOnly | live::OutputOnly | live::NoRefresh).size());

    EXPECT_EQ(xruns, live_private::SecretAudio::XRUNS);
}

TEST(AudioSanity, NFrames) {
    EXPECT_GT(live::audio::nFrames(), (unsigned)0);

    EXPECT_LT(live::audio::nFrames(), (unsigned)2049);
}

class AudioListener : public live::Object{
public:
    bool s_got;
    float s_highest;
    float s_lowest;

    LIVE_INPUT
    LIVE_AUDIO
    AudioListener() : live::Object("AL", false, false,1), s_got(false), s_highest(-2.0f), s_lowest(2.0f) {}
    void aIn(const float *data, int, Object *) {
        for (unsigned i = 0; i < live::audio::nFrames(); ++i) {
            s_highest = qMax(data[i], s_highest);
            s_lowest = qMin(data[i], s_lowest);
        }
        s_got = true;
    }
};

TEST(AudioSanity, NullWorks) {
    AudioListener* b = new AudioListener;
    live::Connection c(live::audio::null(2), b, live::AudioConnection);
    // wait a while for the audio thread to process data.
    usleep(10000);
    EXPECT_TRUE(b->s_got);
    EXPECT_EQ(b->s_highest, 0.0f);
    EXPECT_EQ(b->s_lowest, 0.0f);
    delete b;
}

static void connectAllTheThings(QList<live::Connection>&connectAllTheThingsList) {
    QList<live::ObjectPtr> ins = live::object::get(live::AudioOnly | live::InputOnly | live::NoRefresh);
    QList<live::ObjectPtr> outs = live::object::get(live::AudioOnly | live::OutputOnly | live::NoRefresh);

    for (int i = 0; i < ins.size(); ++i)
        for (int j = 0; j < outs.size(); ++j)
            connectAllTheThingsList.push_back(live::Connection(ins[i], outs[j], live::AudioConnection));
}

TEST(ObjectSanity, Connect) {
    QList<live::ObjectPtr> ins = live::object::get(live::AudioOnly | live::InputOnly | live::NoRefresh);
    QList<live::ObjectPtr> outs = live::object::get(live::AudioOnly | live::OutputOnly | live::NoRefresh);

    for (int h = 0; h < 2; ++h)
        for (int i = 0; i < (h ? outs : ins).size(); ++i) {
            EXPECT_EQ((h ? outs : ins)[i]->aInverseConnectionCount(), 0 );
            EXPECT_EQ((h ? outs : ins)[i]->aConnectionCount(), 0 );
        }

    QList<live::Connection> l;

    connectAllTheThings(l);

    for (int h = 0; h < 2; ++h)
        for (int i = 0; i < (h ? outs : ins).size(); ++i) {
            EXPECT_EQ((h ? outs : ins)[i]->aInverseConnectionCount(), (h ? ins.count() : 0) );
            EXPECT_EQ((h ? outs : ins)[i]->aConnectionCount(), (h ? 0 : outs.count()) );
        }

    l.clear();

    for (int h = 0; h < 2; ++h)
        for (int i = 0; i < (h ? outs : ins).size(); ++i) {
            EXPECT_EQ((h ? outs : ins)[i]->aInverseConnectionCount(), 0 );
            EXPECT_EQ((h ? outs : ins)[i]->aConnectionCount(), 0 );
        }
}

class AudioPoker : public QThread {
    int id;
private:
    void run() {
        for (int i = 0; i < 150; ++i) {
            QList<live::Connection> l;
            connectAllTheThings(l);
            l.clear();
        }
    }
public:
    void setId(int z) { id = z; }
};

TEST(ObjectSanity, ThreadSafety) {
//    live::audio::strictInnocentXruns = true; // this test must not cause xruns

    QList<live::ObjectPtr> ins = live::object::get(live::AudioOnly | live::InputOnly | live::NoRefresh);
    QList<live::ObjectPtr> outs = live::object::get(live::AudioOnly | live::OutputOnly | live::NoRefresh);

    for (int h = 0; h < 2; ++h)
        for (int i = 0; i < (h ? outs : ins).size(); ++i) {
            EXPECT_EQ((h ? outs : ins)[i]->aInverseConnectionCount(), 0 );
            EXPECT_EQ((h ? outs : ins)[i]->aConnectionCount(), 0 );
        }

    AudioPoker a[3];
    for (int i = 0; i < 3; ++i) {
        a[i].setId(i);
        a[i].start();
    }
    for (int i = 0; i < 3; ++i)
        EXPECT_TRUE(a[i].wait());


    for (int h = 0; h < 2; ++h)
        for (int i = 0; i < (h ? outs : ins).size(); ++i) {
            EXPECT_EQ((h ? outs : ins)[i]->aInverseConnectionCount(), 0 );
            EXPECT_EQ((h ? outs : ins)[i]->aConnectionCount(), 0 );
        }

//    live::audio::strictInnocentXruns = false; // this test must not cause xruns
}

class TemporaryObject : public live::Object {
public:
    static int count;
    LIVE_EFFECT
    LIVE_HYBRID
    TemporaryObject()
      : live::Object("TMP", false, true, 2)
    {
        ++count;
    }
    ~TemporaryObject()
    {
        --count;
    }
};

int TemporaryObject::count = 0;

TEST(ObjectSanity, TemporarysAreTemporary) {
    for (int i = 0; i < 30; ++i) {
        live::ObjectPtr a = new TemporaryObject;
    }
    EXPECT_EQ(TemporaryObject::count, 0);
}

class AudioTestGenerator : public live::Object {
public:
    LIVE_EFFECT
    LIVE_HYBRID
    int state;
    float* buffer;
    AudioTestGenerator() : Object("TEST_GENERATOR", 1, 0, 1), state(0), buffer(new float[live::audio::nFrames()]) {}
    void aIn(const float*, int chan, Object *) {
        if (chan) return;
        for (uint i = 0; i < live::audio::nFrames(); ++i) {
            state = (state+1) % 3;
            switch (state) {
            case 0:
                buffer[i] = -1.0f;
                break;
            case 1:
                buffer[i] = 0.0f;
                break;
            case 2:
                buffer[i] = 1.0f;
                break;
            }
        }
        aOut(buffer, chan, this);
    }
};

class AudioTestListener : public live::Object {
public:
    LIVE_OUTPUT
    LIVE_HYBRID
    int state;
    bool gotData;
    int failFrame;
    int frame;
    AudioTestListener() : Object("TEST_LISTENER", 1, 0, 1), state(0), gotData(false), failFrame(-1), frame(-1) {}
    void aIn(const float *data, int chan, Object *) {

        if (frame < 1) {
            if (++frame < 1) return;
            if (data[0] == -1.0f) state = -1;
            if (data[0] == 0.0f) state = 0;
            if (data[0] == 1.0f) state = 1;
        }
        gotData = true;
        ASSERT_EQ(chan, 0);

        for (uint i = 0; i < live::audio::nFrames(); ++i) {
            if (failFrame != -1) return;
            ++frame;
            state = (state+1) % 3;
            switch (state) {
            case 0:
                if (data[i] != -1.0f) failFrame=frame;
                break;
            case 1:
                if (data[i] != 0.0f) failFrame=frame;
                break;
            case 2:
                if (data[i] != 1.0f) failFrame=frame;
                break;
            }
        }
    }
};

TEST(AudioTrack, TestFrameworkSanity) {
    AudioTestGenerator* g = new AudioTestGenerator;
    AudioTestListener* l = new AudioTestListener;
    live::Connection c(live::audio::null(1),g,live::AudioConnection);
    live::Connection c1(g,l,live::AudioConnection);
    usleep(100000);
    EXPECT_TRUE(l->gotData);
    EXPECT_EQ(l->failFrame, -1);
    delete l;
    delete g;
}

TEST(AudioTrack, Sanity) {
    live::ObjectPtr p = new live::AudioTrack(1);
    live::AudioTrack* t = live::cast<live::AudioTrack*>(p);
    t->startRecord();

    AudioTestGenerator* g = new AudioTestGenerator;
    live::Connection c1(live::audio::null(1),g,live::AudioConnection);
    live::Connection c2(g,t,live::AudioConnection);
    t->startPlayback();

    live::audio::strictInnocentXruns = true; // recording to a track must not cause xruns.
    usleep(100000);
    live::audio::strictInnocentXruns = false;

    t->stopPlayback();
    t->stopRecord();
    t->setPos(0);

    delete g;

    live::Connection c3(live::audio::null(1),t,live::AudioConnection);
    AudioTestListener* l = new AudioTestListener;
    t->startPlayback();
    live::Connection c4(t,l,live::AudioConnection);

    usleep(50000);

    EXPECT_TRUE(l->gotData);
    EXPECT_EQ(l->failFrame, -1);

    delete l;
}

TEST(Shutdown, Audio) {
    EXPECT_TRUE(live::audio::getCurrentInterface());
    live::audio::shutdown();
    EXPECT_FALSE(live::audio::getCurrentInterface());
}
