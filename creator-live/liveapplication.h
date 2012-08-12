/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef LIVEAPPLICATION_H
#define LIVEAPPLICATION_H

#include <QApplication>
#include <live/core>

class Ribbon;
class QMainWindow;
class Vst;

class LiveApplication : public QApplication
{
    Q_OBJECT
    friend class Vst;
    friend class Ribbon;
    friend class LiveWindow;

    QWidget* _mainWindow;
//    QWidget* _central;
//    Ribbon* _ribbon;
//    QList<Vst*> _vsts;
public:
    explicit LiveApplication(int& argc,char**argv);
    QWidget* mainWindow() { return _mainWindow; }
//    const Ribbon* constRibbon() { return _ribbon; }
//    Ribbon* ribbon() { return _ribbon; }
//    void addVst(Vst* vst);

signals:

public slots:
    void init();

};

extern LiveApplication* liveApp;    //like qApp

#endif // LIVEAPPLICATION_H
