/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/midibindingqt.h>
#include <live_widgets/bindableparent.h>

QList<live_widgets::BindableParent*> live_widgets::BindableParent::_u;
QList<QObject*> live_widgets::BindableParent::_u_qo;
QStringList live_widgets::BindableParent::s_classBlacklist;
QStringList live_widgets::BindableParent::s_objectnameBlacklist;

void live_widgets::BindableParent::fillBlacklist() {
    s_classBlacklist<<"QTabBar"<<"QSpinBox"<<"QComboBox"<<"QListWidget"<<"QGraphicsView";
    s_objectnameBlacklist<<"qt_scrollarea_vcontainer"<<"qt_scrollarea_hcontainer";
}

void live_widgets::BindableParent::loadBindings(const QByteArray&ba) {
    if (s_classBlacklist.isEmpty()) {
        fillBlacklist();
    }
    QDataStream ret(ba);

    QString xstr;
    ret>>xstr;
    Q_ASSERT(xstr=="BEGIN BindableParent");

    qint32 xint32;
    ret>>xint32;
    Q_ASSERT(xint32==110717);

    QObjectList objs;
    objs<<qo_this->children();
    for (int i=0;i<objs.size();i++) {
        if (s_classBlacklist.contains(objs[i]->metaObject()->className())||s_objectnameBlacklist.contains(objs[i]->objectName())) {
            continue;
        }
        if (!objs[i]->property("bindableParentObject").toBool()) {
            objs<<objs[i]->children();
        }
    }

    while (1) {
        ret>>xstr;
        if (xstr=="END BindableParent") {
            break;
        } else if (xstr=="BEGIN Binding") {
            ret>>xstr;  //xstr=objectName

            QStringList childNames;

            bool boundObjectFound=0;
            for (int i=0;i<objs.size();i++) {
                if (QString(objs[i]->metaObject()->className()).contains("Oxygen::")) {
                    continue;
                }
                QWidget* w=qobject_cast<QWidget*>(objs[i]);
                if (w) {
                    if (w->objectName().isEmpty()) {
                        qDebug();
                        qDebug() << "##############################################################################";
                        qDebug() << "BUG!!! The objectName() of a"<<objs[i]->metaObject()->className() << "of"<<qo_this->objectName() << "is EMPTY!!";
                        qDebug() << "SAVING AND LOADING MIDI BINDINGS OF UNNAMED OBJECTS IS UNDEFINED!";
                        QObject*tobj=objs[i];
                        while (tobj->parent()) {
                            qDebug() << "Parent:"<<tobj->parent()->objectName() << "of type"<<tobj->parent()->metaObject()->className();
                            tobj=tobj->parent();
                        }
                        qDebug() << "##############################################################################";
                        qDebug();
                    }
                    if (childNames.contains(w->objectName())) {

                        qDebug();
                        qDebug() << "##############################################################################";
                        qDebug() << "BUG!!! The name"<<w->objectName() << ",which is a"<<objs[i]->metaObject()->className() << "of"<<qo_this->objectName() << "is NON-UNIQUE!!";
                        qDebug() << "SAVING AND LOADING MIDI BINDINGS OF UNNAMED OBJECTS IS UNDEFINED!";
                        QObject*tobj=objs[i];
                        while (tobj->parent()) {
                            qDebug() << "Parent:"<<tobj->parent()->objectName() << "of type"<<tobj->parent()->metaObject()->className();
                            tobj=tobj->parent();
                        }
                        qDebug() << "##############################################################################";
                        qDebug();

                    }

                    childNames.push_back(w->objectName());

                    //we can be inefficient, because n(max)=88... I think.
                    if (w->objectName()==xstr) {
                        boundObjectFound=1;
                        qint32 ptype,btype,key;
                        bool quant,control;
                        QString data,inID;
                        live::ObjectPtr obj=0;
                        ret>>ptype>>btype>>quant>>data>>key>>inID>>control;
                        obj=live::object::request(inID,live::MidiOnly|live::InputOnly|live::NoRefresh);
                        Q_ASSERT(obj);

                        new MidiBindingQt(w,
                                          static_cast<live::MidiBinding::GuiType>(ptype),
                                          static_cast<live::MidiBinding::BindingType>(btype),
                                          quant,
                                          data,
                                          key,
                                          obj,
                                          control);
                        ret>>xstr;
                        Q_ASSERT(xstr=="END Binding");
                        break;
                    }
                }
            }
            if (!boundObjectFound) {
                qCritical() << "Warning: bound object not found for "<<xstr;
            }
            Q_ASSERT(boundObjectFound);
        } else if (xstr=="BEGIN Child BindableParent") {
            ret>>xstr;  //xstr=objectName
            bool foundObject=0;
            for (int i=0;i<objs.size();i++) {
                qDebug() << "There's a child named"<<objs[i]->objectName() << "of type"<<objs[i]->metaObject()->className();
                if (objs[i]->objectName()==xstr) {
                    BindableParent* bp=0;
                    /*NO DYNAMIC CAST!!!*/
                    for (int j=0;j<_u_qo.size();j++) {
                        if (objs[i]==_u_qo[j]) {
                            bp=_u[j];
                            break;
                        }
                    }
//                    Q_ASSERT(bp);
                    if (!bp) continue;
                    QByteArray ba;
                    ret>>ba;
                    bp->loadBindings(ba);

                    ret>>xstr;
                    Q_ASSERT(xstr=="END Child BindableParent");
                    foundObject=1;
                    break;
                }
            }
            if (!foundObject) {
                qDebug() << "Could not find "<<xstr<<":(";
                qFatal("Thus, I'm confused and must quit.");
            }
        } else qFatal("Malformed command in BindableParent section of the LIVE file.");
    }
}

QByteArray live_widgets::BindableParent::saveBindings() {
    if (s_classBlacklist.isEmpty()) {
        fillBlacklist();
    }
    QByteArray ba;
    QDataStream ret(&ba,QIODevice::WriteOnly);
    ret<<(QString)"BEGIN BindableParent";
    ret<<(qint32)110717;
    QStringList childNames;

    QObjectList objs;
    objs<<qo_this->children();
    for (int i=0;i<objs.size();i++) {
        if (!objs[i]->property("bindableParentObject").toBool()) {
            if (s_classBlacklist.contains(objs[i]->metaObject()->className())||s_objectnameBlacklist.contains(objs[i]->objectName())) {
                continue;
            }
            objs<<objs[i]->children();
        }
    }

    for (int i=0;i<objs.size();i++) {
        QWidget* w=qobject_cast<QWidget*>(objs[i]);
        if (w) {
            qDebug() << "ObjectName():"<<w->objectName() << "of type"<<w->metaObject()->className();
            if (QString(objs[i]->metaObject()->className()).contains("Oxygen::")) {
                continue;
            }
            if (w->objectName().isEmpty()) {
                qDebug();
                qDebug() << "##############################################################################";
                qDebug() << "BUG!!! The objectName() of a"<<objs[i]->metaObject()->className() << "of"<<qo_this->objectName() << "is EMPTY!!";
                qDebug() << "SAVING AND LOADING MIDI BINDINGS OF UNNAMED OBJECTS IS UNDEFINED!";
                QObject*tobj=objs[i];
                while (tobj->parent()) {
                    qDebug() << "Parent:"<<tobj->parent()->objectName() << "of type"<<tobj->parent()->metaObject()->className();
                    tobj=tobj->parent();
                }
                qDebug() << "##############################################################################";
                qDebug();
            }
            if (childNames.contains(w->objectName())) {

                qDebug();
                qDebug() << "##############################################################################";
                qDebug() << "BUG!!! The name"<<w->objectName() << ",which is a"<<objs[i]->metaObject()->className() << "of"<<qo_this->objectName() << "is NON-UNIQUE!!";
                qDebug() << "SAVING AND LOADING MIDI BINDINGS OF UNNAMED OBJECTS IS UNDEFINED!";
                QObject*tobj=objs[i];
                while (tobj->parent()) {
                    qDebug() << "Parent:"<<tobj->parent()->objectName() << "of type"<<tobj->parent()->metaObject()->className();
                    tobj=tobj->parent();
                }
                qDebug() << "##############################################################################";
                qDebug();

            }
            childNames.push_back(w->objectName());
            //we can be inefficient, because n(max)=88... I think.
            for (int i=0;i<live::MidiBinding::universe.size();i++) {
                if (live::MidiBinding::universe[i]->guiObject==w) {
                    ret<<(QString)"BEGIN Binding";
                    ret<<(QString)w->objectName();
                    ret<<(qint32)live::MidiBinding::universe[i]->parentType;
                    ret<<(qint32)live::MidiBinding::universe[i]->type;
                    MidiBindingQt* mbq=qobject_cast<MidiBindingQt*>(live::MidiBinding::universe[i]);
                    Q_ASSERT(mbq);
                    ret<<(bool)mbq->s_quantized;
                    ret<<(QString)live::MidiBinding::universe[i]->data;
                    ret<<(qint32)live::MidiBinding::universe[i]->key;
                    ret<<(QString)live::MidiBinding::universe[i]->obj->name();
                    ret<<(bool)live::MidiBinding::universe[i]->controller;
                    ret<<(QString)"END Binding";

                }
            }
        }
        BindableParent* bp=0;
        /*NO DYNAMIC CAST!!!*/
        for (int j=0;j<_u_qo.size();j++) {
            if (objs[i]==_u_qo[j]) {
                bp=_u[j];
                break;
            }
        }
        if (bp) {
            ret<<(QString)"BEGIN Child BindableParent";
            ret<<objs[i]->objectName();
            ret<<bp->saveBindings();
            ret<<(QString)"END Child BindableParent";
        }
    }
    ret<<(QString)"END BindableParent";
    return ba;
}
