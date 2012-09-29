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
  , x_me(QMutex::Recursive)
  , s_id(++s_lastId)
  , s_busy(0)
  , ui_outputChooser(0)
  , ui_chainWidget(new ChainTypeWidget(this))
  { initialize(); }

Track::Track(Ambition* bp)
  : BindableParent(this)
  , s_th(0)
  , s_ambition(*bp)
  , s_appUi_()
  , x_me(QMutex::Recursive)
  , s_id(-1)
  , s_busy(0)
  , ui_outputChooser(0)
  , ui_chainWidget(new ChainTypeWidget(this))
  { initialize(); }

void Track::initialize() {
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
}

Track::~Track() {
    live_mutex(x_me) {
        delete ui_outputChooser;
        foreach(QWidget* a,s_appUi_) {
            delete a;
        }

        delete &s_ambition;
    }
}

void Track::resizeEvent(QResizeEvent *e) {
    live_mutex(x_me) {
        if (s_th)
            s_th->setGeometry(0, 0, width(), height());
        clearUiPipeline();
        makeUiPipeline();
        remakeChainWidget();
        ui_chainWidget->setGeometry(0,0,width(),5);
        if(e)
            QWidget::resizeEvent(e);
    }
}

void Track::remakeChainWidget() {
    kill_kitten {
        ui_chainWidget->reset();
        int i=0;
        foreach(QWidget* ui,s_appUi_) {
            ui_chainWidget->setBack(ui->x()+ui->width(),s_ambition.at(i)->processingMidi(),s_ambition.at(i)->processingAudio());
            i++;
        }
    }
}

void Track::clearUiPipeline() {
    live_mutex(x_me) {
        ui_chainWidget->reset();
    }
}

void Track::makeUiPipeline() {
    live_mutex(x_me) {
        s_busy = 1;
        int remCount = s_appUi_.count();
        int* sizes = new int[s_appUi_.count()];
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
            int widthForRemaining = (width() - sum - (ui_outputChooser ? ui_outputChooser->width() : 0) - 3) / remCount;
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

        delete[] sizes;

        if (ui_outputChooser) {
            ui_outputChooser->setGeometry(width() - ui_outputChooser->width(), 0, ui_outputChooser->width(), height());
        }
        setGeometry(geometry());
        remakeChainWidget();
        s_busy = 0;
    }
}

void Track::dragEnterEvent(QDragEnterEvent *e) {
    live_mutex(x_me) {
        if(e->mimeData()->hasFormat("text/plain")&&app::appNames().contains(e->mimeData()->text())) {
            if(e->mimeData()->text()!="FILTER"||s_ambition.inputIsMidiObject()) {
                e->acceptProposedAction();
            }
        }
        if(e->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"FileName\"")) {
            e->acceptProposedAction();
        }
    }
}

void Track::dropEvent(QDropEvent *e) {
    live_mutex(x_me) {
        if(e->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"FileName\"")) {
            live::ObjectPtr backend = app::newBackend("SEQUENCER");
            AppFrame* frontend = qobject_cast<AppFrame*>(app::newFrontend("SEQUENCER",backend));
            int x=e->pos().x();
            int pos=0;
            if(s_appUi_.size()) {
                int lastHalfX = static_cast<int> (
                            static_cast<float>(s_appUi_[0]->geometry().x())+0.5*static_cast<float>(s_appUi_[0]->geometry().width())
                        );
                for(int i=1;i<s_ambition.chainSize();i++) {
                    int nextHalfX = static_cast<int> (
                            static_cast<float>(s_appUi_[i]->geometry().x())+0.5*static_cast<float>(s_appUi_[i]->geometry().width())
                        );
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
                QMetaObject::invokeMethod(backend->qoThis(), "importAudio", Q_ARG(QString, file));
            } else if(QFile::exists(file)&&(file.endsWith("midi")||file.endsWith("MIDI")||file.endsWith("mid")||file.endsWith("MID"))) {
                QMessageBox::warning(0,"Error","This functionality has not yet been reimplemented.","Complain to Josh!");
                QMetaObject::invokeMethod(backend->qoThis(), "importMidi", Q_ARG(QString, file));
            } else {
                QMessageBox::critical(this,"Not supported","flac,ogg,wav,midi,MIDI,mid,and MID files are the only types of files currently supported.");
            }
        } else if(app::appNames().contains(e->mimeData()->text())) {
            live::ObjectPtr backend = app::newBackend(e->mimeData()->text());
            AppFrame* frontend = qobject_cast<AppFrame*>(app::newFrontend(e->mimeData()->text(),backend));
            int x=e->pos().x();
            int pos=0;
            if(s_appUi_.size()) {
                int lastHalfX = static_cast<int> (
                        static_cast<float>(s_appUi_[0]->geometry().x()) + 0.5*static_cast<float>(s_appUi_[0]->geometry().width())
                    );
                for(int i=1;i<s_ambition.chainSize();i++) {
                    int nextHalfX = static_cast<int> (
                        static_cast<float>(s_appUi_[i]->geometry().x()) + 0.5*static_cast<float>(s_appUi_[i]->geometry().width())
                    );
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
    }
}

void Track::setOutput(live::ObjectPtr output,live::ObjectPtr loopback) {
    s_ambition.setOutput(output);
    s_ambition.setLoopbackOut(loopback);
}

void Track::setInput(live::ObjectPtr input) {
    s_ambition.setInput(input);
}

void Track::addWidget(int, QWidget *) {
    TCRASH();
}

void Track::addApp(int i,AppFrame* appUi,live::ObjectPtr app) {
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

    live_mutex(x_me) {
        clearUiPipeline();
        appUi->setParent(this);
        s_appUi_.insert(i,appUi);
        makeUiPipeline();
        update();
    }
}

void Track::delApp(int i) {
    live_mutex(x_me) {
        //ONLY CALLED FROM logic_delApp. Do not actually delete app.

        s_ambition.removeFromChain(i);

        clearUiPipeline();
        s_appUi_.takeAt(i);
        makeUiPipeline();
    }
}

void Track::outputSelection() {
    live_mutex(x_me) {
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
                            setOutput(out,object::get(AudioOnly|OutputOnly|NoRefresh)[i]);
                        } else {
                            QMessageBox::warning(this,"Oops.","Something went wrong in trying to set the output. Sorry about that. Maybe try again?");
                        }
                    }
                } else if(out->name()==a[i]) {
                    setOutput(out);
                } else {
                    QMessageBox::warning(this,"Oops.","Something went wrong in trying to set the output. Sorry about that. Maybe try again?");
                }
            }
        }
        clearUiPipeline();
        makeUiPipeline();
    }
}

void Track::logic_appBack() {
    live_mutex(x_me) {
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
    }
}

void Track::logic_appDel() {
    live_mutex(x_me) {
        AppFrame*x=qobject_cast<AppFrame*>(sender()->parent());
        Q_ASSERT(x);
        for(int i=0;i<s_appUi_.size();i++) {
            if(x==s_appUi_[i]) {
                AppFrame* of=s_appUi_[i];
                delApp(i);
                of->deleteLater();
            }
        }
    }
}

void Track::logic_appNext() {
    live_mutex(x_me) {
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
    }
}

void Track::updateGeometriesIfNeeded() {
    live_mutex(x_me) {
        if (s_busy)
            return;
        for (int i = 0; i < s_appUi_.size(); ++i) {
            s_appUi_[i]->b_resizing = true;
        }
        clearUiPipeline();
        makeUiPipeline();
    }
}

void Track::updateGeometriesOrDie() {
    live_mutex(x_me) {
        clearUiPipeline();
        makeUiPipeline();
        for (int i = 0; i < s_appUi_.size(); ++i) {
            s_appUi_[i]->b_resizing = false;
        }
    }
}

int Track::getMaximumWidthFor(QWidget* w) {
    live_mutex(x_me) {

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

        return wi;

    }
    Q_ASSERT(0);
    return -1;
}

void Track::setOutputChooser(live_widgets::ObjectChooser* a) {
    live_mutex(x_me) {
        if (ui_outputChooser)
            ui_outputChooser->deleteLater();

        ui_outputChooser = a;

        if (!ui_outputChooser) {
            resizeEvent();
            return;
        }

        ui_outputChooser->b_trackName = s_ambition.b_output;

        connect(&s_ambition.b_output,
                SIGNAL(changeObserved(QString,QString)),
                &ui_outputChooser->b_trackName,
                SLOT(set(QString)));

        ui_outputChooser->setParent(this);
        ui_outputChooser->setObjectName("ui_outputName");
        ui_outputChooser->show();
        connect(ui_outputChooser, SIGNAL(resized()), this, SLOT(updateGeometriesIfNeeded()));
        connect(ui_outputChooser, SIGNAL(objectChosen(live::ObjectPtr)), this, SLOT(setOutput(live::ObjectPtr)));
        connect(ui_outputChooser, SIGNAL(objectChosen(live::ObjectPtr,live::ObjectPtr)), this, SLOT(setOutput(live::ObjectPtr,live::ObjectPtr)));
        connect(ui_outputChooser, SIGNAL(doneResizing()), this, SLOT(updateGeometriesOrDie()));
        updateGeometriesOrDie();
    }
}
