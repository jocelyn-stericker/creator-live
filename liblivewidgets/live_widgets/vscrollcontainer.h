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
    QList<QWidget*> m_identity;
public:

    ShadowContainer* m_sHathorView;
    bool compact;
    int c;
    QVBoxLayout* m_vBox;
    bool m_resizeable;
public slots:
    void removeOne(QObject* a) { m_identity.removeOne(qobject_cast<QWidget*>(a)); }
public:
    QWidget*& operator[](int z) { return m_identity[z]; }
    const QWidget* operator[](int z) const { return m_identity.at(z); }
    const QWidget* at(int z) const { return m_identity[z]; }
    unsigned count() const { return m_identity.size(); }
    QWidget* takeFirst() { return m_identity.takeFirst(); }
    void push_back(QWidget* w) { m_identity.push_back(w); }
    void insert(const int& i, QWidget* w) { m_identity.insert(i, w); }
    QWidget* back() { return m_identity.back(); }
    const QWidget* back() const { return m_identity.back(); }

    VScrollContainer(bool cresizeable=1)
      : BindableParent(this)
      , m_identity()
      , m_sHathorView(new ShadowContainer)
      , compact(0)
      , c(0)
      , m_vBox(new QVBoxLayout(this))
      , m_resizeable(cresizeable)
      { m_vBox->setContentsMargins(0, 0, 0, 0);
        live_widgets::binding::addWidget(this);
        m_vBox->setSizeConstraint(QLayout::SetMaximumSize);
    }

    ~VScrollContainer() {
        delete m_sHathorView;
    }

    void updateItems() {
        for(int i=0;i< m_vBox->count();i++) {
            if( m_vBox->itemAt(i)->spacerItem()) {
                delete m_vBox->takeAt(i--);
            }
        }
        for (int i=0; i< m_sHathorView->size(); i++) {
            m_vBox->removeWidget((* m_sHathorView)[i]);
        }
        m_sHathorView->clear();
        float mh=0.0f;
        for (unsigned i = 0; i < count(); ++i ) {
            if(operator[](i)->objectName()=="") {
                qCritical() << "WARNING: adding a null-named object of type"<<at(i)->metaObject()->className() << "inside"<<objectName();
                qCritical() << "THIS IS A BUG WHICH MUST BE DEALT WITH OR SAVING WILL NOT ALWAYS WORK!";
            }
            m_vBox->addWidget( operator [](i) );
            m_sHathorView->push_back( operator [](i) );
            mh += (float) operator[](i)->size().height();
        }

        if(compact) {
            m_vBox->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
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
      , m_identity()
      , m_sHathorView(new ShadowContainer)
      , compact(0)
      , c(0)
      , m_vBox(new QVBoxLayout(this))
      , m_resizeable(0)
      { TCRASH();
    }
    VScrollContainer& operator=(const VScrollContainer&) {
        TCRASH();
        return *this;
    }
};

}

#endif // VSCROLLCONTAINER_H
