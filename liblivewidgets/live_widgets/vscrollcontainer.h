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
#include "live/object"
#include "live_widgets/bindableparent.h"
#include "live_widgets/midibindingqt.h"
#include "liblivewidgets_global.h"

namespace live_widgets {

typedef QList<QWidget*> ShadowContainer;

class LIBLIVEWIDGETSSHARED_EXPORT VScrollContainer : public QWidget, public live_widgets::BindableParent {
    Q_OBJECT
    QList<QWidget*> s_identity;
public:

    ShadowContainer* s_sHathorView;
    bool compact;
    int c;
    QVBoxLayout* s_vBox;
    bool s_resizeable;
public slots:
    void removeOne(QObject* a) { s_identity.removeOne(qobject_cast<QWidget*>(a)); }
public:
    QWidget*& operator[](int z) { return s_identity[z]; }
    const QWidget* operator[](int z) const { return s_identity.at(z); }
    const QWidget* at(int z) const { return s_identity[z]; }
    unsigned count() const { return s_identity.size(); }
    QWidget* takeFirst() { return s_identity.takeFirst(); }
    void push_back(QWidget* w) { s_identity.push_back(w); }
    void insert(const int& i, QWidget* w) { s_identity.insert(i, w); }
    QWidget* back() { return s_identity.back(); }
    const QWidget* back() const { return s_identity.back(); }

    VScrollContainer(bool cresizeable=1)
      : BindableParent(this)
      , s_identity()
      , s_sHathorView(new ShadowContainer)
      , compact(0)
      , c(0)
      , s_vBox(new QVBoxLayout(this))
      , s_resizeable(cresizeable)
      { s_vBox->setContentsMargins(0, 0, 0, 0);
        live_widgets::binding::addWidget(this);
        s_vBox->setSizeConstraint(QLayout::SetMaximumSize);
    }

    ~VScrollContainer() {
        delete s_sHathorView;
    }

    void updateItems() {
        for(int i=0;i<s_vBox->count();i++) {
            if(s_vBox->itemAt(i)->spacerItem()) {
                delete s_vBox->takeAt(i--);
            }
        }
        for (int i=0; i<s_sHathorView->size(); i++) {
            s_vBox->removeWidget((*s_sHathorView)[i]);
        }
        s_sHathorView->clear();
        float mh=0.0f;
        for (unsigned i = 0; i < count(); ++i ) {
            if(operator[](i)->objectName()=="") {
                qCritical() << "WARNING: adding a null-named object of type"<<at(i)->metaObject()->className() << "inside"<<objectName();
                qCritical() << "THIS IS A BUG WHICH MUST BE DEALT WITH OR SAVING WILL NOT ALWAYS WORK!";
            }
            s_vBox->addWidget( operator [](i) );
            s_sHathorView->push_back( operator [](i) );
            mh += (float) operator[](i)->size().height();
        }

        if(compact) {
            s_vBox->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
        }

        for(int i=0;i<children().size();i++) {
            if(children()[i]->objectName()=="") {
                children()[i]->setObjectName("UnknownWidget_"+QString::number(++c));
            }
        }
    }

    QByteArray saveBindings() {
        updateItems();
        return BindableParent::saveBindings();
    }

    void loadBindings(const QByteArray &ba) {
        updateItems();
        BindableParent::loadBindings(ba);
    }
private:
    VScrollContainer(const VScrollContainer&)
      : QWidget()
      , BindableParent(this)
      , s_identity()
      , s_sHathorView(new ShadowContainer)
      , compact(0)
      , c(0)
      , s_vBox(new QVBoxLayout(this))
      , s_resizeable(0)
      { TCRASH();
    }
    VScrollContainer& operator=(const VScrollContainer&) {
        TCRASH();
        return *this;
    }
};

}

#endif // VSCROLLCONTAINER_H
