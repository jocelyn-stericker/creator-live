/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "livewindow.h"
#include "ui_livewindow.h"

#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPropertyAnimation>
#include <QFileDialog>

#include "live/app.h"
#include "live/appinterface.h"
#include "live/object.h"
#include "live_widgets/vscrollcontainer.h"
#include "live_widgets/newinput.h"
#include "live_widgets/draglabel.h"
#include "live_widgets/introwizard.h"
#include "trackgroupaudio.h"
#include "trackgroupmidi.h"
#include "live/midibinding.h"
#include "settingslinux.h"

using namespace live;
using namespace live_widgets;

LiveWindow* LiveWindow::singleton=0;

LiveWindow::LiveWindow(QWidget *parent) :
    QWidget(parent), BindableParent(this),
    s_curPatch(0), s_iw(0), ui(new Ui::LiveWindow)
{
    singleton=this;
    ui->setupUi(this);
    QMenu* m=new QMenu();
    m->addAction("&New Project",this,SLOT(newProject()));
    m->addAction("&Open Project...",this,SLOT(open()));
    m->addAction("&Save Project",this,SLOT(saveAct()));
    m->addAction("&Save Project As...",this,SLOT(saveAs()));

    QMenu* recent=new QMenu("Recent Files...");
    s_recentMenu= recent;
    m->addMenu(recent);
//    s_recent.push_back(recent->addAction("File 1"));
//    s_recent.push_back(recent->addAction("File 2"));
//    s_recent.push_back(recent->addAction("File 3"));
//    s_recent.push_back(recent->addAction("File 4"));
//    s_recent.push_back(recent->addAction("File 5"));
    updateRecent();
    m->addSeparator();
    m->addAction("New &Input...",this,SLOT(newInput()));
    m->addAction("&Edit Metadata...");
    m->addSeparator();
    m->addAction("I&mport...");
    m->addAction("E&xport...");
    m->addAction("&Vst Instruments...",this,SLOT(editVst()));
    m->addAction("&Audio Setup...",this,SLOT(editAudioSetup()));
    m->addSeparator();
    m->addAction("&Help");
    m->addAction("&Quit Creator Live",this,SLOT(close()));

    ui->pushButton_creatorLive->setMenu(m);
    hideInsert();
    newProject(0);
    selectMode();

    connect(ui->comboBox_mode,SIGNAL(activated(int)),this,SLOT(setMode(int)));
    connect(ui->comboBox_key,SIGNAL(activated(int)),this,SLOT(setKey(int)));
    connect(ui->spinBox_bpm,SIGNAL(valueChanged(int)),this,SLOT(setBPM(int)));
    connect(ui->toolButton_metro,SIGNAL(toggled(bool)),this,SLOT(toggleMetro(bool)));
    connect(ui->toolButton_bindMidi,SIGNAL(toggled(bool)),bindings::me(),SLOT(showBindings(bool)));
    connect(bindings::me(),SIGNAL(showBindingsChanged(bool)),ui->toolButton_bindMidi,SLOT(setChecked(bool)));

    connect(ui->comboBox_patch,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentPatch(int)));
    connect(ui->toolButton_patchEdit,SIGNAL(clicked()),this,SLOT(editCurrentPatchName()));

    for(int i=0;i<app::interfaces().size();i++)
    {
        DragLabel* d=new DragLabel();
        d->setPixmap(app::interfaces()[i]->icon().pixmap(85,85));
        d->setFixedSize(85,85);
        d->setProperty("dragID",app::interfaces()[i]->name());
        d->setStyleSheet("* {"
                         "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 255), stop:0.243243 rgb(77, 76, 77), stop:0.972973 rgba(255, 255, 255, 255), stop:1 rgba(255, 255, 255, 255));"
                       "border-width:0;"
                         "}");
        d->show();
        dynamic_cast<QBoxLayout*>(ui->sac_insert->layout())->insertWidget(1,d);
    }

    MidiBindingQtSys::addWidget(this);
}

void LiveWindow::newProject(bool ask)
{
    s_patches.clear();
    s_curPatch=0;

    s_patches.push_back(new Patch);
    hideInsert();
    if(ask&&!askForClose("Create new Project?","Create a new project anyway?")) return;

    *song::current()->keySignature=KeySignature('C', ' ', KeySignature::Major);

    song::current()->metronome->setBpm(120);
    if(song::current()->metronome->active())
    {
        song::current()->metronome->pause();
    }
    song::current()->songName="Untitled";
    while(ui->sac_contents->count()) {
        delete ui->sac_contents->takeFirst();
    }

    connect(ui->spinBox_bpm,SIGNAL(valueChanged(int)),&song::current()->metronome->b_bpm,SLOT(set(int)));
    connect(&song::current()->metronome->b_bpm,SIGNAL(changeObserved(int,int)),this,SLOT(setBPM(int)));

    if(ask) selectMode();
    if(ask) s_fileName="";
}

VScrollContainer* LiveWindow::hathorView()
{
    return ui->sac_contents;
}

void LiveWindow::selectMode()
{
    delete s_iw;
    s_iw=new IntroWizard(ui->sac_contents);
    ui->header->hide(); ui->header_->show();
    connect(s_iw,SIGNAL(standardRequested()),this,SLOT(newInput()));
    connect(s_iw,SIGNAL(quitRequested()),this,SLOT(close()));
    ui->sac_contents->push_back(s_iw);
    ui->sac_contents->updateItems();
    curPatch()->widgets.push_back(s_iw);
}

void LiveWindow::newInput()
{
    setUpdatesEnabled(0);
    ui->header->show(); ui->header_->hide();
    if(s_iw) {
        ui->sac_contents->removeOne(s_iw);
        ui->sac_contents->updateItems();
        curPatch()->widgets.removeOne(s_iw);
        delete s_iw;
        s_iw=0;
    }
    NewInput* ni=new NewInput(ui->sac_contents);
    connect(ni,SIGNAL(created(live::ObjectPtr )),this,SLOT(reactOnCreation(live::ObjectPtr )));
    ui->sac_contents->push_back(ni);
    ui->sac_contents->updateItems();
    curPatch()->widgets.push_back(ni);
    setUpdatesEnabled(1);
}

void LiveWindow::reactOnCreation(live::ObjectPtr s)
{
//    showInsert();
    curPatch()->widgets.removeOne(qobject_cast<QWidget*>(sender()));
    ui->sac_contents->removeOne(qobject_cast<QWidget*>(sender()));
    if(s->isMidiObject())
    {
        TrackGroupMidi* h=new TrackGroupMidi(s, ui->sac_contents);
        curPatch()->widgets.push_back(h);
        ui->sac_contents->push_back(h);
        ui->sac_contents->updateItems();
        h->show();
    }
    else if (s->isAudioObject())
    {
        TrackGroupAudio* h=new TrackGroupAudio(s, ui->sac_contents);
        curPatch()->widgets.push_back(h);
        ui->sac_contents->push_back(h);
        ui->sac_contents->updateItems();
        h->show();
    }
}

void LiveWindow::hideInsert()
{
    for(int i=0;i<3;i++) {
        QWidget* w=(i?((i==2)?ui->widget_insertTop:ui->widget_insertBottom):ui->scrollArea_2);
        QPropertyAnimation* pa1=new QPropertyAnimation(w,"maximumHeight");
        pa1->setStartValue(w->maximumHeight());
        pa1->setEndValue(0);
        pa1->start(QPropertyAnimation::DeleteWhenStopped);
    }

}

void LiveWindow::showInsert()
{
    for(int i=0;i<3;i++) {
        QWidget* w=(i?((i==2)?ui->widget_insertTop:ui->widget_insertBottom):ui->scrollArea_2);
        QPropertyAnimation* pa1=new QPropertyAnimation(w,"maximumHeight");
        pa1->setStartValue(w->maximumHeight());
        pa1->setEndValue(i?15:85);
        pa1->start(QPropertyAnimation::DeleteWhenStopped);
    }
}

void LiveWindow::setMode(int a)
{
    ui->comboBox_mode->setCurrentIndex(a);
    switch(a) {
    case 0:
        showInsert();
        break;
    case 1:
        hideInsert();
        break;
    case 2:
        qFatal("Not implemented");
        break;
    }
}

void LiveWindow::setKey(int a)
{
    ui->comboBox_key->setCurrentIndex(a);
    switch(a) {
    case 0:
        *song::current()->keySignature=KeySignature('F','b',KeySignature::Major);
        break;
    case 1:
        *song::current()->keySignature=KeySignature('C','b',KeySignature::Major);
        break;
    case 2:
        *song::current()->keySignature=KeySignature('G','b',KeySignature::Major);
        break;
    case 3:
        *song::current()->keySignature=KeySignature('D','b',KeySignature::Major);
        break;
    case 4:
        *song::current()->keySignature=KeySignature('A','b',KeySignature::Major);
        break;
    case 5:
        *song::current()->keySignature=KeySignature('E','b',KeySignature::Major);
        break;
    case 6:
        *song::current()->keySignature=KeySignature('B','b',KeySignature::Major);
        break;
    case 7:
        *song::current()->keySignature=KeySignature('F',' ',KeySignature::Major);
        break;
    case 8:
        *song::current()->keySignature=KeySignature('C',' ',KeySignature::Major);
        break;
    case 9:
        *song::current()->keySignature=KeySignature('G',' ',KeySignature::Major);
        break;
    case 10:
        *song::current()->keySignature=KeySignature('D',' ',KeySignature::Major);
        break;
    case 11:
        *song::current()->keySignature=KeySignature('A',' ',KeySignature::Major);
        break;
    case 12:
        *song::current()->keySignature=KeySignature('E',' ',KeySignature::Major);
        break;
    case 13:
        *song::current()->keySignature=KeySignature('B',' ',KeySignature::Major);
        break;
    case 14:
        *song::current()->keySignature=KeySignature('F','#',KeySignature::Major);
        break;
    case 15:
        *song::current()->keySignature=KeySignature('C','#',KeySignature::Major);
        break;
    case 16:
        *song::current()->keySignature=KeySignature('G','#',KeySignature::Major);
        break;
    default:
        qFatal("Unrecognized Key Signature");
        break;
    }
}

void LiveWindow::setBPM(int a)
{
    ui->spinBox_bpm->setValue(a);
    song::current()->metronome->setBpm(a);
}

void LiveWindow::toggleMetro(bool m)
{
    ui->pushButton_creatorLive->setChecked(m);
//    ui->spinBox_bpm->setEnabled(!m);
    if(m) {
        Q_ASSERT(!song::current()->metronome->active());
        song::current()->metronome->start();
    } else {
        Q_ASSERT(song::current()->metronome->active());
        song::current()->metronome->pause();
    }
}

void LiveWindow::closeEvent(QCloseEvent *e)
{
    if(askForClose()) {
        e->accept();
    } else {
        e->ignore();
    }
}

bool LiveWindow::askForClose(QString title,QString question)
{
    return (QMessageBox::question(this,title,
                             "Everything since you last saved will be lost. "+question,
                             QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes);
}

LiveWindow::~LiveWindow()
{
    delete ui;
}

void LiveWindow::setCurrentPatch(int a)
{
    if(a>=s_patches.size()||a<0) {
        insertPatch();
        return;
    }
    ui->comboBox_patch->setCurrentIndex(a);
    s_patches[s_curPatch]->deactivate();
    s_curPatch=a;
    s_patches[s_curPatch]->activate();
}

void LiveWindow::setCurrentPatchName(QString s)
{
    ui->comboBox_patch->setItemText(s_curPatch,s);
}

void LiveWindow::editCurrentPatchName()
{
    setCurrentPatchName(QInputDialog::getText(this,"Patch Name","Patch Name"));
}

void LiveWindow::insertPatch()
{
    s_patches.push_back(new Patch);
    s_patches[s_curPatch]->deactivate();
    ui->comboBox_patch->insertItem(s_patches.size(),"New Patch");
    setCurrentPatch(s_patches.size()-1);
    editCurrentPatchName();
    newInput();
}

void LiveWindow::editVst()
{
    return;
    /*
    setMode(1);
    curPatch()->deactivate();
    ui->header->setEnabled(0);
    ui->pushButton_creatorLive->setEnabled(0);

    // FIXME
    VstPathChooser* vpc=new VstPathChooser();
    ui->sac_contents->push_back(vpc);
    ui->sac_contents->updateItems();
    vpc->show();
    connect(vpc,SIGNAL(destroyed(QObject*)),ui->sac_contents,SLOT(removeOne(QObject*)));
    connect(vpc,SIGNAL(destroyed()),this,SLOT(editVstDone()));*/
}

void LiveWindow::editVstDone()
{

    curPatch()->activate();
    ui->header->setEnabled(1);
    ui->pushButton_creatorLive->setEnabled(1);
}

void LiveWindow::editAudioSetup()
{
    setMode(1);
    curPatch()->deactivate();
    ui->header->setEnabled(0);
    ui->pushButton_creatorLive->setEnabled(0);

    LiveAudioSettingsWidget* w = new LiveAudioSettingsWidget();
    ui->sac_contents->push_back(w);
    ui->sac_contents->updateItems();
    connect(w,SIGNAL(destroyed(QObject*)),ui->sac_contents,SLOT(removeOne(QObject*)));
    connect(w,SIGNAL(destroyed()),this,SLOT(editAudioSetupDone()));
}

void LiveWindow::editAudioSetupDone()
{
    curPatch()->activate();
    ui->header->setEnabled(1);
    ui->pushButton_creatorLive->setEnabled(1);
}

void LiveWindow::saveAct()
{
    bool ok=1;
    if(!s_fileName.size()) ok=0;
    QFile* file=0;
    if(ok) {
        file= new QFile(s_fileName);
        if(!file->open(QFile::WriteOnly)) {
            delete file;
            file=0;
            ok=0;
        }
    }
    if(!ok) {
        QString f=QFileDialog::getSaveFileName(this,"Save...","","Creator Live Project Files (*.live)");
        s_fileName=f;
        if(!f.size()) return;
        file=new QFile(s_fileName);
        if(!file->open(QFile::WriteOnly)) {
            delete file;
            file=0;
            QMessageBox::critical(this,"Error Opening File","Could not open "+s_fileName,QMessageBox::Ok);
            return;
        }
    }
    file->write(save());
    file->close();
}

void LiveWindow::saveAs()
{
    QString f=QFileDialog::getSaveFileName(this,"Save...","","Creator Live Project Files (*.live)");
    s_fileName=f;
    saveAct();
}

void LiveWindow::open()
{
    QString file=QFileDialog::getOpenFileName(this,"Open...","","Creator Live Project Files (*.live)");
    if(!file.size()) return;
    QFile f(file);
    if(!f.open(QFile::ReadOnly)) {
        QMessageBox::critical(this,"Error Opening File","Could not open "+file,QMessageBox::Ok);
        return;
    }
    newProject(0);
    load(f.readAll());
}

void LiveWindow::updateRecent()
{
    QSettings settings;
    QStringList l=settings.value("Recent",QStringList()).toStringList();
    s_recentMenu->clear();
    for(int i=0;i<5&&i<l.size();i++) {
        s_recentMenu->addAction(l[i],this,SLOT(loadRecent()));
    }
}

void LiveWindow::loadRecent()
{
    QString file=qobject_cast<QAction*>(sender())->text();
    QFile f(file);
    if(!f.open(QFile::ReadOnly)) {
        QMessageBox::critical(this,"Error Opening File","Could not open "+file,QMessageBox::Ok);
        return;
    }
    newProject(0);
    load(f.readAll());
}
