/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "sequencergraph.h"
#include <live_widgets/midibindingqt.h>
#include "looperapp.h"
#include <QBrush>
#include <QPainter>
#include <QPicture>
#include <QMouseEvent>
#include <math.h>

using namespace live;
using namespace live_widgets;

SequencerGraph::SequencerGraph( QWidget* parent,SequencerApp* capp ) :
    QWidget(parent),
    app(capp),
    s_bindMode(0),
    selection(-1),
    s_initial(0),
    s_redrawpos_st(-1),
    s_redrawpos_nd(-1),
    s_scale(2646000),
    audioTrack(0),
    audioEstart(0),
    simpleCounter(0),
    audioLTime(0),
    lastA(0),
    lastB(0),
    midiTrack(0),
    midiOriginal(0),
    midiEstart(0),
    midiLTime(0)
{
    setAutoFillBackground(0);
    connect(bindings::me(),SIGNAL(showBindingsChanged(bool)),this,SLOT(setShowBindingsChanged(bool)));
//AUDIO
    connect(app->s_audioTrack,SIGNAL(dataUpdated(int,int)),this,SLOT(updateAudioData(int,int)));
    connect(app,SIGNAL(posSet(quint32)),this,SLOT(updatePos(quint32)));
    setMinimumSize( 50, 50 );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    for (int i=0; i<2; i++)
        audioOriginal[i] = 0;
    audioLTime = -1;

    audioTrack = app->s_audioTrack;

    updateAudioData(0,1);

//MIDI
//    connect(app->s_midiTrack, SIGNAL(dataUpdated()), this, SLOT(updateMidiData()) );
    connect(app->s_audioTrack, SIGNAL(locationChanged(quint32)), this, SLOT(updatePos(quint32)));

    midiTrack = app->s_midiTrack;
    midiOriginal = NULL;
    midiLTime = -1;

    updateMidiData();   // ensure this is last

    MidiBindingQtSys::addWidget(this);
    setObjectName("sequencerGraph");
}

SequencerGraph::~SequencerGraph()
{
    delete audioOriginal[0];
    delete audioOriginal[1];
    delete midiOriginal;
}

void SequencerGraph::setTime( double time )
{
    audioLTime = midiLTime = time;
    update();
}

// HELPER FUNCTIONS --

inline char letterName(int a)
{
    int dif[]={0,1,1,2,2,3,3,4,5,5,6,6};                        // common letter name differences by intervals in semi-tones
    int m_root=song::current()->keySignature->midiNote()%12;   // the role the tonic has in the chromatic octave starting on C (0=C, 1=C#, ...)
    a=(a%12)+((m_root>a%12)?12:0);                              // set 'a' such that 'a-m_root' defines the interval in semi-tones between the tonic and 'a'
    return ((QChar(song::current()->keySignature->s_root).toUpper().toAscii()-'A'+dif[a-m_root])%7+'A');   // return the best guess for the letter name based on the tonic letter name(...->s_root) and the interval (m_root-a)
}

inline int xoctave(int mPitch)
{
    return mPitch/12+((letterName(mPitch)<'C' )?1:0)+
            ((song::current()->keySignature->s_accidental==Pitch::Flat&&(mPitch%12==11))?1:0)-
            ((song::current()->keySignature->s_accidental==Pitch::Sharp&&(mPitch%12==0))?1:0);
}

void SequencerGraph::updateAudioData( int t1, int t2 )
{
    qDebug() << "update audio.";
    lthread::ui();

    if (!updatesEnabled()||!isVisible()||parentWidget()->width()<30) return;

    updateMidiData();

    if (!midiOriginal)
    {
        return;
    }
    //AUDIO
    int width = s_scale;
    float wscale = (float) this->width() / ( float ) width;
    float hscale = (float) this->height() / 2000.0f;
    int wscale_inv = (float) width / (float) this->width();

    AudioContainer** DATA=audioTrack->acquireData();

    QColor red(255,0,0);
    QColor white(255,255,255);

    for ( int i = 0; DATA&& i < 2; i++ )
    {
        if ( !DATA[i] )
        {
            continue;
        }
        if ( t1 == -1 || t2 == -1 || !audioOriginal[i] || this->size()!=audioOriginal[i]->size())
        {
            if ( audioOriginal[i] )
            {
                delete audioOriginal[i];
            }
            audioOriginal[i] = new QPixmap(this->size());
            audioOriginal[i]->fill(QColor(255,0,0,0));
            QPainter painter;
            painter.begin( audioOriginal[i] );
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.setRenderHint(QPainter::Antialiasing,1);
            const float*dataPtr;

            int START=qMax(0,(s_initial/wscale_inv)*wscale_inv);

            int counter=DATA[i]->getConstPointer(START,dataPtr);
            dataPtr-=dataPtr?1:0;
            int c=0;
            for ( int j = START; j < s_initial+width+wscale_inv;j+= wscale_inv ) //hack
            {
                float minx=1.1,maxx=-1.1;
                for (int k=0;k<wscale_inv;k++)
                {
                    if (dataPtr) ++dataPtr;
                    if (--counter==-1)
                    {
                        counter=DATA[i]->getConstPointer(j+k,dataPtr);
                        counter--;
                    }
                    if (dataPtr) {
                        ++c;
                        if (*dataPtr<minx) minx=*dataPtr;
                        if (*dataPtr>maxx) maxx=*dataPtr;
                    }
                    else
                    {
                        minx=qMin(minx,0.0f);
                        maxx=qMax(minx,0.0f);
                    }
                }
                painter.fillRect( (j-s_initial)*wscale, 1000*hscale,
                                  1, minx*1000*hscale,(qMax(qAbs(minx),maxx)>=1.0?red:white) );
                painter.fillRect( (j-s_initial)*wscale, 1000*hscale,
                                  1, maxx*1000*hscale,(qMax(qAbs(minx),maxx)>=1.0?red:white) );
            }
            qDebug() << "PROC:"<<c;
            painter.end();
            update();
        }
        else
        {
            QPainter painter;

            painter.begin( audioOriginal[i] );
            const float*dataPtr;

            int START=qMax(0,(t1/wscale_inv)*wscale_inv-wscale_inv);

            int counter=DATA[i]->getConstPointer(START,dataPtr);
            dataPtr-=dataPtr?1:0;
//            const quint32& nFrames=audio::nFrames();
            int c=0;
            for ( int j = START; j < t2; j+=wscale_inv  )
            {
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.fillRect((j-s_initial)*wscale, 0,1,2000*hscale,QColor(255,0,0,0));
                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                float minx=1,maxx=-1;
                for (int k=0;k<wscale_inv;k++)
                {
                    if (dataPtr) ++dataPtr;
                    if (--counter==-1)
                    {
                        counter=DATA[i]->getConstPointer(j+k,dataPtr);
                        counter--;
                    }

                    if (dataPtr) {
                        ++c;
                        if (*dataPtr<minx) minx=*dataPtr;
                        if (*dataPtr>maxx) maxx=*dataPtr;
                    }
                    else
                    {
                        minx=qMin(minx,0.0f);
                        maxx=qMax(minx,0.0f);
                    }
                }
                painter.fillRect( (j-s_initial)*wscale, 1000*hscale,
                                  1, minx*1000*hscale,(qMax(qAbs(minx),maxx)>=1.0?red:white) );
                painter.fillRect( (j-s_initial)*wscale, 1000*hscale,
                                  1, maxx*1000*hscale,(qMax(qAbs(minx),maxx)>=1.0?red:white) );
            }
//            qDebug() << "PROC2"<<c;
            painter.end();
            update(QRect((lastA-s_initial)*wscale,0,lastB*wscale,2000*hscale));
            lastA=t1;
            lastB=t2;
//            update(QRect((t1-s_initial)*wscale,0,t2*wscale,2000*hscale));
            if (s_redrawpos_st!=-1&&s_redrawpos_nd!=-1&&s_redrawpos_nd!=s_redrawpos_st)
            {
                int st=s_redrawpos_st;
                int nd=s_redrawpos_nd;
                s_redrawpos_st=s_redrawpos_nd=-1;
                qDebug()<<st<<":"<<nd<<":"<<s_initial;
                update((st-s_initial)*wscale,0,(nd-s_initial)*wscale,2000*hscale);
            }
            audioTrack->releaseData();
            return;
        }
    }
//    if (++simpleCounter==10)
    {
        simpleCounter=0;
    }
    audioTrack->releaseData();
}

void SequencerGraph::updateMidiData(float t1, float t2)
{
    lthread::ui();

    if (!updatesEnabled()||!isVisible()||parentWidget()->width()<30) return;

    if (midiOriginal) return;
    int width = s_scale;
    float wscale = (float) this->width() / ( float ) width;
//    float wscale = 1.0f;

    Q_UNUSED(t1);
    Q_UNUSED(t2);

    if ( midiTrack /*&& (t1 == -1 || t2 == -1 || !midiOriginal)*/ )
    {
        if ( midiOriginal )
        {
            delete midiOriginal;
        }
        midiOriginal = new QPixmap(this->width(),height());
        midiOriginal->fill(QColor(255,255,255,0));
        QPainter painter;
        painter.begin( midiOriginal );
        painter.scale(1.0f,((float)height())/2000.0f);
        painter.drawLine( 0, 1000, s_scale, 1000 );
        QList< QPair<int,float> > cache;
        QList< int > velCache;
        // fix this inefficient code
        QList<Event> DATA=midiTrack->getData();

        const float& sampleRate=audio::sampleRate();

        for (int i=DATA.size()-1; i>=0; i--)
        {
            Event* ev= &DATA[i];
            const float left=qMin((float)s_scale+s_initial,qAbs((float)ev->time.toTime_ms()/1000.0f*(float)sampleRate));

            if ( !ev->buddy && ev->velocity() != 0 && ev->simpleStatus()==Event::NOTE_ON)
            {
                cache.append( qMakePair( (int)ev->note(), (float) ev->time.toTime_ms()/1000.0f ) );
                velCache.append( ev->velocity() );
            }
            else if ( ev->buddy && (ev->velocity() == 0 || ev->simpleStatus()==Event::NOTE_OFF) )
            {
                float buddyTime=((float)ev->buddy->time.toTime_ms()/1000.0f);

                QPen pen( QColor( 0, 0, 0, ev->buddy->velocity()*2 ) );
                QBrush brush( QColor( 0, 0, 0, ev->buddy->velocity()*2 ) );
                painter.setBrush( brush );
                painter.setPen( pen );
                if (buddyTime*(float)sampleRate-s_initial>=0 &&
                        (left>buddyTime*(float)sampleRate-s_initial))
                    painter.drawRect(
                                wscale*(buddyTime*(float)sampleRate-(s_initial)),
                                1850-(letterName(ev->note())-'C'+xoctave(ev->note())*7-10)*30,
                                wscale*(left-buddyTime*(float)sampleRate),
                                30 );
            }
        }
//        qDebug() << "I"<<cache.size();
        for ( int i = 0; i < cache.size(); i++ )
        {
            const float left=qMin((double)s_scale+s_initial,(double)app->pos()/1000.0*(float)sampleRate);

            QPen pen( QColor( 0, 0, 0, velCache[i]*2 ) );
            QBrush brush( QColor( 0, 0, 0, velCache[i]*2 ) );
            painter.setBrush( brush );
            painter.setPen( pen );
            if (cache[i].second*(float)sampleRate-s_initial>=0 && (qMin((double)s_scale+s_initial,(double)app->pos()/1000.0*sampleRate)>cache[i].second*(float)sampleRate))
                painter.drawRect(
                            wscale*(cache[i].second*(float)sampleRate-s_initial),
                            1850-(letterName(cache[i].first)-'C'+xoctave(cache[i].first)*7-10)*30,
                            wscale*(left-cache[i].second*(float)sampleRate),
                            30 );

        }
        painter.end();
    }
//    update();
}

QList<Event> SequencerGraph::getEvents(int evx, int evy)
{
    QList<Event> ret;

    if (midiOriginal)
    {
        int width = midiOriginal->width();
        float wscale = (float) this->width() / ( float ) width;
        float hscale = (float) this->height() / 2000.0f;
        int a = evx/wscale;
        int b = evy/hscale;
        QList< int > trackPosCache;
        QList< QPair<int,float> > cache;
        QList< int > velCache;
        // fix this inefficient code
//        for (int i=0;i<midiTrack->s_data;i++)
        QList<Event> DATA=midiTrack->getData();
        for (int i=DATA.size()-1; i>=0; i--)
        {
            Event* ev= &DATA[i];

            if ( ev->velocity() != 0 )
            {
                trackPosCache.push_back(i);
                cache.append( qMakePair( (int)ev->note(), (float) ev->time.toTime_ms()/1000.0f ) );
                velCache.append( ev->velocity() );
            }
            else
            {
                for ( int i = 0; i < cache.size(); i++ )
                {
                    if ( cache[i].first == ev->note() )
                    {
                        if (a>=cache[i].second*(float)audio::sampleRate()&&a<=(float)ev->time.toTime_ms()/1000.0f*(float)audio::sampleRate()&&
                                b>=2000-(letterName(ev->note())-'C'+xoctave(ev->note())*7-10)*60&&b<=
                                2000-(letterName(ev->note())-'C'+xoctave(ev->note())*7-10)*60+60)
                        {
                            ret.push_back(DATA[trackPosCache[i]]);
                            ret.push_back(DATA[i]);
                            return ret;
                        }
                        cache.removeAt( i );
                        velCache.removeAt( i );
                        trackPosCache.removeAt( i );
                        i--;
                        break;
                    }
                }
            }
        }
    }
    return ret;
}

void SequencerGraph::paintEvent( QPaintEvent* ev )
{
    lthread::ui();

    if (!isVisible()||parentWidget()->width()<30)
    if (!midiOriginal) updateMidiData();
    if (!audioOriginal[0]) updateAudioData();
    Q_UNUSED(ev);
    QPainter painter;
    painter.begin( this );
    int width = s_scale;
    float wscale = (float) this->width() / ( float ) width;
    float hscale = (float) this->height() / 2000.0f;

    if (selection!=-1) {
        painter.scale( wscale, hscale );
        painter.fillRect(app->pos()/1000.0*audio::sampleRate()-s_initial,0,(selection-app->pos())/1000.0*audio::sampleRate(),2000,"grey" );
        painter.scale( 1.0/wscale, 1.0/hscale);
    }

    if (audioOriginal[0])
    {
        QRectF target(0.0, 0.0, this->width(), this->height());
        QRectF source(0.0, 0.0, audioOriginal[0]->width(), audioOriginal[0]->height());
        painter.drawPixmap(target,*audioOriginal[0],source);
    }

    if (midiOriginal)
    {
        QRectF target(0.0, 0.0, this->width(), this->height());
        QRectF source(0.0, 0.0, midiOriginal->width(), midiOriginal->height());
        painter.drawPixmap(target,*midiOriginal,source);
    }

    painter.scale( wscale, hscale );

    if (app->isPlaying())
    {
        int ACTX=(((float)app->pos())/1000.0*audio::sampleRate()-s_initial)*wscale;
        while (ACTX>=this->width()*0.85) {
            incrScroll();
            ACTX=(app->pos()/1000.0*audio::sampleRate()-s_initial)*wscale;
        }
        while (ACTX<0) {
            decrScroll();
            ACTX=(app->pos()/1000.0*audio::sampleRate()-s_initial)*wscale;
        }
    }

    painter.scale( 1.0f/wscale, 1.0f );

    painter.setPen(app->isRecord()?"red":"blue");
    painter.setBrush(QBrush("red"));
    if ( midiOriginal )
    {
//        midiOriginal->play( &painter );
        painter.scale( wscale, 1.0f );
        if ( midiLTime < 0 ) midiLTime = (double)app->pos()/1000.0;
        painter.drawLine( midiLTime*audio::sampleRate()-s_initial, 0, (double)app->pos()/1000.0*audio::sampleRate()-s_initial, 2000 );
        midiLTime = -1;
    } else {
        painter.scale( wscale, 1.0f );
    }
    LooperApp* lapp=app->s_cheat;
    if (lapp)
    {
        int loopTime=lapp->b_loopLength;
        painter.drawLine( (float)loopTime/1000.0f*(float)audio::sampleRate()-s_initial, 0, (float)loopTime/1000.0f*(float)audio::sampleRate()-s_initial, 2000);

    }
    painter.scale( 1.0f/wscale, 1.0f/hscale );

    if (s_bindMode) {
        painter.fillRect(ev->rect(),QColor(0,0,255,80));
    }

    painter.end();
}

void SequencerGraph::mousePressEvent( QMouseEvent* ev )
{
    lthread::ui();

    if (ev->button()==Qt::LeftButton) {
        if (s_bindMode) {
            emit customContextMenuRequested(ev->pos());
            return;
        }
    }

    if ( !app )
    {
        return;
    }

    if (ev->button()==Qt::LeftButton)
    {
        selection=-1;
        if (midiOriginal)   // else out of luck...
        {
            int width = s_scale;
            float wscale = (float) this->width() / ( float ) width;

            double time = (float) ev->x() / wscale / (float)audio::sampleRate()+(float)s_initial/(float)audio::sampleRate();

            app->setPos(time*1000 );
        }
    }
    if (ev->button()==Qt::RightButton)
    {
//        emit customContextMenuRequested(QPoint());
    }
}

void SequencerGraph::mouseMoveEvent(QMouseEvent *ev)
{
    lthread::ui();

    setFocus();
    if (!app||!(ev->buttons()&Qt::LeftButton))
    {
        return;
    }
    int width = s_scale;
    float wscale = (float) this->width() / ( float ) width;
    double time = (float) ev->x() / wscale / (float)audio::sampleRate()+(float)s_initial/(float)audio::sampleRate();
    selection=time*1000;

    {
        s_leftMost=qMin(s_leftMost,(int)(time*audio::sampleRate()));
        s_rightMost=qMax(s_rightMost,(int)(time*audio::sampleRate()));
        int one=app->pos()/1000.0*audio::sampleRate();
//        qDebug()<<s_leftMost<<s_rightMost<<one;
//        int two=one+(selection-app->pos())/1000.0*audio::sampleRate();

        if (s_redrawpos_st!=-1)
        {
//            s_redrawpos_st=qMin(s_redrawpos_st,qMin(one,two));
            s_redrawpos_st=qMin(s_redrawpos_st,one);
        }
        else
        {
//            s_redrawpos_st=qMin(one,two);
            s_redrawpos_st=one;
        }
        s_redrawpos_st=qMin(s_redrawpos_st,s_leftMost);
        s_redrawpos_nd=qMax(s_redrawpos_nd,one);
//        s_redrawpos_nd=qMax(s_redrawpos_nd,qMax(one,two));
        s_redrawpos_nd=qMax(s_redrawpos_nd,s_rightMost);

        s_rightMost=s_leftMost=(int)(time*audio::sampleRate());
//        update;
    }
}

void SequencerGraph::wheelEvent(QWheelEvent *ev)
{
    if (selection)
    {
        // BUG: We could get in to fights with recording.

        AudioContainer** DATA=audioTrack->acquireData();

        int a=qMin(selection/1000.0f*audio::sampleRate(),(float)app->pos()/1000.0f*audio::sampleRate());
        int b=qMax(selection/1000.0f*audio::sampleRate(),(float)app->pos()/1000.0f*audio::sampleRate());
        for (int h=0;h<2;h++)
        {
            const float* dataPtr;
            int counter=DATA[h]->getConstPointer(a,dataPtr);
            dataPtr-=dataPtr?1:0;

            for (int i=a;i<b;i++)
            {
                if ((dataPtr+=dataPtr?1:0),--counter==-1)
                {
                    counter=DATA[h]->getConstPointer(i,dataPtr);
                }
                if (dataPtr)
                {
                    // remove this functionality
//                    *dataPtr=(*dataPtr)*(1+((float)ev->delta()/8.0/15.0/10.0));
                }
            }
        }
        audioTrack->releaseData();
        updateAudioData();
    }
//    if (midiOriginal)
//    {
//        QList<Event*> evs=getEvents(ev->pos().x(),ev->pos().y());
//        if (evs.size()>=2)
//        {
////            int width = midiOriginal->boundingRect().width();
////            float wscale = (float) this->width() / ( float ) width;
////            int a = ev->pos().x()/wscale;

//            if (wev->modifiers()&Qt::ShiftModifier&&wev->modifiers()&Qt::ControlModifier)
//            {
//                evs[0]->time=evs[0]->time-Time(wev->delta()*wev->delta()/400*(wev->delta()<0?-1:1));
//                evs[1]->time=evs[1]->time-Time(wev->delta()*wev->delta()/400*(wev->delta()<0?-1:1));
//            }
//            else if (wev->modifiers()&Qt::ShiftModifier)
//            {
//                bool swap=0;
//                Time* a_t = &evs[0]->time;
//                Time* b_t = &evs[1]->time;
////                if (qAbs(a-(cache[i].second-0.10)*(float)audio::sampleRate())<qAbs(a-(float)(ev->time.toTime_ms()+100)/1000.0f*(float)audio::sampleRate()))
//                {
//                    swap=1;
//                    qSwap(a_t,b_t);
//                }

//                if (!logical_xor( Time(a_t->toTime_ms()*(swap?-1:1))<*b_t+Time(wev->delta()*wev->delta()/400*(wev->delta()<0?-1:1)),swap))
//                {
//                    *a_t=*a_t+Time(wev->delta()*wev->delta()/400*(wev->delta()<0?-1:1)*(swap?-1:1));
//                }
//            }
//            else if (wev->modifiers()&Qt::ControlModifier)
//            {
//                int newNote=evs[0]->note()+wev->delta()/120;
//                evs[1]->setNote(newNote);
//                evs[0]->setNote(newNote);
//            }
//            else
//            {
//                evs[0]->setVelocity(evs[0]->velocity()+ wev->delta()/120);
//                if (evs[0]->velocity()<0)
//                {
//                    evs[0]->setVelocity(0);
//                }
//            }
//            return;
//        }
//    }
}

void SequencerGraph::keyPressEvent(QKeyEvent *ev)
{
    lthread::ui();

    kill_kitten if (ev->key()==Qt::Key_Delete&&selection!=-1)
    {
        {
            float a=qMin(selection/1000.0f*audio::sampleRate(),(float)app->pos()/1000.0f*audio::sampleRate());
            float b=qMax(selection/1000.0f*audio::sampleRate(),(float)app->pos()/1000.0f*audio::sampleRate());
            audioTrack->clearData(a,b);
            updateAudioData();
        }

        int a = qMin(selection,(float)app->pos());
        int b = qMax(selection,(float)app->pos());
        qDebug() << "$"<<selection<<app->pos();

        midiTrack->remove(a,b);

    }
}

void SequencerGraph::incrScroll()
{
    s_initial+=s_scale/3;
    updateAudioData();
    updateMidiData();
}

void SequencerGraph::decrScroll()
{
    s_initial=qMax(0,s_initial-s_scale/3);
    updateAudioData();
    updateMidiData();
}

void SequencerGraph::setShowBindingsChanged(bool ean)
{
    s_bindMode=ean;
    update();
}

void SequencerGraph::updatePos(quint32 a)
{
    int width = s_scale;
    float wscale = (float) this->width() / ( float ) width;
    float hscale = (float) this->height() / 2000.0f;
    if (s_redrawpos_st!=-1) s_redrawpos_st=qMin(s_redrawpos_st,(int)(a/1000.0*audio::sampleRate()));
    else s_redrawpos_st=a/1000.0*audio::sampleRate();
    s_redrawpos_nd=qMax(s_redrawpos_nd,(int)(a/1000.0*audio::sampleRate()));
    update((s_redrawpos_st-s_initial)*wscale,0,(s_redrawpos_nd-s_initial)*wscale,2000*hscale);
}

void SequencerGraph::setScale(int a)
{
    lthread::ui();

    s_scale=a;
    updateAudioData();
    updateMidiData();
}
