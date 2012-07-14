/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef INPUTDRAGWIDGET_H
#define INPUTDRAGWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QLineEdit>

#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT SpawnFrame : public QFrame
{
    Q_OBJECT
public:
    SpawnFrame(QWidget*p) : QFrame(p) { setAcceptDrops(1); }
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
signals:
    void textDropped(QString);
};

}

namespace Ui {
    class InputDragWidget;
}

namespace live_widgets {

class DropFrame;

class LIBLIVEWIDGETSSHARED_EXPORT InputDragWidget : public QFrame
{
    Q_OBJECT
    int s_count;
    Ui::InputDragWidget* s_ui;
public:
    explicit InputDragWidget(QWidget *parent = 0);
    ~InputDragWidget();
    QList<QLineEdit*> groupNames;
    QList<DropFrame*> dropFrameAs;
    QList<DropFrame*> dropFrameBs;
public slots:
    void newGroup(QString,QString="",QString="");
    void refresh();
    void destroyRow();
signals:
    void modified();
};

class LIBLIVEWIDGETSSHARED_EXPORT OutputDragWidget : public QFrame
{
    Q_OBJECT
    int s_count;
    Ui::InputDragWidget* s_ui;
public:
    explicit OutputDragWidget(QWidget *parent = 0);
    ~OutputDragWidget();
    QList<QLineEdit*> groupNames;
    QList<DropFrame*> dropFrameAs;
    QList<DropFrame*> dropFrameBs;
public slots:
    void newGroup(QString,QString="",QString="");
    void refresh();
    void destroyRow();
signals:
    void modified();
};

}

#endif // INPUTDRAGWIDGET_H
