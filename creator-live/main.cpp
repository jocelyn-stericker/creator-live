/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include "liveapplication.h"
#include "livewindow.h"
#include <live_widgets/draglabel.h>

#include <QtGui/QApplication>
#include <QMutex>
#include <iostream>
using namespace live;
using namespace std;

int main(int argc,char** argv)
{
    cout<<"Creator Live is copyright Joshua Netterfield 2010-2012. All rights are reserved."<<std::endl;
    LiveApplication* liveApp=new LiveApplication(argc,argv);
    int ret = liveApp->exec();
    audio::stop();
    return ret;
}
