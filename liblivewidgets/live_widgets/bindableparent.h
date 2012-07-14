/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef BINDABLEPARENT_H
#define BINDABLEPARENT_H

#include <QVariant>
#include "liblivewidgets_global.h"

namespace live_widgets {

/**
 * To allow for MIDI-bound child widgets subclass this class
 * and ensure that all children at every level have unique
 * names.
 *
 * Since the name refers to QObject's "objectName()" and
 * thus BindableParent must _ALSO_ be a QObject.
 */
class LIBLIVEWIDGETSSHARED_EXPORT BindableParent
{
    QObject* qo_this;
    static QList<BindableParent*> _u;
    static QList<QObject*> _u_qo;
    static QStringList s_classBlacklist;
    static QStringList s_objectnameBlacklist;
    static void fillBlacklist();
public:
    BindableParent(QObject* qo__this) : qo_this(qo__this) {qo_this->setProperty("bindableParentObject",QVariant::fromValue(true));_u.push_back(this);_u_qo.push_back(qo_this);}
    ~BindableParent() {_u_qo.removeAt(_u.indexOf(this)); _u.removeOne(this);}
    virtual void loadBindings(const QByteArray&);
    virtual QByteArray saveBindings();
};

}

#endif // BINDABLEPARENT_H
