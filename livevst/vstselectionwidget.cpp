/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                 ( ( ( VST PLUGIN ) ) )

*******************************************************/

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

#include <QInputDialog>
#include <QString>
using namespace live;

VstSelectionWidget::VstSelectionWidget(live::ObjectPtr out, live::ObjectPtr loopback, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VstSelectionWidget)
{
    (new VstWizard(out,loopback,parent))->show();
    s_out=out;
    s_loopback=loopback;

    ui->setupUi(this);
    ui->listWidget_vstis->addItems(Vst::getVstPaths());
    connect(ui->listWidget_vstis,SIGNAL(activated(QModelIndex)),this,SLOT(vstChangedLogic()));
    connect(ui->comboBox_instance,SIGNAL(currentIndexChanged(int)),this,SLOT(instanceChangedLogic(int)));
    connect(ui->comboBox_chan,SIGNAL(currentIndexChanged(int)),this,SLOT(chanChangedLogic(int)));
    connect(ui->pushButton_vstisettings,SIGNAL(clicked()),this,SLOT(clickedLogic()));
    connect(ui->comboBox_loopback,SIGNAL(currentIndexChanged(int)),this,SLOT(loopbackChangedLogic(int)));

    for(int i=0;i<Vst::s_map.size();i++) {
        Vst* v=cast<Vst*>(Vst::s_map.values()[i].second);
        if(!v) continue;
        for(int j=0;j<v->rep->s_sidekicks.size();j++) {
            if(s_out==v->rep->s_sidekicks[j]) {
                qDebug()<<"Found (I)";
                for(int k=0;k<ui->listWidget_vstis->count();k++) {
                    if(ui->listWidget_vstis->item(k)->text()==Vst::s_map.keys()[i]) {
                        qDebug()<<"Found (II)";
                        ui->listWidget_vstis->setCurrentRow(k);
                    }
                }
                vstChangedLogic();
                for(int k=0;k<ui->comboBox_instance->count();k++) {
                    qDebug()<<"COMPARE"<<ui->comboBox_instance->itemText(k)<<Vst::s_map.values()[i].first;
                    if(ui->comboBox_instance->itemText(k)==Vst::s_map.values()[i].first) {
                        qDebug()<<"Found (III)";
                        ui->comboBox_instance->setCurrentIndex(k);
                    }
                }
                ui->comboBox_chan->setCurrentIndex(j);
            }
        }
    }

    QList<live::ObjectPtr> list=live::object::get(live::AudioOnly|live::OutputOnly|live::NoRefresh);
    int ci=0;
    QStringList strlist;
    for(int i=0;i<list;i++) {
        strlist.push_back(list[i]->name());
        ui->comboBox_loopback->addItem(strlist.back());
        if(loopback==list[i]) ci=i;
    }
    if(strlist.size()) ui->comboBox_loopback->setCurrentIndex(ci);
    connect(object::singleton(),SIGNAL(stockChanged()),this,SLOT(showEvent()));
}

VstSelectionWidget::~VstSelectionWidget()
{
    delete ui;
}

void VstSelectionWidget::showEvent(QShowEvent * e)
{
    QString r1=ui->listWidget_vstis->currentItem()?ui->listWidget_vstis->currentItem()->text():"";
    ui->listWidget_vstis->clear();
    ui->listWidget_vstis->addItems(Vst::getVstPaths());
    for(int i=0;i<ui->listWidget_vstis->count();i++) {
        if(r1==ui->listWidget_vstis->item(i)->text()) ui->listWidget_vstis->setCurrentRow(i);
    }
    vstChangedLogic();

    ui->comboBox_loopback->clear();

    QList<live::ObjectPtr> list=live::object::get(live::AudioOnly|live::OutputOnly|live::NoRefresh);
    int ci=0;
    QStringList strlist;
    for(int i=0;i<list;i++) {
        strlist.push_back(list[i]->name());
        ui->comboBox_loopback->addItem(strlist.back());
        if(s_loopback==list[i]) ci=i;
    }
    if(strlist.size()) ui->comboBox_loopback->setCurrentIndex(ci);

    if(e) QWidget::showEvent(e);
}

void VstSelectionWidget::vstChangedLogic()
{
    if(!ui->listWidget_vstis->currentItem()) return;
    QString r2=ui->comboBox_instance->currentText();
    ui->comboBox_instance->clear();
    ui->comboBox_instance->addItem("Create a New Instance");
    for(int i=0;i<Vst::s_map.size();i++) {
        if(Vst::s_map.keys()[i]==ui->listWidget_vstis->currentItem()->text()&&Vst::s_map.values()[i].second.valid()) {
            QString s = Vst::s_map.values()[i].first;
            ui->comboBox_instance->addItem(s);
        }
    }

    for(int i=0;i<ui->comboBox_instance->count();i++) {
        if(ui->comboBox_instance->itemText(i)==r2) {
            ui->comboBox_instance->setCurrentIndex(i);
        }
    }
}

void VstSelectionWidget::instanceChangedLogic(int z)
{
    if(!z) {
        ui->pushButton_vstisettings->setText("Create...");
        if(s_out.valid()) emit instrumentUpdated(s_out=live::ObjectPtr(),s_loopback);
    } else {
        ui->pushButton_vstisettings->setText("Edit...");
        QString r2=ui->comboBox_instance->currentText();

        Vst* a=0;
        QList< QPair<QString, ObjectPtr > > list;
        if(ui->listWidget_vstis->currentItem()) list=Vst::s_map.values(ui->listWidget_vstis->currentItem()->text());
        for(int i=0;i<list.size();i++) {
            if(list[i].first==r2) {
                a=cast<Vst*>(list[i].second);
                break;
            }
        }
        if(!a) {
            qDebug()<<"NO SUCH VST";
            return;
        }

        char c='A';
        ui->comboBox_chan->clear();
        for(int i=0;i<a->rep->s_sidekicks.size();i++) {
            ui->comboBox_chan->addItem(QString(c));
            ++c;
        }
        chanChangedLogic(ui->comboBox_chan->currentIndex());
//        emit instrumentUpdated(s_out=a,s_loopback);
    }
}

void VstSelectionWidget::loopbackChangedLogic(int z)
{
    if(z==-1) return;
    QList<live::ObjectPtr> list=live::object::get(live::AudioOnly|live::OutputOnly|live::NoRefresh);
    if(s_loopback!=list[z]) emit instrumentUpdated(s_out,s_loopback=list[z]);
}

void VstSelectionWidget::chanChangedLogic(int z)
{
    if(z==-1) return;

    QString r2=ui->comboBox_instance->currentText();

    Vst* a=0;
    QList< QPair<QString, ObjectPtr > > list;
    if(ui->listWidget_vstis->currentItem()) list=Vst::s_map.values(ui->listWidget_vstis->currentItem()->text());
    for(int i=0;i<list.size();i++) {
        if(list[i].first==r2) {
            a=cast<Vst*>(list[i].second);
            break;
        }
    }
    if(!a) {
        qDebug()<<"NO SUCH VST";
        return;
    }

    if(a->rep->s_sidekicks.size()<z) {
        qDebug()<<"Not enough channels";
        return;
    }
    if(s_out!=a->rep->s_sidekicks[z]) emit instrumentUpdated(s_out=a->rep->s_sidekicks[z],s_loopback);
}

void VstSelectionWidget::clickedLogic()
{
    if(!ui->comboBox_instance->currentIndex()) {
        QString str=QInputDialog::getText(this,"Please choose Instance Name","Name:");
        if(str.isEmpty()) {
            return;
        } else {
            if(!ui->listWidget_vstis->currentItem()) {
                qDebug()<<"!NO SELECTION!!!";
                return;
            }
            new Vst(ui->listWidget_vstis->currentItem()->text(),str);
            vstChangedLogic();
            bool ok=0;
            for(int i=0;i<ui->comboBox_instance->count();i++) {
                if(ui->comboBox_instance->itemText(i)==str) {
                    ok=1;
                    ui->comboBox_instance->setCurrentIndex(i);
                    break;
                }
            }
            if(!ok) {
                qDebug()<<"Something went wrong in clickedLogic()";
            }
            Q_ASSERT(ok);
        }
    } else {
        QString r2=ui->comboBox_instance->currentText();

        Vst* a=0;
        QList< QPair<QString, Vst* > > list;
        if(ui->listWidget_vstis->currentItem()) Vst::s_map.values(ui->listWidget_vstis->currentItem()->text());
        for(int i=0;i<list.size();i++) {
            if(list[i].first==r2) {
                a=list[i].second;
                break;
            }
        }
        if(!a) {
            qDebug()<<"NO SUCH VST";
            return;
        }

        a->show();
    }
}
