#include <live/object>
#include <live/audio>
#include <live/midi>
#include "gtest/gtest.h"
#include "live/../audiosystem_p.h"

#include <QApplication>

TEST(AudioSanity, Init) {
    live::lthread::uiInit(); // the testing thread is the UI thread.

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
    AudioListener() : live::Object("AL", false, false), s_got(false), s_highest(-2.0f), s_lowest(2.0f) {}
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
    live::audio::null(2)->audioConnect(b);
    // wait a while for the audio thread to process data.
    usleep(10000);
    EXPECT_TRUE(b->s_got);
    EXPECT_EQ(b->s_highest, 0.0f);
    EXPECT_EQ(b->s_lowest, 0.0f);
    delete b;
}


static void connectAllTheThings() {
    QList<live::ObjectPtr> ins = live::object::get(live::AudioOnly | live::InputOnly | live::NoRefresh);
    QList<live::ObjectPtr> outs = live::object::get(live::AudioOnly | live::OutputOnly | live::NoRefresh);

    for (int i = 0; i < ins.size(); ++i)
        for (int j = 0; j < outs.size(); ++j)
            ins[i]->audioConnect(outs[j]);
}

TEST(ObjectSanity, Connect) {
    QList<live::ObjectPtr> ins = live::object::get(live::AudioOnly | live::InputOnly | live::NoRefresh);
    QList<live::ObjectPtr> outs = live::object::get(live::AudioOnly | live::OutputOnly | live::NoRefresh);

    for (int h = 0; h < 2; ++h)
        for (int i = 0; i < (h ? outs : ins).size(); ++i) {
            EXPECT_EQ((h ? outs : ins)[i]->aInverseConnectionCount(), 0 );
            EXPECT_EQ((h ? outs : ins)[i]->aConnectionCount(), 0 );
        }

    connectAllTheThings();

    for (int h = 0; h < 2; ++h)
        for (int i = 0; i < (h ? outs : ins).size(); ++i) {
            EXPECT_EQ((h ? outs : ins)[i]->aInverseConnectionCount(), (h ? ins.count() : 0) );
            EXPECT_EQ((h ? outs : ins)[i]->aConnectionCount(), (h ? 0 : outs.count()) );
        }

    connectAllTheThings();

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
        for (int i = 0; i < 3000; ++i) {
             connectAllTheThings();
        }
    }
public:
    void setId(int z) { id = z; }
};

TEST(ObjectSanity, ThreadSafety) {
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
}

class TemporaryObject : public live::Object {
public:
    static int count;
    LIVE_EFFECT
    LIVE_HYBRID
    TemporaryObject()
      : live::Object("TMP", false, true)
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

TEST(Shutdown, Audio) {
    EXPECT_TRUE(live::audio::getCurrentInterface());
    live::audio::shutdown();
    EXPECT_FALSE(live::audio::getCurrentInterface());
}
