/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef INTROWIZARD_H
#define INTROWIZARD_H

#include <QWidget>

#include "liblivewidgets_global.h"

namespace Ui {
class IntroWizard;
}

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT IntroWizard : public QWidget {
    Q_OBJECT
    
public:
    explicit IntroWizard(QWidget *parent = 0);
    ~IntroWizard();
    
private:
    void resizeEvent(QResizeEvent *);
    Ui::IntroWizard *ui;

signals:
    void standardRequested();
    void quitRequested();

private:
    IntroWizard(const IntroWizard &);
    IntroWizard& operator=(const IntroWizard&);
};

}

#endif // INTROWIZARD_H
