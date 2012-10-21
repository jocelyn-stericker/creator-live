/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#include <live_widgets/midibindingqt.h>

#include <QInputDialog>
#include <QPushButton>
#include <QMenu>
#include <QApplication>

using namespace live;
using namespace live_widgets;

LIBLIVEWIDGETSSHARED_EXPORT MidiBindingQtSys* MidiBindingQtSys::singleton=0;

namespace MidiBindingQtSysPrivate {
    void bindClick(QWidget* widget, live::MidiBinding::GuiType widgetType,bool q ) {
        live::MidiBinding::universe.push_back( new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingClick,q) );
    }

    void bindToggle(QWidget* widget, live::MidiBinding::GuiType widgetType,bool q ) {
        live::MidiBinding::universe.push_back( new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingToggle,q) );
    }

    void bindSetCurrentIndex(QWidget* widget, live::MidiBinding::GuiType widgetType,bool q) {
        if (widgetType != 1) return;

        bool ok;
        QComboBox *cb = qobject_cast<QComboBox *>( widget );

        QStringList a;
        for ( int i = 0; i < cb->count(); i++ ) {
            a.append( cb->itemText(i) );
        }

        live::MidiBinding::universe.push_back( new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingSetCurrentIndex,q,
                                   QInputDialog::getItem(qApp->activeWindow() , "Creator Live",
                                                         "Bind next MIDI event to:", a, 0, false, &ok) )
                );

        if (!ok) {
            live::MidiBinding::universe.takeLast().detach();
        }
    }

    void bindSetText(QWidget* widget, live::MidiBinding::GuiType widgetType,bool q) {
        bool ok = true;
        if (widgetType == 1) {
            QComboBox *cb = static_cast<QComboBox *>( widget );

            live::MidiBinding::universe.push_back(new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingSetText,q,
                                       QInputDialog::getText(qobject_cast<QWidget *>(qApp->activeWindow()), "Creator Live",
                                                             "Bind next MIDI event to:", QLineEdit::Normal, cb->currentText(), &ok) )
                    );
        } else if (widgetType == 3 ) {
            QAbstractSlider *s = static_cast<QAbstractSlider *>( widget );

            live::MidiBinding::universe.push_back( new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingSetText,q,
                                       QString::number( QInputDialog::getInt(
                                                            qobject_cast<QWidget *>(qApp->activeWindow()),"Creator Live",
                                                            "Bind next MIDI event to:", s->value(),
                                                            s->minimum(), s->maximum(), s->singleStep(), &ok ) ) )
                    );
        } else {
            Q_ASSERT(0);
        }

        if (!ok) {
            live::MidiBinding::universe.takeLast().detach();
        }
    }

    void bindStepDown(QWidget* widget, live::MidiBinding::GuiType widgetType,bool q ) {
        live::MidiBinding::universe.push_back( new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingStepDown,q) );
    }

    void bindStepUp(QWidget* widget, live::MidiBinding::GuiType widgetType,bool q ) {
        live::MidiBinding::universe.push_back( new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingStepUp,q) );
    }

    void bindMenuAction(QAction* action, QWidget* widget, live::MidiBinding::GuiType widgetType,bool q ) {
        Q_ASSERT(action);
        Q_UNUSED(widgetType);

        QString a = action->text();
        a.remove("Bind next MIDI event to action ");

        QPushButton *pb = qobject_cast<QPushButton *>( widget );
        if ( pb && pb->menu() ) for ( int i = 0; i < pb->menu()->actions().size(); i++ ) {
            if ( pb->menu()->actions().at( i )->text() == a ) {
                live::MidiBinding::universe.push_back(new live_widgets::MidiBindingQt( pb->menu()->actions().at( i ), live::MidiBinding::GuiAction, live::MidiBinding::BindingClick,q, "" ));
            }
        }
    }

//    void bindSeqCurrentPosition(SequencerApp* app, MidiBinding::GuiType widgetType,bool q ) {
//        Q_ASSERT(app);
//        Q_UNUSED(widgetType);

//        MidiBinding::universe.push_back(new MidiBindingQt( app, MidiBinding::Sequencer, MidiBinding::BindingSeqSetPos, q,QString::number(app->pos()) ));
//    }

//    void bindSeqChoosePosition(SequencerApp* app, MidiBinding::GuiType widgetType,bool q ) {
//        Q_ASSERT(app);
//        Q_UNUSED(widgetType);

//        MidiBinding::universe.push_back(new MidiBindingQt( app, MidiBinding::Sequencer, MidiBinding::BindingSeqSetPos,q,
//                                                           QString::number( QInputDialog::getInt(
//                                                                                qobject_cast<QWidget *>(qApp->activeWindow()),"Creator Live",
//                                                                                "Enter desired position in msec:", 0))));

//    }

    void bindSlider(QWidget* widget, live::MidiBinding::GuiType widgetType) {
        live::MidiBinding::universe.push_back( new live_widgets::MidiBindingQt( widget, widgetType, live::MidiBinding::BindingSlider,0) );
    }

    int getWidgetType( QObject *object ) {
        if ( qobject_cast<QAbstractButton *>(object) ) return 0;
        if ( qobject_cast<QComboBox *>(object) ) return 1;
        if ( qobject_cast<QAbstractSpinBox *>(object) ) return 2;
        if ( qobject_cast<QAbstractSlider *>(object) ) return 3;
        if ( qobject_cast<QAction *>(object) ) return 4;
//        if ( qobject_cast<SequencerGraph*>(object)) return 5;
        return -1;
    }
}

void live_widgets::MidiBindingQtSys::addWidget(QWidget* widget) {
    if (!singleton) singleton = new MidiBindingQtSys();
    singleton->addWidgetReal(widget);
}

void live_widgets::MidiBindingQtSys::addWidgetReal(QWidget *widget) {
    for ( int i = 0; i < widget->children().size(); i++ ) {
        QWidget* child = qobject_cast< QWidget* >( widget->children()[i] );
        if ( child ) addWidget( child );
    }

    if ( MidiBindingQtSysPrivate::getWidgetType( widget ) != -1 ) {
        widgets.push_back( widget );
        widget->setContextMenuPolicy( Qt::CustomContextMenu );
        connect( widget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(delWidgetReal(QObject*)));
    }
    // silent fail.
}

void live_widgets::MidiBindingQtSys::delWidget(QWidget* widget) {
    if (!singleton) singleton = new MidiBindingQtSys();
    singleton->delWidgetReal(widget);
}

void live_widgets::MidiBindingQtSys::delWidgetReal(QObject *object) {
    QWidget* widget = qobject_cast<QWidget*>(object);
    for ( int i = 0; i < widgets.size(); i++ ) {
        if ( widgets[i] == object ) {
            if (widget) { //else, it's already been done.
                //No need for:
//                disconnect( widget, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(showContextMenu(QPoint)) );
            }
            widgets.removeAt( i );
            i--;
        }
    }
    for ( int i = 0;i<live::MidiBinding::universe;i++) {
        if ( object == live::MidiBinding::universe[i]->guiObject ) {
            live::MidiBinding::universe.takeAt(i).detach();
            i--;
        }
    }
}

void live_widgets::MidiBindingQtSys::showContextMenu( QPoint ) {
    QWidget *widget = qobject_cast<QWidget *>( sender() );

    if (!widget) {
        return;
    }

    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }

    for (int i=0;i<live::MidiBinding::universe;i++) {
        if (widget==live::MidiBinding::universe[i]->guiObject) {
            activeWidget = widget;
            activeWidgetType = (live::MidiBinding::GuiType) MidiBindingQtSysPrivate::getWidgetType(widget);
            currentCM = new QMenu;
            currentCM->addAction("Remove binding",this,SLOT(removeBind()));
            currentCM->exec( QCursor::pos() );
            return; //// !!
        }
    }

    switch ( MidiBindingQtSysPrivate::getWidgetType( widget ) ) {
    case 0: { // ABSTRACT BUTTON
            QAbstractButton *a = 0;
            activeWidget = widget;
            activeWidgetType = live::MidiBinding::GuiAbstractButton;
            currentCM = new QMenu;
            a = static_cast<QAbstractButton *>(widget);
            if ( a->isCheckable() ) {
                currentCM->addAction( "Bind next MIDI event to toggle", this, SLOT(bindToggle()) );
                currentCM->addAction( "Bind next MIDI event to toggle (quantized)", this, SLOT(bindToggleQ()) );
            } else {
                currentCM->addAction( "Bind next MIDI event to click", this, SLOT(bindClick()) );
                currentCM->addAction( "Bind next MIDI event to click (quantized)", this, SLOT(bindClickQ()) );
            }
            QPushButton *b = qobject_cast<QPushButton *>(widget);
            if ( b && b->menu() ) {
                for ( int i = 0; i < b->menu()->actions().size(); i++ ) {
                    currentCM->addAction( "Bind next MIDI event to action " + b->menu()->actions().at( i )->text(), this,
                                          SLOT(bindMenuAction()) );
                    currentCM->addAction( "Bind next MIDI event to action " + b->menu()->actions().at( i )->text()+" (quantized)", this,
                                          SLOT(bindMenuActionQ()) );
                }
            }
        }
        break;
    case 1: // COMBOBOX
        activeWidget = widget;
        activeWidgetType = live::MidiBinding::GuiComboBox;
        currentCM = new QMenu;
        currentCM->addAction( "Bind next MIDI event to set index", this, SLOT(bindSetCurrentIndex()) );
        currentCM->addAction( "Bind next MIDI event to set index (quantized)", this, SLOT(bindSetCurrentIndexQ()) );
        currentCM->addAction( "Bind next MIDI event to set text", this, SLOT(bindSetText()) );
        currentCM->addAction( "Bind next MIDI event to set text (quantized)", this, SLOT(bindSetTextQ()) );
        break;
    case 2: // SPIN BOX
        activeWidget = widget;
        activeWidgetType = live::MidiBinding::GuiAbstractSpinBox;
        currentCM = new QMenu;
        currentCM->addAction( "Bind next MIDI event to step up", this, SLOT(bindStepUp()) );
        currentCM->addAction( "Bind next MIDI event to step up (quantized)", this, SLOT(bindStepUpQ()) );
        currentCM->addAction( "Bind next MIDI event to step down", this, SLOT(bindStepDown()) );
        currentCM->addAction( "Bind next MIDI event to step down (quantized)", this, SLOT(bindStepDownQ()) );
        currentCM->addAction( "Bind MIDI slider/knob",this,SLOT(bindSlider()));
        break;
    case 3: // SLIDER
        activeWidget = widget;
        activeWidgetType = live::MidiBinding::GuiAbstractSlider;
        currentCM = new QMenu;
        currentCM->addAction( "Bind next MIDI event to step up", this, SLOT(bindStepUp()) );
        currentCM->addAction( "Bind next MIDI event to step up (quantized)", this, SLOT(bindStepUpQ()) );
        currentCM->addAction( "Bind next MIDI event to step down", this, SLOT(bindStepDown()) );
        currentCM->addAction( "Bind next MIDI event to step down (quantized)", this, SLOT(bindStepDownQ()) );
        currentCM->addAction( "Bind next MIDI event to set value", this, SLOT(bindSetText()) );
        currentCM->addAction( "Bind next MIDI event to set value (quantized)", this, SLOT(bindSetTextQ()) );
        currentCM->addAction( "Bind MIDI slider/knob",this,SLOT(bindSlider()));
        break;
    case 5: // SEQ
        activeWidget = widget;
        activeWidgetType = live::MidiBinding::Sequencer;
        currentCM = new QMenu;
        currentCM->addAction( "Bind next MIDI event to set to current position", this, SLOT(bindSeqCurrentPos()));
        currentCM->addAction( "Bind next MIDI event to set to current position (quantized)", this, SLOT(bindSeqCurrentPosQ()));
        currentCM->addAction( "Bind next MIDI event to set to a certain position", this, SLOT(bindSeqChoosePos()));
        currentCM->addAction( "Bind next MIDI event to set to a certain position (quantized)", this, SLOT(bindSeqChoosePosQ()));
        break;
    default:
        return;
    }

    Q_ASSERT(currentCM);
    currentCM->exec( QCursor::pos() );
}

void live_widgets::MidiBindingQtSys::bindClick(bool q) {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindClick(activeWidget,activeWidgetType,q);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }
}

void live_widgets::MidiBindingQtSys::bindClickQ() { bindClick(1); }

void live_widgets::MidiBindingQtSys::bindToggle(bool q) {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindToggle(activeWidget,activeWidgetType,q);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }
}

void live_widgets::MidiBindingQtSys::bindToggleQ() { bindToggle(1); }

void live_widgets::MidiBindingQtSys::bindSetCurrentIndex(bool q) {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindSetCurrentIndex(activeWidget,activeWidgetType,q);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }
}

void live_widgets::MidiBindingQtSys::bindSetCurrentIndexQ() { bindSetCurrentIndex(1); }

void live_widgets::MidiBindingQtSys::bindSetText(bool q) {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindSetText(activeWidget,activeWidgetType,q);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }
}

void live_widgets::MidiBindingQtSys::bindSetTextQ() { bindSetText(1); }

void live_widgets::MidiBindingQtSys::bindStepUp(bool q) {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindStepUp(activeWidget,activeWidgetType,q);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }

}

void live_widgets::MidiBindingQtSys::bindStepUpQ() { bindStepUp(1); }

void live_widgets::MidiBindingQtSys::bindStepDown(bool q) {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindStepDown(activeWidget,activeWidgetType,q);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }
}

void live_widgets::MidiBindingQtSys::bindStepDownQ() { bindStepDown(1); }

void live_widgets::MidiBindingQtSys::bindMenuAction(bool q) {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindMenuAction(qobject_cast<QAction*>(sender()),activeWidget,activeWidgetType,q);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }
}

void live_widgets::MidiBindingQtSys::bindMenuActionQ() { bindMenuAction(1); }

//void live_widgets::MidiBindingQtSys::bindSeqCurrentPos(bool q)
//{
//    Q_ASSERT(activeWidget);
//    MidiBindingQtSysPrivate::bindSeqCurrentPosition(static_cast<SequencerGraph*>(activeWidget)->app,activeWidgetType,q);
//    if ( currentCM != 0 ) {
//        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
//        delete currentCM;
//        currentCM = 0;
//    }
//}

//void live_widgets::MidiBindingQtSys::bindSeqCurrentPosQ() { bindSeqCurrentPos(1); }

//void live_widgets::MidiBindingQtSys::bindSeqChoosePos(bool q)
//{
//    Q_ASSERT(activeWidget);
//    MidiBindingQtSysPrivate::bindSeqChoosePosition(static_cast<SequencerGraph*>(activeWidget)->app,activeWidgetType,q);
//    if ( currentCM != 0 ) {
//        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
//        delete currentCM;
//        currentCM = 0;
//    }
//}

//void live_widgets::MidiBindingQtSys::bindSeqChoosePosQ() { bindSeqChoosePos(1); }

void live_widgets::MidiBindingQtSys::bindSlider() {
    Q_ASSERT(activeWidget);
    MidiBindingQtSysPrivate::bindSlider(activeWidget,activeWidgetType);
    if ( currentCM != 0 ) {
        for (int i = 0; i < currentCM->actions().size(); i++) currentCM->actions()[i]->disconnect();
        delete currentCM;
        currentCM = 0;
    }
}

void live_widgets::MidiBindingQtSys::removeBind() {
    Q_ASSERT(activeWidget);
    for (int i=0;i<live::MidiBinding::universe;i++) {
        if (activeWidget==live::MidiBinding::universe[i]->guiObject) {
            live::MidiBinding::universe.takeAt(i).detach();
            return;
        }
    }
}

live_widgets::MidiBindingQtSys::~MidiBindingQtSys() {
    singleton=0;
    delete currentCM;

    while (live::MidiBinding::universe.size()) {
        live::MidiBinding::universe.takeFirst().detach();
    }
}
