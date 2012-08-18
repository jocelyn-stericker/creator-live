#include "vstwizard.h"
#include "ui_vstwizard.h"

#include "vst.h"
#ifdef _WIN32
#include "vst_p.h"
#else
#include "vstlinux_p.h"
#endif
#include "vstselectionwidget.h"
#include "ui_vstselectionwidget.h"
#include "vstsidekick.h"
#include "vstwizard.h"
#include "midisystem_p.h"

#include <QProgressDialog>
#include <QInputDialog>
#include <QMessageBox>

// Can't have duplicates. FIXME

using namespace live;

VstWizard::VstWizard(live::ObjectPtr out, live::ObjectPtr loopback, QWidget *parent) :
    QStackedWidget(parent),
    s_out(out),
    s_loopback(loopback),
    ui(new Ui::VstWizard)
{
    ui->setupUi(this);

    ui->wizardPage1->setComplete(0);

    connect(ui->listWidget_vsti,SIGNAL(clicked(QModelIndex)),ui->listWidget_vsti,SLOT(setCurrentIndex(QModelIndex)));   //overwrite double-click select on certain systems
    connect(ui->listWidget_vsti,SIGNAL(currentRowChanged(int)),this,SLOT(vstChanged(int)));
    connect(ui->listWidget_vsti,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(next()));
    connect(ui->listWidget_out, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_chan, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));
    connect(ui->listWidget_inst, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(next()));

    connect(ui->listWidget_inst,SIGNAL(clicked(QModelIndex)),ui->listWidget_inst,SLOT(setCurrentIndex(QModelIndex)));   //overwrite double-click select on certain systems

    stockChangedEvent();
    if (ui->listWidget_vsti->count()) ui->listWidget_vsti->setCurrentRow(0);
    if (ui->listWidget_out->count()) ui->listWidget_out->setCurrentRow(0);

    connect(live::object::singleton(),SIGNAL(stockChanged()),this,SLOT(stockChangedEvent()));

    connect(ui->toolButton_back2, SIGNAL(clicked()), this, SLOT(back()));
    connect(ui->toolButton_back3, SIGNAL(clicked()), this, SLOT(back()));
    connect(ui->toolButton_back4, SIGNAL(clicked()), this, SLOT(back()));

    connect(ui->toolButton_next1, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_next2, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_next3, SIGNAL(clicked()), this, SLOT(next()));
    connect(ui->toolButton_next4, SIGNAL(clicked()), this, SLOT(finishUp()));

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(pageChangeEvent(int)));
}

void VstWizard::vstChanged(int z) {
    ui->wizardPage1->setComplete(z!=-1);

    if (!ui->listWidget_vsti->currentItem()) return;
    QString r2=(ui->listWidget_inst->currentItem()?ui->listWidget_inst->currentItem()->text():"");
    ui->listWidget_inst->clear();
    ui->listWidget_inst->addItem("Create a New Instance");
    ui->listWidget_inst->setCurrentRow(0);
    for (int i=0;i<Vst::s_map.size();i++) {
        if (Vst::s_map.keys()[i]==ui->listWidget_vsti->currentItem()->text()&&Vst::s_map.values()[i].second.valid()) {
            QString s = Vst::s_map.values()[i].first;
            ui->listWidget_inst->addItem(s);
        }
    }

    for (int i=0;i<ui->listWidget_inst->count();i++) {
        if (ui->listWidget_inst->item(i)->text()==r2) {
            ui->listWidget_inst->setCurrentRow(i);
        }
    }

    for (int i=0;i<Vst::s_map.size();i++) {
        Vst* v=cast<Vst*>(Vst::s_map.values()[i].second);
        if (!v) continue;
        if (v->filename!=ui->listWidget_vsti->currentItem()->text()) continue;
        for (int j=0;j<v->rep->s_sidekicks.size();j++) {
            if (s_out==v->rep->s_sidekicks[j]) {
                for (int k=0;k<ui->listWidget_inst->count();k++) {
                    qDebug() << "COMPARE"<<ui->listWidget_inst->item(k)->text()<<Vst::s_map.values()[i].first;
                    if (ui->listWidget_inst->item(k)->text()==Vst::s_map.values()[i].first) {
                        qDebug() << "Found (III)";
                        ui->listWidget_inst->setCurrentRow(k);
                    }
                }
            }
        }
    }
}

void VstWizard::pageChangeEvent(int page) {
    if (page==2) {   //INST->CHAN
        bool ok=1;
        if (ui->listWidget_inst->currentRow()==0) {
            //Check INST
            QString name;
            if (Vst::s_vst.count()) name="VST "+QString::number(Vst::s_vst.back()->getId()+2);
            else name="VST 1";
            QString s=QInputDialog::getText(this,"Instance Name","What name do you want to give to the new VST Window?",QLineEdit::Normal,name,&ok);
            if (ok&&s.size()) {
                // Check uniqueness
                for (int i=0;i<ui->listWidget_inst->count()&&ok;i++) {
                    if (ui->listWidget_inst->item(i)->text()==s) ok=0;
                }
                if (!ok) {
                    QMessageBox::critical(this,"Name already taken","The name you chose has already been taken. Please choose a different name...");
                } else {
                    // Launch!
                    QProgressDialog p("Launching "+ui->listWidget_vsti->currentItem()->text(),"",0,100,this);
                    p.show();
                    p.setValue(10);
                    p.setCancelButton(0);
                    p.setWindowTitle("Launching VSTi");
                    qApp->processEvents();
                    kill_kitten
                        s_vst=new Vst(ui->listWidget_vsti->currentItem()->text(),s);
                    if (!(cast<Vst*>(s_vst)->ok())) {
                        ok=0;
                        QMessageBox::critical(this,"Error starting VST","Could not start "+ui->listWidget_vsti->currentItem()->text());
                    }
                    if (ok) {
                        p.close();
                        ui->listWidget_inst->addItem(s);
                        ui->listWidget_inst->setCurrentRow(ui->listWidget_inst->count()-1);
                    }
                }
            }
        } else {        // Find the VST
            QList< QPair<QString, ObjectPtr > > list;
            if (ui->listWidget_vsti->currentItem()) list=Vst::s_map.values(ui->listWidget_vsti->currentItem()->text());
            for (int i=0;i<list.size();i++) {
                if (list[i].first==ui->listWidget_inst->currentItem()->text()) {
                    s_vst=cast<Vst*>(list[i].second);
                    break;
                }
            }
            if (!s_vst) {
                QMessageBox::critical(this,"Missing VST","Creator Live seems to have lost an instance of a vst... Maybe try creating a new instance?");
                ok=0;
                qDebug() << "NO SUCH VST";
            }
        }

        if (!ok) {
            back();
            return;
        } else {
            // Add correct chans...
            ui->listWidget_chan->clear();
            Vst* x=cast<Vst*>(s_vst);
            if (!x) {
                QMessageBox::critical(this,"Unknown Error","An unknown error occured. You could try again, or loudly complain to joshua@nettek.ca!");
                back();
                return;
            }
            char c='A';
            ui->listWidget_chan->clear();
            qDebug() << "ADD0";
            for (int i=0;i<x->rep->s_sidekicks.size();i++) {
                qDebug() << "ADD1";
                ui->listWidget_chan->addItem("Channel "+QString(c));
                ++c;
            }

            if (ui->listWidget_chan->count()) {
                ui->listWidget_chan->setCurrentRow(0);
            } else {
                QMessageBox::critical(this,"No Channels","Could not find any channels!!!");
                back();
                return;
            }
            for (int j=0;j<x->rep->s_sidekicks.size();j++) {
                if (s_out==x->rep->s_sidekicks[j]) {
                    ui->listWidget_chan->setCurrentRow(j);
                }
            }
        }
    }
}

void VstWizard::finishUp()
{
    Vst*a = cast<Vst*>(s_vst);
    if (!a||!ui->listWidget_chan->currentItem()) {
        QMessageBox::critical(this,"Unknown Error","An unknown error occured while trying to set the VSTi.");
        return;
    }
    int z=ui->listWidget_chan->currentRow();
    if (a->rep->s_sidekicks.size()<z) {
        QMessageBox::critical(this,"Bad channel","The channel you selected is invalid (and shouldn't have been shown to you!)...");
        return;
    }

    if (!ui->listWidget_out->currentItem())
    {
        QMessageBox::critical(this,"Unknown Error","An unknown error occured while trying to set the output.");
    }

    QList<ObjectPtr> ol=object::get(OutputOnly|AudioOnly);
    QStringList sl;
    for (int i=0;i<ol.size();i++) sl.push_back(ol[i]->name());
    int index=sl.indexOf(ui->listWidget_out->currentItem()->text());

    if (index==-1) {
        QMessageBox::critical(this,"Bad output","The output you selected is invalid (and shouldn't have been shown to you!)...");
        return;
    }

    emit instrumentUpdated(s_out=a->rep->s_sidekicks[z],s_loopback=ol[index]);
    deleteLater();
}

VstWizard::~VstWizard()
{
    delete ui;
}

void VstWizard::stockChangedEvent()
{
    qDebug() << "RESCAN"<<s_out.valid()<<dynamic_cast<live_private::MidiNull*>(s_out.data());
    bool ok=(currentIndex()==2)?!ui->listWidget_inst->currentRow():(currentIndex()<2);  //before selecting inst
    ui->listWidget_vsti->clear();
    ui->listWidget_vsti->addItems(Vst::getVstPaths());
    if (ui->listWidget_inst->currentItem()) for (int i=0;i<Vst::s_map.size();i++) {
        Vst* v=cast<Vst*>(Vst::s_map.values()[i].second);
        if (!v) continue;
        if (ui->listWidget_inst->currentItem()->text()==Vst::s_map.values()[i].first) {
            ok=1;
        }
    }
    if (!ok&&(currentIndex())) {
        restart();
    }

    QList<ObjectPtr> ol=object::get(OutputOnly|AudioOnly);
    QStringList sl;
    for (int i=0;i<ol.size();i++) sl.push_back(ol[i]->name());
    ui->listWidget_out->clear();
    ui->listWidget_out->addItems(sl);
    if (ol.size()) {
        ui->listWidget_out->setCurrentRow(0);
        for (int i=0;i<ol.size();i++)
        {
            if (ol[i]==s_loopback) ui->listWidget_out->setCurrentRow(i);
        }
    } else {
        QMessageBox::critical(this,"No Outputs","No outputs are available. Please adjust audio settings in the Creator Live menu.");
        close();
    }
}

void VstWizard::next() {
    if (currentIndex() + 1 != count()) {
        setCurrentIndex(currentIndex() + 1);
    }
}

void VstWizard::back() {
    if (currentIndex()) {
        setCurrentIndex(currentIndex() - 1);
    }
}

void VstWizard::restart() {
    setCurrentIndex(0);
}
