/*******************************************************

    Part of the Creator Live Music Production Suite.
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2012

                  All rights reserved.

*******************************************************/

#ifndef MIDIBINDINGQT_H
#define MIDIBINDINGQT_H

#include "live/midibinding"
#include <QComboBox>
#include <QAbstractButton>
#include <QLineEdit>
#include <QAction>
#include <QAbstractSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMutex>
#include "liblivewidgets_global.h"

namespace live_widgets {

class LIBLIVEWIDGETSSHARED_EXPORT MidiBindingQt : public live::MidiBinding {
    Q_OBJECT
protected:
    bool s_quantized;
public:
    LIVE_MIDI
    LIVE_OUTPUT
    bool mOn() const{ return 1; } bool aOn() const { return 0; }
    friend class MidiBinding;
    friend class BindableParent;

    MidiBindingQt(QObject* cGuiObject,GuiType cparentType,BindingType ctype,bool quantized,QString cdata="",int ckey=-1,live::ObjectPtr parent=0, bool controller=0) :
        MidiBinding(cGuiObject,cparentType,ctype,cdata,ckey,parent,controller), s_quantized(quantized) {
        QObject* qtGuiObject=static_cast<QObject*>(cGuiObject);
        connect(qtGuiObject,SIGNAL(destroyed()),this,SLOT(deleteLater()));
    }

protected:
    virtual void doAction(int ccval=-1) {
        live::lthread::ui();
        QComboBox *cb = NULL;
        QAbstractSlider *s = NULL;
        switch( type ) {
        case BindingNull:
            break;
        case BindingClick:
            if ( parentType == GuiAbstractButton ) {
                if(s_quantized) {
                    new live::QuantizedAction(static_cast<QAbstractButton*>(guiObject),SLOT(click()),2);  //quantize to half the beat
                } else {
                    ( static_cast<QAbstractButton *>(guiObject) )->click();
                }
            }
            if ( parentType == GuiAction ) {
                if(s_quantized) {
                    new live::QuantizedAction( static_cast<QAction *>(guiObject),SLOT(trigger()),2);  //quantize to half the beat
                } else {
                    ( static_cast<QAction *>(guiObject) )->trigger();
                }
            }
            break;
        case BindingToggle:
            if ( parentType == GuiAbstractButton ) {
                if(s_quantized) {
                    new live::QuantizedAction(static_cast<QAbstractButton *>(guiObject),SLOT(click()),2);
                } else {
                    ( static_cast<QAbstractButton *>(guiObject) )->click();
                }
            }
            break;
        case BindingSetCurrentIndex:
            if ( parentType != GuiComboBox ) break;
            cb = static_cast<QComboBox *>(guiObject);

            for ( int i = 0; i < cb->count(); i++ ) {
                if ( cb->itemText(i) == data ) {
                    if(s_quantized) {
                        new live::QuantizedAction(cb,SLOT(setCurrentIndex(int)),2,i);
                    } else {
                        cb->setCurrentIndex(i);
                    }
                }
            }

            break;
        case BindingSetText:
            if ( parentType == GuiComboBox ) {
                cb = static_cast<QComboBox *>(guiObject);
                if ( cb->lineEdit() ) {
                    if(s_quantized) {
                        new live::QuantizedAction(cb->lineEdit(),SLOT(setText(QString)),2,data);
                    } else {
                        cb->lineEdit()->setText( data );
                    }
                }
                break;
            } else if ( parentType == 3 ) {
                s = static_cast<QAbstractSlider *>(guiObject);
                if(s_quantized) {
                    new live::QuantizedAction(s,SLOT(setValue(int)),2,data.toInt());
                } else {
                    s->setValue( data.toInt() );
                }
            }
        case BindingStepUp:
            if ( parentType == 2 ) {
                if(s_quantized) {
                    new live::QuantizedAction(static_cast<QAbstractSpinBox*>(guiObject),SLOT(stepUp()),2);
                } else {
                    ( static_cast<QAbstractSpinBox *>(guiObject) )->stepUp();
                }
            } else if ( parentType == 3 ) {
                if(s_quantized) {
                    QAbstractSlider*as=static_cast<QAbstractSlider*>(guiObject);
                    new live::QuantizedAction(as,SLOT(setValue(int)),
                                        float(as->value()+as->singleStep()));    //best we can get easily
                } else {
                    ( static_cast<QAbstractSlider *>(guiObject) )->triggerAction( QAbstractSlider:: SliderSingleStepAdd );
                }
            }
            break;
        case BindingStepDown:
            if ( parentType == 2 ) {
                if(s_quantized) {
                    new live::QuantizedAction(static_cast<QAbstractSpinBox*>(guiObject),SLOT(stepDown()),2);
                } else {
                    ( static_cast<QAbstractSpinBox *>(guiObject) )->stepDown();
                }
            } else if ( parentType == 3 ) {
                if(s_quantized) {
                    QAbstractSlider*as=static_cast<QAbstractSlider*>(guiObject);
                    new live::QuantizedAction(as,SLOT(setValue(int)),
                                        float(as->value() - as->singleStep()));    //best we can get easily
                } else {
                    ( static_cast<QAbstractSlider *>(guiObject) )->triggerAction( QAbstractSlider:: SliderSingleStepSub );
                }
            }
            break;
//        case BindingSeqSetPos:
//        {
//            SequencerApp* app=static_cast<SequencerApp*>(guiObject);
//            if(s_quantized)
//            {
//                new live::QuantizedAction(app,SLOT(setPos(int)),2,data.toInt());
//            }
//            else
//            {
//                app->setPos(data.toInt());
//            }
//        }
        case BindingSeqSetPos:
            qFatal("BindingSeqSetPos Depricated");
        case BindingSlider: {
            if(parentType==GuiAbstractSpinBox) {
                // FIXME: Are you sure it's a plain SpinBox?
                QSpinBox*sb=static_cast<QSpinBox*>(guiObject);
                if(sb) {
                    sb->setValue(static_cast<int>((float)ccval/127.0*(float)(sb->maximum()-sb->minimum()))+sb->minimum());
                }
            } else if (parentType==GuiAbstractSlider) {
                QAbstractSlider* as=static_cast<QAbstractSlider*>(guiObject);
                as->setValue(static_cast<int>((float)ccval/127.0*(float)(as->maximum()-as->minimum()))+as->minimum());
            }
        }
        case InvalidBinding:
        case NumberOfBindingTypes:
            TCRASH();
        } // switch
    }
};

class LIBLIVEWIDGETSSHARED_EXPORT MidiBindingQtSys : public QObject {
    Q_OBJECT
public:
    static MidiBindingQtSys* singleton;
    static void addWidget(QWidget* w);
    static void delWidget(QWidget* w);
private:
    QList<QWidget*> widgets;
    QMenu* currentCM;
    QWidget* activeWidget;
    live::MidiBinding::GuiType activeWidgetType;
public:
    MidiBindingQtSys()
      : widgets()
      , currentCM(0)
      , activeWidget(0)
      , activeWidgetType(live::MidiBinding::InvalidType)
      {
    }

    virtual ~MidiBindingQtSys();

public slots:
    void addWidgetReal(QWidget* w);
    void delWidgetReal(QObject* w);
public slots:
    void showContextMenu( QPoint ); // point is ignored
    void bindClick(bool q=0);
    void bindToggle(bool q=0);
    void bindSetCurrentIndex(bool q=0);
    void bindSetText(bool q=0);
    void bindStepUp(bool q=0);
    void bindStepDown(bool q=0);
    void bindMenuAction(bool q=0);
//    void bindSeqCurrentPos(bool q=0);
//    void bindSeqChoosePos(bool q=0);

    void bindClickQ();
    void bindToggleQ();
    void bindSetCurrentIndexQ();
    void bindSetTextQ();
    void bindStepUpQ();
    void bindStepDownQ();
    void bindMenuActionQ();
//    void bindSeqCurrentPosQ();
//    void bindSeqChoosePosQ();
    void bindSlider();

    void removeBind();

private:
    MidiBindingQtSys(const MidiBindingQtSys&)
      : QObject()
      , widgets()
      , currentCM(0)
      , activeWidget(0)
      , activeWidgetType(live::MidiBinding::InvalidType)
      { TCRASH();
    }

    MidiBindingQtSys& operator=(const MidiBindingQtSys&) {
        TCRASH();
        return *this;
    }
};

}

#endif // MIDIBINDINGQT_H
