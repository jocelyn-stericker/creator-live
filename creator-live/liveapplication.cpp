/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "liveapplication.h"
#include "livewindow.h"
#include "settingslinux.h"
#include <live/core>
#include <live/appinterface>
#include <live/instrument>
#include <live/instrumentinterface>
#include <QFontDatabase>
#include <QSettings>
#include <QMessageBox>
#include <QPluginLoader>
#include "live/../audiosystem_p.h"
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QWaitCondition>
#include <iostream>

//#include "src/Pipeline/soundfont.h"

using namespace live;
using namespace live_widgets;

LiveApplication* liveApp=0;

LiveApplication::LiveApplication(int& argc,char** argv) :
    QApplication(argc,argv), _mainWindow(0)
{
    liveApp=this;
    audio::registerInterface(new live_private::SecretAudio);

    QFontDatabase::addApplicationFont(":/icons/HandelGotDLig.ttf");
    QFile stylefile(":/style/default");
    stylefile.open(QIODevice::ReadOnly);
    setStyleSheet(QString(stylefile.readAll()));

    setApplicationName("Live");
    setOrganizationName("Nettek");
    setWindowIcon(QPixmap(":/icons/creatorLive.jpeg"));

    QSettings init;
    if (!init.value("Initialized",0).toBool()) {
        (new LiveAudioSettingsWidget)->exec();
    }
    init.setValue("Initialized",1);

    QTimer::singleShot(0, this, SLOT(init()));

#ifndef __QNX__
    // By default, the PlayBook is already setup and good to go, so whatever.

    while (!LiveAudioSettingsWidget::apply())
    {
        QMessageBox::warning(0,"Settings","Settings are not valid. Opening settings editor.");
        (new LiveAudioSettingsWidget)->exec();
    }
#endif

    song::setCurrent( new song("Default System") );

    midi::refresh();

#ifndef __QNX__
    QDir pluginsDir = QDir(qApp->applicationDirPath()+"/../plugins");
    std::cerr << "Plugin directory:" << pluginsDir.absolutePath().toAscii().data() << std::endl;
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (!fileName.endsWith(".so")&&!fileName.endsWith(".dll",Qt::CaseInsensitive)) continue;
        std::cerr << "Info for plugin " << qPrintable(qApp->applicationDirPath()+"/../plugins/"+fileName) << ": ";
        QPluginLoader loader(qApp->applicationDirPath()+"/../plugins/"+fileName);
        QObject *plugin = loader.instance();
        if (plugin) {
            AppInterface* appi=qobject_cast<AppInterface*>(plugin);
            if (appi) {
                std::cerr << "APP[";
                do {
                    std::cerr << " " << appi->name().toAscii().data();
                    app::registerInterface(appi);
                } while ((appi=appi->next())); // Extra parentheses to avoid warning
                std::cerr << " ]";
            }

            InstrumentInterface * insi = qobject_cast<InstrumentInterface*>(plugin);
            if (insi) {
                std::cerr << "INSTRUMENT[";
                do {
                    std::cerr << " " << insi->name().toAscii().data();
                    instrument::registerInterface(insi);
                } while ((insi = insi->next())); // Extra parentheses to avoid warning
                std::cerr << " ]";
            }

            std::cerr << endl;

        } else std::cerr << "ERROR[ " << qPrintable(loader.errorString()) << " ]" << std::endl;
    }

#else
    // reason for seperate implementation: applicationDirPath() just returns '.'
    QDir pluginsDir("/apps/ca.nettek.creatorlive.testDev_creatorlivef32c5f97/native/plugins/");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        qDebug()<<qPrintable(qApp->applicationDirPath()+"/plugins/"+fileName);
        if (!fileName.endsWith(".so")&&!fileName.endsWith(".dll",Qt::CaseInsensitive)) continue;
        QPluginLoader loader("/apps/ca.nettek.creatorlive.testDev_creatorlivef32c5f97/native/plugins/"+fileName);
        QObject *plugin = loader.instance();
        if (plugin) {
            std::cerr<<"Something good happened";
            AppInterface* appi=qobject_cast<AppInterface*>(plugin);
            if (appi) do app::registerInterface(appi); while ((appi=appi->next()));
        } else qDebug()<<qPrintable(loader.errorString())<<"!!";
    }
#endif

    //    qDebug()<<Store::get(Store::MidiOnly|Store::InputOnly).size() << "#@";
    //    Soundfont* sf=new Soundfont("/home/joshua/Dropbox/Etc/Steinway Grand Piano.sf2");
    //    Store::request("Portable Grand MIDI 1")->hybridConnect(sf);
    //    sf->audioConnect(Store::get(Store::AudioOnly|Store::OutputOnly)[0]);
    _mainWindow=new LiveWindow;
    _mainWindow->setWindowState(Qt::WindowMaximized);
    _mainWindow->show();
}

//void LiveApplication::addVst(Vst *vst)
//{
//    _vsts.push_back(vst);
//}

void LiveApplication::init() {
    lthread::uiInit();
}
