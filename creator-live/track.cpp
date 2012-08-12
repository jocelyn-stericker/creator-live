/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "track.h"
#include <live/app>
#include <live_widgets/trackhint.h>

#include <QCoreApplication>

using namespace live;
using namespace live_widgets;

int Track::s_lastId=-1;

Track::Track(live::ObjectPtr cinput, live::ObjectPtr coutput)
    : BindableParent(this)
    , s_th(new TrackHint)
    , s_ambition(*(new Ambition(cinput,ObjectChain(),coutput)))
    , s_appUi_()
    , s_id(++s_lastId)
    , s_busy(0)
    , ui_outputName(0)
    , ui_chainWidget(new ChainTypeWidget(this)) {
    Object::beginAsyncAction();
    /*MAKE CHANGES IN BELOW CONSTRUCTOR TOO!!!!*/
    ui_chainWidget->setGeometry(0,0,width(),3);
    ui_chainWidget->setFixedHeight(2);
    setAcceptDrops(1);
    s_th->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    ui_midiSelect->hide();

    setFixedHeight(350);
//    connect(ui_midiSelect,SIGNAL(indexSet(int)),this,SLOT(selectOutput(int)));
    MidiBindingQtSys::addWidget(this);

    setObjectName("Track_"+QString::number(s_id));
    setGeometry(geometry());
    qDebug() << "XX" << coutput->name();
    Object::endAsyncAction();

}

Track::Track(Ambition* bp)
  : BindableParent(this)
  , s_th(0)
  , s_ambition(*bp)
  , s_appUi_()
  , s_id(-1)
  , ui_outputName(0)
  , ui_chainWidget(new ChainTypeWidget(this)) {

    Object::beginAsyncAction();
    /*MAKE CHANGES IN BELOW CONSTRUCTOR TOO!!!!*/
    ui_chainWidget->setGeometry(0,0,width(),3);
    ui_chainWidget->setFixedHeight(2);
    setAcceptDrops(1);
    s_th->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    ui_midiSelect->hide();


    setFixedHeight(350);
//    connect(ui_midiSelect,SIGNAL(indexSet(int)),this,SLOT(selectOutput(int)));
    MidiBindingQtSys::addWidget(this);

    setObjectName("Track_"+QString::number(s_id));
    setGeometry(geometry());
    Object::endAsyncAction();
}

Track::~Track() {
    Object::beginAsyncAction();
    qDebug() << "DESTROY TRACK";
    delete ui_outputName;
    foreach(QWidget* a,s_appUi_) {
        delete a;
    }

    delete &s_ambition;
    Object::endAsyncAction();
}

void Track::resizeEvent(QResizeEvent *e) {
    if (s_th)
        s_th->setGeometry(0, 0, width(), height());
    clearUiPipeline();
    makeUiPipeline();
    remakeChainWidget();
    ui_chainWidget->setGeometry(0,0,width(),5);
    if(e)
        QWidget::resizeEvent(e);
}

void Track::remakeChainWidget() {
    Object::beginAsyncAction();
    ui_chainWidget->reset();
    int i=0;
    foreach(QWidget* ui,s_appUi_) {
        ui_chainWidget->setBack(ui->x()+ui->width(),s_ambition.at(i)->processingMidi(),s_ambition.at(i)->processingAudio());
        i++;
    }
    Object::endAsyncAction();
}

void Track::clearUiPipeline() {
    // not marked async, so be careful.
    ui_chainWidget->reset();
}

void Track::makeUiPipeline() {
    // not marked async, so be careful.
    s_busy = 1;
    int remCount = s_appUi_.count();
    int sizes[s_appUi_.count()];
    for (int i = 0; i < s_appUi_.count(); ++i) {
        sizes[i] = -1;
    }
    int sum = 0;
    for (int i = 0; i < s_appUi_.count(); ++i) {
        AppFrame* ui = s_appUi_[i];
        if (!ui->expanding()) {
            sizes[i] = ui->getDesiredWidth();
            sum += sizes[i];
            --remCount;
        }
        ui->show();
    }
    if (remCount) {
        int widthForRemaining = (width() - sum) / remCount - 3 - (ui_outputName ? ui_outputName->width() : 0);
        for (int i = 0; i < s_appUi_.size(); ++i) {
            if (sizes[i] == -1)
                sizes[i] = widthForRemaining;
        }
    }
    int state_x = 0;
    for (int i = 0; i < s_appUi_.count(); ++i) {
        AppFrame* ui = s_appUi_[i];
        ui->setFixedHeight(height());
        ui->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->setGeometry(state_x, 0, sizes[i], height());
        state_x += sizes[i];
    }
    if (ui_outputName) {
        qDebug() << ui_outputName->width();
        ui_outputName->setGeometry(width() - ui_outputName->width(), 0, ui_outputName->width(), height());
        qDebug() << ui_outputName->geometry();
    }
    setGeometry(geometry());
    remakeChainWidget();
    QCoreApplication::processEvents();
    s_busy = 0;
}

void Track::dragEnterEvent(QDragEnterEvent *e) {
    Object::beginAsyncAction();
    if(e->mimeData()->hasFormat("text/plain")&&app::appNames().contains(e->mimeData()->text())) {
        if(e->mimeData()->text()!="FILTER"||s_ambition.inputIsMidiObject()) {
            e->acceptProposedAction();
        }
    }
    if(e->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"FileName\"")) {
        e->acceptProposedAction();
    }
    Object::endAsyncAction();
}

void Track::dropEvent(QDropEvent *e) {
    Object::beginAsyncAction();
    if(e->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"FileName\"")) {
        live::ObjectPtr backend = app::newBackend("SEQUENCER");
        AppFrame* frontend = qobject_cast<AppFrame*>(app::newFrontend("SEQUENCER",backend));
        int x=e->pos().x();
        int pos=0;
        if(s_appUi_.size()) {
            int lastHalfX=s_appUi_[0]->geometry().x()+0.5*s_appUi_[0]->geometry().width();
            for(int i=1;i<s_ambition.chainSize();i++) {
                int nextHalfX=s_appUi_[i]->geometry().x()+0.5*s_appUi_[i]->geometry().width();
                if(lastHalfX<=x&&x<=nextHalfX) {
                    pos=i;
                    break;
                }
                lastHalfX=nextHalfX;
                if(x>=lastHalfX) {
                    pos=i+1;
                }
            }
            if(s_appUi_.size()==1&&x>=lastHalfX) {
                pos=s_appUi_.size();
            }
        }
        addApp(pos,frontend,backend);

        QString file=e->mimeData()->data("application/x-qt-windows-mime;value=\"FileName\"");
        if(QFile::exists(file)&&(file.endsWith("flac",Qt::CaseInsensitive)||file.endsWith("ogg",Qt::CaseInsensitive)||file.endsWith("wav",Qt::CaseInsensitive))) {
            QMessageBox::warning(0,"Error","This functionality has not yet been reimplemented.","Complain to Josh!");
//            dynamic_cast<SequencerApp*>(backend.data())->s_audioTrack->importFile(file);
        } else if(QFile::exists(file)&&(file.endsWith("midi")||file.endsWith("MIDI")||file.endsWith("mid")||file.endsWith("MID"))) {
            QMessageBox::warning(0,"Error","This functionality has not yet been reimplemented.","Complain to Josh!");
//            dynamic_cast<SequencerApp*>(backend.data())->s_midiTrack->importFile(file);
        } else {
            QMessageBox::critical(this,"Not supported","flac,ogg,wav,midi,MIDI,mid,and MID files are the only types of files currently supported.");
        }
    } else if(app::appNames().contains(e->mimeData()->text())) {
        live::ObjectPtr backend = app::newBackend(e->mimeData()->text());
        AppFrame* frontend = qobject_cast<AppFrame*>(app::newFrontend(e->mimeData()->text(),backend));
        int x=e->pos().x();
        int pos=0;
        if(s_appUi_.size()) {
            int lastHalfX=s_appUi_[0]->geometry().x()+0.5*s_appUi_[0]->geometry().width();
            for(int i=1;i<s_ambition.chainSize();i++) {
                int nextHalfX=s_appUi_[i]->geometry().x()+0.5*s_appUi_[i]->geometry().width();
                if(lastHalfX<=x&&x<=nextHalfX) {
                    pos=i;
                    break;
                }
                lastHalfX=nextHalfX;
                if(x>=lastHalfX) {
                    pos=i+1;
                }
            }
            if(s_appUi_.size()==1&&x>=lastHalfX) {
                pos=s_appUi_.size();
            }
        }
        addApp(pos,frontend,backend);
    } else {
        qDebug() << "Rejected "<<e->mimeData()->text() << "(it is not a valid app name!!!)";
    }
    Object::endAsyncAction();
}

void Track::setOutput(live::ObjectPtr output,live::ObjectPtr loopback) {
    Object::beginAsyncAction();
    s_ambition.setOutput(output);
    s_ambition.setLoopbackOut(loopback);
    Object::endAsyncAction();
}

void Track::setInput(live::ObjectPtr input) {
    Object::beginAsyncAction();
    s_ambition.setInput(input);
    Object::endAsyncAction();
}

void Track::addWidget(int i, QWidget *frame) {

}

void Track::addApp(int i,AppFrame* appUi,live::ObjectPtr app) {
    Object::beginAsyncAction();
    if(s_th) {
        s_th->deleteLater();
        s_th=0;
    }
    if(app.valid()) {
        s_ambition.insert(i,app);
    }
    connect(appUi, SIGNAL(desiredWidthChanged()), this, SLOT(updateGeometriesIfNeeded()));

    connect(appUi->_tbBack,SIGNAL(clicked()),this,SLOT(logic_appBack()));
    connect(appUi->_tbClose,SIGNAL(clicked()),this,SLOT(logic_appDel()));
    connect(appUi->_tbNext,SIGNAL(clicked()),this,SLOT(logic_appNext()));

    clearUiPipeline();
    appUi->setParent(this);
    s_appUi_.insert(i,appUi);
    makeUiPipeline();
    update();
    Object::endAsyncAction();
}

void Track::delApp(int i) {
    Object::beginAsyncAction();
    //ONLY CALLED FROM logic_delApp. Do not actually delete app.

    s_ambition.removeFromChain(i);

    clearUiPipeline();
    s_appUi_.takeAt(i);
    makeUiPipeline();

    Object::endAsyncAction();
}

void Track::outputSelection() {
    if(s_ambition.inputIsAudioObject()) {
        QStringList a;
        a<<object::get(AudioOnly|OutputOnly);
        bool ok;
        QString ix=QInputDialog::getItem(this,"Select an output","Output:",a,a.indexOf(s_ambition.b_output.ref()),0,&ok);
        int i=a.indexOf(ix);
        if(ok) {
            QList<live::ObjectPtr> x=object::get(AudioOnly|OutputOnly);
            if(i!=-1&&i<x.size()&&x[i]->name()==ix) {
                setOutput(x[i]);
            } else {
                QMessageBox::warning(this,"Oops.","Something went wrong in trying to set the output. Sorry about that. Maybe try again?");
            }
        }
    } else if(s_ambition.inputIsMidiObject()) {
        QStringList a;
        a<<object::get(MidiOnly|OutputOnly|Instrument);
        QStringList out=a;
        bool ok;
        QString ix=QInputDialog::getItem(this,"Select an output","Output:",out,out.indexOf(s_ambition.b_output),0,&ok);
        int i=out.indexOf(ix);
        if(ok) {
            live::ObjectPtr out=object::get(MidiOnly|OutputOnly|Instrument|NoRefresh)[i];
            if(out->isAudioObject()&&out->isMidiObject()) {
                //Now for output
                QStringList a;
                a<<object::get(AudioOnly|OutputOnly);
                bool ok;
                QString ix=QInputDialog::getItem(this,"Select an output","This device supports audio out. Select one:",a,a.indexOf(s_ambition.b_output),0,&ok);
                int i=a.indexOf(ix);
                if(ok) {
                    if(i!=-1&&i<object::get(AudioOnly|OutputOnly|NoRefresh).size()&&
                            object::get(AudioOnly|OutputOnly|NoRefresh)[i]->name()==ix) {
                        Object::beginAsyncAction();
                        setOutput(out,object::get(AudioOnly|OutputOnly|NoRefresh)[i]);
                        Object::endAsyncAction();
                    } else {
                        QMessageBox::warning(this,"Oops.","Something went wrong in trying to set the output. Sorry about that. Maybe try again?");
                    }
                }
            } else if(out->name()==a[i]) {
                Object::beginAsyncAction();
                setOutput(out);
                Object::endAsyncAction();
            } else {
                QMessageBox::warning(this,"Oops.","Something went wrong in trying to set the output. Sorry about that. Maybe try again?");
            }
        }
    }
    clearUiPipeline();
    makeUiPipeline();
}

void Track::logic_appBack() {
    Object::beginAsyncAction();
    AppFrame*x=qobject_cast<AppFrame*>(sender()->parent());
    Q_ASSERT(x);
    for(int i=0;i<s_appUi_.size();i++) {
        if(x==s_appUi_[i]) {
            if(!i) {
                return;
            } else {
                live::ObjectPtr oa=s_ambition.at(i);
                AppFrame* of=s_appUi_[i];
                delApp(i);
                addApp(i-1,of,oa);
            }
        }
    }
    Object::endAsyncAction();
}

void Track::logic_appDel() {
    Object::beginAsyncAction();
    AppFrame*x=qobject_cast<AppFrame*>(sender()->parent());
    Q_ASSERT(x);
    for(int i=0;i<s_appUi_.size();i++) {
        if(x==s_appUi_[i]) {
            AppFrame* of=s_appUi_[i];
            delApp(i);
            of->deleteLater();
        }
    }
    Object::endAsyncAction();
}

void Track::logic_appNext() {
    Object::beginAsyncAction();
    AppFrame*x=qobject_cast<AppFrame*>(sender()->parent());
    Q_ASSERT(x);
    for(int i=0;i<s_appUi_.size();i++) {
        if(x==s_appUi_[i]) {
            if(i+1==s_appUi_.size()) {
                return;
            } else {
                live::ObjectPtr oa=s_ambition.at(i);
                AppFrame* of=s_appUi_[i];
                delApp(i);
                addApp(i+1,of,oa);
            }
        }
    }
    Object::endAsyncAction();
}

void Track::updateGeometriesIfNeeded() {
    if (s_busy)
        return;
    for (int i = 0; i < s_appUi_.size(); ++i) {
        s_appUi_[i]->b_resizing = true;
    }
    clearUiPipeline();
    makeUiPipeline();
}

void Track::updateGeometriesOrDie() {
    clearUiPipeline();
    makeUiPipeline();
    for (int i = 0; i < s_appUi_.size(); ++i) {
        s_appUi_[i]->b_resizing = false;
    }
}

int Track::getMaximumWidthFor(QWidget* w) {
    Object::beginAsyncAction();

    int sum = 0;
    int otherCount = 0;
    for (int i = 0; i < s_appUi_.count(); ++i) {
        AppFrame* ui = s_appUi_[i];

        if (ui == w)
            continue;

        if (!ui->expanding())
            sum += ui->getDesiredWidth();
        else
            ++otherCount;
        ui->show();
    }

    int wi = (width() - 18 - sum)/(otherCount + 1);
    Object::endAsyncAction();

    return wi;
}

void Track::setOutputChooser(AudioOutputChooser* a) {
    if (ui_outputName)
        ui_outputName->deleteLater();

    ui_outputName = a;

    if (!ui_outputName) {
        resizeEvent();
        return;
    }

    ui_outputName->b_trackName = s_ambition.b_output.ref();

    connect(&s_ambition.b_output,
            SIGNAL(changeObserved(QString,QString)),
            &ui_outputName->b_trackName,
            SLOT(set(QString)));

    ui_outputName->setParent(this);
    ui_outputName->setObjectName("ui_outputName");
    ui_outputName->show();
    connect(ui_outputName, SIGNAL(resized()), this, SLOT(updateGeometriesIfNeeded()));
    connect(ui_outputName, SIGNAL(outputChosen(live::ObjectPtr)), this, SLOT(setOutput(live::ObjectPtr)));
    connect(ui_outputName, SIGNAL(doneResizing()), this, SLOT(updateGeometriesOrDie()));
    updateGeometriesOrDie();
}
