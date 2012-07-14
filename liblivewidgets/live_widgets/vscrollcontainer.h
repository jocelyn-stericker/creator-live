/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef VSCROLLCONTAINER_H
#define VSCROLLCONTAINER_H

#include <QWidget>
#include <QScrollArea>
#include <QList>
#include <QVBoxLayout>
#include "live/object.h"
#include "live_widgets/bindableparent.h"
#include "live_widgets/midibindingqt.h"
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT ShadowContainer : public QList<QWidget*> {};

class LIBLIVEWIDGETSSHARED_EXPORT VScrollContainer : public QWidget, public QList<QWidget*>, public live_widgets::BindableParent
{
    Q_OBJECT
public:
    ShadowContainer* s_sHathorView;
    bool compact;
    int c;
    QVBoxLayout* s_vBox;
    bool s_resizeable;
public slots:
    void removeOne(QObject* a) { QList<QWidget*>::removeOne(qobject_cast<QWidget*>(a)); }
public:
    VScrollContainer(bool cresizeable=1) :
        BindableParent(this),
        s_sHathorView(new ShadowContainer),
        compact(0),
        c(0),
        s_vBox(new QVBoxLayout(this)),
        s_resizeable(cresizeable)
    {
        live_widgets::MidiBindingQtSys::addWidget(this);
        s_vBox->setSizeConstraint(QLayout::SetMaximumSize);
        s_vBox->setContentsMargins(0,0,0,0);
    }

    ~VScrollContainer()
    {
        delete s_sHathorView;
    }

    void updateItems()
    {
        for(int i=0;i<s_vBox->count();i++)
        {
            if(s_vBox->itemAt(i)->spacerItem())
            {
                delete s_vBox->takeAt(i--);
            }
        }
        for(int i=0; i<s_sHathorView->size(); i++)
        {
            s_vBox->removeWidget((*s_sHathorView)[i]);
        }
        s_sHathorView->clear();
        float mh=0.0f;
        for(int i=0; i< QList<QWidget*>::size(); i++ )
        {
            if(operator[](i)->objectName()=="")
            {
                qCritical()<<"WARNING: adding a null-named object of type"<<at(i)->metaObject()->className()<<"inside"<<objectName();
                qCritical()<<"THIS IS A BUG WHICH MUST BE DEALT WITH OR SAVING WILL NOT ALWAYS WORK!";
            }
            s_vBox->addWidget( operator [](i) );
            s_sHathorView->push_back( operator [](i) );
            mh+=operator[](i)->size().height();
        }

        if(compact)
        {
            s_vBox->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));
        }

        for(int i=0;i<children().size();i++)
        {
            if(children()[i]->objectName()=="")
            {
                children()[i]->setObjectName("UnknownWidget_"+QString::number(++c));
            }
        }
    }

    QByteArray saveBindings()
    {
        updateItems();
        return BindableParent::saveBindings();
    }

    void loadBindings(const QByteArray &ba)
    {
        updateItems();
        BindableParent::loadBindings(ba);
    }
};

}

#endif // VSCROLLCONTAINER_H
