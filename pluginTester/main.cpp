/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <QApplication>
#include "apptester.h"
#include "instrumenttester.h"
#include "live/appinterface.h"
#include "live/core.h"
#include "live/instrumentinterface.h"
#include "live/instrument.h"
#include "live/../audiosystem_p.h"
#include <QSettings>
#include <QFontDatabase>
#include <QPluginLoader>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include "live_widgets/appframe.h"

using namespace live;
using namespace live_widgets;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString plugin;
    if(argc>1) {
        QStringList args=a.arguments();
        args.removeFirst();
        plugin=args.join(" ");
    }

    audio::registerInterface(new live_private::SecretAudio);

    QFontDatabase::addApplicationFont(":/icons/HandelGotDLig.ttf");
    QFile stylefile(":/style/default");
    stylefile.open(QIODevice::ReadOnly);
    a.setStyleSheet(QString(stylefile.readAll()));

    a.setApplicationName("Live");
    a.setOrganizationName("Nettek");

    QSettings settings;
    QList<QVariant> inMap=settings.value("audio_inputMappings").toList();
    QList<QVariant> outMap=settings.value("audio_outputMappings").toList();

    audio::resetMappings();
    for(int i=0; i<inMap; i++)
    {
        Q_ASSERT(inMap[i].toStringList().size());
        audio::addMapping(inMap[i].toStringList(),1,settings.value("audio_inputMappingNames").toStringList()[i]);
    }

    for(int i=0; i<outMap; i++)
    {
        Q_ASSERT(outMap[i].toStringList().size());
        audio::addMapping(outMap[i].toStringList(),0,settings.value("audio_outputMappingNames").toStringList()[i]);
    }
    audio::refresh();

    song::current=new song("Default System");

    midi::refresh();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QDir pluginsDir = QDir(a.applicationDirPath()+"/../plugins");
    qDebug()<<pluginsDir;
    int i=-1;
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if(!fileName.endsWith(".so")&&!fileName.endsWith(".dll",Qt::CaseInsensitive)) continue;
        QPluginLoader loader(a.applicationDirPath()+"/../plugins/"+fileName);
        QObject *plugin = loader.instance();
        if(plugin) {
            AppInterface* appi=qobject_cast<AppInterface*>(plugin);
            InstrumentInterface* insti=qobject_cast<InstrumentInterface*>(plugin);
            if(appi) do app::registerInterface(appi); while((appi=appi->next()));
            if(insti) do instrument::registerInterface(insti); while((insti=insti->next()));

        } else qDebug()<<"ERROR:"<<qPrintable(loader.errorString());
    }

    QStringList list;
    for(int ib=0;ib<app::interfaces().size();ib++) {
        list.push_back(app::interfaces()[ib]->name()+" - "+app::interfaces()[ib]->description());
        if(!plugin.compare(app::interfaces()[ib]->name(),Qt::CaseInsensitive)) i=ib;
    }

    for(int ib=0;ib<instrument::interfaces().size();ib++) {
        list.push_back(instrument::interfaces()[ib]->name()+" - "+instrument::interfaces()[ib]->description());
        if(!plugin.compare(instrument::interfaces()[ib]->name(),Qt::CaseInsensitive)) i=ib+app::interfaces().size();
    }

    if(!list.size()) {
        QMessageBox::critical(0,"No plugins","Could not find any plugins to test");
        return 0;
    }
    if(list.removeDuplicates()) {
        QMessageBox::critical(0,"Duplicates","There is at lease one duplicate of one plugin!");
        return 0;
    }
    bool ok=1;
    if(plugin.size()&&i==-1) ok=0;
    if(ok&&i==-1) i =list.indexOf(QInputDialog::getItem(0,"Choose Plugin to test","Chose a plugin to test. (Tip: you can also specify a plugin as an argument in the command line)",list,0,0,&ok));

    if(!ok) {
        return 0;
    }
    
    if(i<app::interfaces().size()) {
        AppTester w(app::interfaces()[i]);
        w.show();
        return a.exec();
    } else {
        i-=app::interfaces().size();

        QList<ObjectPtr> objs=object::get(MidiOnly|InputOnly);
        QStringList objns;
        for(int ib=0;ib<objs.size();++ib) {
            objns.push_back(objs[ib]->name());
        }
        QString n=QInputDialog::getItem(0,"Choose MIDI input","Midi Input:",objns,0,0,&ok);
        if(!ok) return 0;
        InstrumentTester w(objs[objns.indexOf(n)],instrument::interfaces()[i]);
        w.show();
        return a.exec();
    }

}
